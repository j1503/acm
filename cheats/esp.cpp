#include "esp.hpp"

#include "../drawing.hpp"

const char* cheats::esp::name() const noexcept
{
    return "ESP";
}

void cheats::esp::run() noexcept
{
    auto curropt = this->options();
    if (curropt->active) {
        drawing::before2D();
        playerent* lp = globals::MemoryManager->localPlayer;
        for (size_t i = 0; i < *globals::MemoryManager->entityCount; ++i) {
            auto ent = (*globals::MemoryManager->entityList)[i];

            if (!ent) continue;
            if (ent == lp) continue;
            if (ent->state != CS_ALIVE) continue;
            if (ent->health <= 0) continue;
            bool ally = lp->team == ent->team;
            if (!curropt->teammates && ally) continue;
            float dist = lp->origin.dist(ent->origin);
            if (dist < 3.f) continue;

            auto head = ent->origin;
            head.z += ent->aboveeye;
            auto base = ent->origin;
            base.z -= ent->eyeheight;
            
            drawing::drawESPBox(base, head, 1.5f, ally ? colors::toColor(curropt->allyColor) : colors::toColor(curropt->enemyColor));

            if (curropt->healthbar)
                drawing::drawHealthBar(base, head, ent->health);
        }
        drawing::after2D();
    }
}

void cheats::esp::on() noexcept
{
    this->options()->active = true;
}

void cheats::esp::off() noexcept
{
    this->options()->active = false;
}

configManager::config::esp_conf* cheats::esp::options() noexcept
{
    return &(*this->profile)->esp;
}
