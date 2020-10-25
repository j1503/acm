#include "triggerbot.hpp"
#include "../memory.hpp"
#include "../input.hpp"
#include "../sdk/game.hpp"

#include <memory>

cheats::triggerbot::triggerbot()
	: cheats::cheat() {}

cheats::triggerbot::~triggerbot()
{
	this->off();
}


const char * cheats::triggerbot::name() const noexcept
{
	return "Triggerbot";
}

void cheats::triggerbot::run() noexcept
{
	auto curropt = this->options();
	if (curropt.active) {
		if (globals::MemoryManager->localPlayer) {
			playerent* lp = globals::MemoryManager->localPlayer;
			
			if ((lp->weaponsel->type == GUN_KNIFE) || (lp->weaponsel->type == GUN_GRENADE)) return; 

			int bone;
			float distance;
			/*	float raylength = 1000.f;
				vec ray(1.f, lp->yaw, lp->pitch);
				ray = angleVector(ray);
				ray.mul(raylength);
				ray.add(lp->origin); */
			playerent* ent = globals::MemoryManager->callRayIntersectEnt(&distance, &lp->origin, globals::MemoryManager->lookingAt, lp, &bone);

			if (ent) {
				bool ally = m_teammode(*globals::MemoryManager->gamemode) && (ent->team == lp->team);

				if (!curropt.friendlyfire && ally) return;

				lp->attacking = true;
			}
			else {
				if (!globals::InputManager->leftMouseDown)
					lp->attacking = false;
			}
		}
	}
}


void cheats::triggerbot::on() noexcept
{
	this->options().active = true;
}

void cheats::triggerbot::off() noexcept
{
	playerent* lp = globals::MemoryManager->localPlayer;
	if (!globals::InputManager->leftMouseDown) {
		lp->attacking = false;
	}
	this->options().active = false;
}

config_manager::config::triggerbot_conf& cheats::triggerbot::options() noexcept
{
	return globals::ConfigManager->active().triggerbot;
}