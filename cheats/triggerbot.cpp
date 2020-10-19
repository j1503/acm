#include "triggerbot.hpp"
#include "../memory.hpp"
#include "../input.hpp"

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
	if (this->options()->active) {
		if (globals::MemoryManager->localPlayer) {
			playerent* lp = globals::MemoryManager->localPlayer;
			// TODO FIX
			// if ((lp->weaponsel->type == GUN_KNIFE) || (lp->weaponsel->type == GUN_GRENADE)) return; 

			int bone;
			float distance;
			/*	float raylength = 1000.f;
				vec ray(1.f, lp->yaw, lp->pitch);
				ray = angleVector(ray);
				ray.mul(raylength);
				ray.add(lp->origin); */
			playerent* ent = globals::MemoryManager->callRayIntersectEnt(&distance, &lp->origin, globals::MemoryManager->lookingAt, lp, &bone);
			
			if(lp->weaponsel->info.isauto)

			if (ent) {
				if (!this->options()->friendlyfire && (ent->team == lp->team))
					return;
				globals::MemoryManager->executeRet("attack");
				//globals::InputManager->sendMouseEvent(SDL_BUTTON_LEFT, SDL_PRESSED);
				//lp->attacking = true;
			}
			else {
				if (!globals::InputManager->leftMouseDown);
					//globals::MemoryManager->execute("attack", 0);
			}

			//else {
			//	lp->attacking = false;
			//	if (globals::InputManager->leftMouseDown) { // rapid-fire as a side effect, lol
			//		lp->attacking = true;
			//	}
			//}
		}
	}
}


void cheats::triggerbot::on() noexcept
{
	this->options()->active = true;
}

void cheats::triggerbot::off() noexcept
{
	this->options()->active = false;
}

configManager::config::triggerbot_conf* cheats::triggerbot::options() noexcept
{
	return &(*this->profile)->triggerbot;
}