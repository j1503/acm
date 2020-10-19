#include "tracelines.hpp"
#include <memory>
#include "../drawing.hpp"
#include "../sdk/entity.hpp"

cheats::tracelines::tracelines()
	: cheats::cheat()
{
	if (this->options()->active) {
		this->on();
	}
}

cheats::tracelines::~tracelines()
{
	this->off();
}

const char * cheats::tracelines::name() const noexcept
{
	return "Tracelines";
}

void cheats::tracelines::run() noexcept
{
	if (this->options()->active && globals::MemoryManager->entityList) {
		drawing::before2D();

		playerent* lp = globals::MemoryManager->localPlayer;
		float w = (float)*globals::MemoryManager->screenWidth;
		float h = (float)*globals::MemoryManager->screenHeight;
		for (size_t i = 1; i < *globals::MemoryManager->entityCount; ++i) {
			playerent* curr = (*globals::MemoryManager->entityList)[i];
			if (curr) {
				if(curr->health > 0){
					bool ally = curr->team == lp->team;
					if(!this->options()->teammates && ally)
						continue;
					vec pos = curr->origin;
					pos.z -= curr->eyeheight;
					auto poss = drawing::worldToScreen(pos);
					if (poss.has_value()) {
						drawing::drawLine({ w/2.f, h }, { poss->x, poss->y }, this->options()->linewidth, 
							ally ? colors::toColor(this->options()->allyColor) : colors::toColor(this->options()->enemyColor));
					}
				}
			}
		}
		drawing::after2D();
	}
}

void cheats::tracelines::on() noexcept
{
	this->options()->active = true;
}

void cheats::tracelines::off() noexcept
{
	this->options()->active = false;
}

configManager::config::tracelines_conf* cheats::tracelines::options() noexcept
{
	return &(*this->profile)->tracelines;
}
