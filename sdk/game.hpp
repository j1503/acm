#ifndef ACM_SDK_GAME_HPP
#define ACM_SDK_GAME_HPP
#pragma once

#include "entity.hpp"
#include <cstdint>

// gamemodes
#define m_lms(gamemode)         (gamemode==3 || gamemode==4 || gamemode==20)
#define m_ctf(gamemode)         (gamemode==5)
#define m_pistol(gamemode)      (gamemode==6 || gamemode==16 || gamemode==18)
#define m_lss(gamemode)         (gamemode==9 || gamemode==17 || gamemode==19)
#define m_osok(gamemode)        ((gamemode>=10 && gamemode<=12) || gamemode==21)
#define m_htf(gamemode)         (gamemode==13)
#define m_ktf(gamemode)         (gamemode==14 || gamemode==15)

#define m_teammode(gamemode)    (gamemode==0 || gamemode==4 || gamemode==5 || gamemode==7 || gamemode==11 || gamemode==13 || gamemode==14 || gamemode==16 || gamemode==17 || gamemode==20 || gamemode==21)
#define m_flags       (m_ctf || m_htf || m_ktf)


#define LARGEST_FACTOR 11

// some random declarations
struct flaginfo
{
    int team;
    entity* flagent;
    int actor_cn;
    playerent* actor;
    vec pos;
    int state; // one of CTFF_*
    bool ack;
    flaginfo() {}
};

struct entitystats_s
{
    int entcnt[MAXENTTYPES];                // simple count for every basic ent type
    int spawns[3];                          // number of spawns with valid attr2
    int flags[2];                           // number of flags with valid attr2
    int flagents[2];                        // entity indices of the flag ents
    int pickups;                            // total number of pickup entities
    int pickupdistance[LARGEST_FACTOR + 1]; // entity distance values grouped logarithmically
    int flagentdistance;                    // distance of the two flag entities, if existing
    int modes_possible;                     // bitmask of possible gamemodes, depending on flag and spawn entities
    bool hasffaspawns;                      // has enough ffa spawns
    bool hasteamspawns;                     // has enough team spawns
    bool hasflags;                          // has exactly 2 valid flags
    int unknownspawns, unknownflags;        // attr2 is not valid
    int firstclip;                          // index of first clipped entity
    short first[MAXENTTYPES], last[MAXENTTYPES]; // first and last occurence of every basic ent type
};

struct mapdim_s
{   //   0   2   1   3     6         7
    int x1, x2, y1, y2, minfloor, maxceil;       // outer borders (points to last used cube)
    //    4      5
    int xspan, yspan;                            // size of area between x1|y1 and x2|y2
    float xm, ym;                                // middle of the map
};

struct serverstate { int autoteam; int mastermode; int matchteamsize; void reset() { } };

#endif
