#include "snaplines.hpp"
#include <memory>
#include "../drawing.hpp"
#include "../sdk/entity.hpp"
#include "../sdk/game.hpp"

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
			playerent* ent = (*globals::MemoryManager->entityList)[i];
			if (ent) {
				if(ent->health > 0){
					bool ally = m_teammode(*globals::MemoryManager->gamemode) && (ent->team == lp->team);
					if(!this->options()->teammates && ally)
						continue;
					vec pos = ent->origin;
					pos.z -= ent->eyeheight;
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
