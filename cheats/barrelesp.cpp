#include "barrelesp.hpp"
#include "../drawing.hpp"
#include "../sdk/entity.hpp"

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
	if (this->options()->active) {
		drawing::before2D();
		playerent* lp = globals::MemoryManager->localPlayer;
		for(size_t i = 1; i < *globals::MemoryManager->entityCount; ++i){
			playerent* curr = (*globals::MemoryManager->entityList)[i];
			if (curr) {
				if(curr->health > 0){
					bool ally = curr->team == lp->team;
					if (!this->options()->teammates && ally)
						continue;
					vec ray(1.f, curr->yaw, curr->pitch);
					vec head = curr->origin;
					head.z += 0.25f;
					ray = angleVector(ray);
					vec transform = ray;
					transform.div(5);
					ray.mul(this->options()->linelength);
					ray.add(head);
					drawing::drawLineWorld(head.add(transform), ray, this->options()->linewidth,
						ally ? colors::toColor(this->options()->allyColor) : colors::toColor(this->options()->enemyColor));
				}
			}
		}
		drawing::after2D();
	}
}

void cheats::barrelesp::on() noexcept
{
	this->options()->active = true;
}

void cheats::barrelesp::off() noexcept
{
	this->options()->active = false;
}

configManager::config::barrelesp_conf* cheats::barrelesp::options() noexcept
{
	return &(*this->profile)->barrelesp;
}
