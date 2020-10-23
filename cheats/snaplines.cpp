#include "snaplines.hpp"
#include <memory>
#include "../drawing.hpp"
#include "../sdk/entity.hpp"

cheats::snaplines::snaplines()
	: cheats::cheat()
{
	if (this->options()->active) {
		this->on();
	}
}

cheats::snaplines::~snaplines()
{
	this->off();
}

const char * cheats::snaplines::name() const noexcept
{
	return "Snaplines";
}

void cheats::snaplines::run() noexcept
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
					vec poss;
					if(drawing::worldToScreen(pos, poss)){
						drawing::drawLine({ w/2.f, h }, { poss.x, poss.y }, this->options()->linewidth, 
							ally ? colors::toColor(this->options()->allyColor) : colors::toColor(this->options()->enemyColor));
					}
				}
			}
		}
		drawing::after2D();
	}
}

void cheats::snaplines::on() noexcept
{
	this->options()->active = true;
}

void cheats::snaplines::off() noexcept
{
	this->options()->active = false;
}

configManager::config::snaplines_conf* cheats::snaplines::options() noexcept
{
	return &(*this->profile)->snaplines;
}
