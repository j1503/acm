#ifndef ACM_SDK_ENTITY_HPP
#define ACM_SDK_ENTITY_HPP
#pragma once
#include "geom.hpp"
#include "weapon.hpp"
#include <cstdint>

typedef unsigned char uchar;

enum { ENT_PLAYER = 0, ENT_BOT, ENT_CAMERA, ENT_BOUNCE };
enum { CS_ALIVE = 0, CS_DEAD, CS_SPAWNING, CS_LAGGED, CS_EDITING, CS_SPECTATE };
enum { CR_DEFAULT = 0, CR_ADMIN };
enum { SM_NONE = 0, SM_DEATHCAM, SM_FOLLOW1ST, SM_FOLLOW3RD, SM_FOLLOW3RD_TRANSPARENT, SM_FLY, SM_OVERVIEW, SM_NUM };

class worldobject
{
public:
	virtual ~worldobject() {};
};

class physent : public worldobject
{
public:
	vec origin, vel, vel_t;                         // origin, velocity
	vec deltapos, newpos;                       // movement interpolation
	float yaw, pitch, roll;             // used as vec in one place
	float pitchvel;
	float maxspeed;                     // cubes per second, 24 for player
	int timeinair;                      // used for fake gravity
	float radius, eyeheight, maxeyeheight, aboveeye;  // bounding box size
	bool inwater;
	bool onfloor, onladder, jumpnext, crouching, crouchedinair, trycrouch, cancollide, stuck, scoping, shoot;
	int lastjump;
	float lastjumpheight;
	int lastsplash;
	char move, strafe;
	uchar state, type;
	float eyeheightvel;
	int last_pos;

	physent() {}
	virtual ~physent() {}
	void resetinterp() {}
	void reset() {}
	virtual void oncollision() {}
	virtual void onmoved(const vec& dist) {}
};

struct animstate                                // used for animation blending of animated characters
{
	int anim, frame, range, basetime;
	float speed;
	animstate() { reset(); }
	void reset() { }

	bool operator==(const animstate& o) const { }
	bool operator!=(const animstate& o) const { }
};

class dynent : public physent                 // animated ent
{
public:
	bool k_left, k_right, k_up, k_down;         // see input code

	animstate prev[2], current[2];              // md2's need only [0], md3's need both for the lower&upper model
	int lastanimswitchtime[2];
	void* lastmodel[2];
	int lastrendered;

	void stopmoving() {}
	void resetanim() {}
	void reset() {}
	dynent() {}
	virtual ~dynent() {}
};

#define MAXNAMELEN 15
enum { GUN_KNIFE = 0, GUN_PISTOL, GUN_CARBINE, GUN_SHOTGUN, GUN_SUBGUN, GUN_SNIPER, GUN_ASSAULT, GUN_CPISTOL, GUN_GRENADE, GUN_AKIMBO, NUMGUNS };
struct itemstat { int add, start, max, sound; };

class playerstate
{
public:
	int health, armour;
	int primary, nextprimary;
	int gunselect;
	bool akimbo;
	int ammo[NUMGUNS], mag[NUMGUNS], gunwait[NUMGUNS];
	int pstatshots[NUMGUNS], pstatdamage[NUMGUNS];

	playerstate() {}
	virtual ~playerstate() {}
	void resetstats() {}
	itemstat* itemstats(int type) {}
	bool canpickup(int type) {}
	void additem(itemstat& is, int& v) {}
	void pickup(int type) {}
	void respawn() {}
	virtual void spawnstate(int gamemode) {}
	int dodamage(int damage, int gun) {}
};

#define HEADSIZE 0.4f

#define ROLLMOVMAX 20
#define ROLLMOVDEF 0
#define ROLLEFFMAX 30
#define ROLLEFFDEF 10

#define POSHIST_SIZE 6

struct poshist
{
	int nextupdate, curpos, numpos;
	vec pos[POSHIST_SIZE];

	poshist() {}
	const int size() const {}
	void reset() {}
	void addpos(const vec& o) {}
	const vec& getpos(int i) const {}
	void update(const vec& o, int lastmillis) {}
};

class playerent : public dynent, public playerstate
{
private:
	int curskin, nextskin[2];
public:
	int clientnum, lastupdate, plag, ping;
	uint32_t address;
	//int lifesequence;                   // sequence id for each respawn, used in damage test
	int frags, flagscore, deaths, points, tks;
	int lastaction, lastmove, lastpain, lastvoicecom, lastdeath;
	int clientrole;
	bool attacking;
	char name[260];
	int team;
	int weaponchanging;
	int nextweapon; // weapon we switch to
	int spectatemode, followplayercn;
	int eardamagemillis;
	float maxroll, maxrolleffect, movroll, effroll;  // roll added by movement and damage
	bool allowmove() { }

	weapon* weapons[NUMGUNS];
	weapon* prevweaponsel, * weaponsel, * nextweaponsel, * primweap, * nextprimweap, * lastattackweapon;

	poshist history; // Previous stored locations of this player

	const char* skin_noteam, * skin_cla, * skin_rvsf;

	float deltayaw, deltapitch, newyaw, newpitch;
	int smoothmillis;

	vec head;

	bool ignored, muted;
	bool nocorpse;

	playerent() {}
	virtual ~playerent() {}
	void damageroll(float damage) {}
	void hitpush(int damage, const vec& dir, playerent* actor, int gun) {}
	void resetspec() {}
	void respawn() {}
	void spawnstate(int gamemode) {}
	void selectweapon(int w) {}
	void setprimary(int w) {}
	void setnextprimary(int w) {}
	bool isspectating() {}
	void weaponswitch(weapon* w) {}
	int skin(int t = -1) {}
	void setskin(int t, int s) {}
};

// own code

playerent* rayIntersectEnt(int* bone, float* dist, float raylength = 1e6f);

#endif