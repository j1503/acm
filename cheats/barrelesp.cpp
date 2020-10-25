#include "barrelesp.hpp"
#include "../drawing.hpp"
#include "../sdk/entity.hpp"
#include "../sdk/game.hpp"

cheats::barrelesp::barrelesp()
	: cheats::cheat() {}

cheats::barrelesp::~barrelesp()
{
	this->off();
}

const char* cheats::barrelesp::name() const noexcept
{
	return "Barrelesp";
}

void cheats::barrelesp::run() noexcept
{
	auto curropt = this->options();
	if (curropt.active) {
		drawing::before2D();
		playerent* lp = globals::MemoryManager->localPlayer;
		for(size_t i = 1; i < *globals::MemoryManager->entityCount; ++i){
			playerent* ent = (*globals::MemoryManager->entityList)[i];
			if (ent) {
				if(ent->health > 0){
					bool ally = m_teammode(*globals::MemoryManager->gamemode) && (ent->team == lp->team);
					if (!curropt.teammates && ally)
						continue;
					vec ray(1.f, ent->yaw, ent->pitch);
					vec head = ent->origin;
					head.z += 0.25f;
					ray = angleVector(ray);
					vec transform = ray;
					transform.div(5);
					ray.mul(curropt.linelength);
					ray.add(head);
					drawing::drawLineWorld(head.add(transform), ray, curropt.linewidth,
						ally ? colors::toColor(curropt.allyColor) : colors::toColor(curropt.enemyColor));
				}
			}
		}
		drawing::after2D();
	}
}

void cheats::barrelesp::on() noexcept
{
	this->options().active = true;
}

void cheats::barrelesp::off() noexcept
{
	this->options().active = false;
}

config_manager::config::barrelesp_conf& cheats::barrelesp::options() noexcept
{
	return globals::ConfigManager->active().barrelesp;
}
