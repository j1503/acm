#include "aimbot.hpp"
#include "../drawing.hpp"

cheats::aimbot::aimbot()
    : cheats::cheat() {}

cheats::aimbot::~aimbot()
{
    this->off();
}

const char* cheats::aimbot::name() const noexcept
{
    return "Aimbot";
}

void cheats::aimbot::run() noexcept
{
    auto curropt = this->options();
    if (curropt->active) {
        if(curropt->showcircle){
        drawing::before2D();
            float cradius = (*globals::MemoryManager->screenWidth / 2.f) * tanf(curropt->fov * RAD);
            drawing::drawCircle({ *globals::MemoryManager->screenWidth/2.f, *globals::MemoryManager->screenHeight/2.f },
                cradius, 1.0f, colors::toColor(curropt->circlecolor));
            drawing::after2D();
        }

        playerent* lp = globals::MemoryManager->localPlayer;

        // TODO FIX
        // if ((lp->weaponsel->type == GUN_KNIFE) || (lp->weaponsel->type == GUN_GRENADE)) return; 

        playerent* best = nullptr;

        // call rayintersectent
        float dist;
        int bone;
        playerent* intersects = globals::MemoryManager->callRayIntersectEnt(&dist, &lp->origin, globals::MemoryManager->lookingAt, lp, &bone);

        if (curropt->targetLock) {
            if (!this->isGoodTarget(this->lockedOn) || !intersects) {
                this->lockedOn = this->findGoodTarget();
            }
            best = this->lockedOn;
        }
        else {
            best = this->findGoodTarget();
        }
        if (!best) return;

        lp->attacking = false;
        if (globals::InputManager->leftMouseDown ) {
            lp->attacking = true;
        }

        if (curropt->range && (dist > curropt->rangevalue))
            return;

        if (curropt->onkey && !globals::InputManager->getKeyState(SDLKey(curropt->hotkey)))
            return;

        vec aimat = best->origin;
        aimat.z -= best->aboveeye;
        auto fov = aimbot::viewAngleDist({ 1.f, lp->yaw, lp->pitch }, lp->origin, aimat, false);
        if ((fabs(fov.first) <= curropt->fov) && (fabs(fov.second) <= curropt->fov)) {
            if(auto angle = aimbot::getAngles(lp->origin, aimat); curropt->smoothing) {
                float newyaw = lp->yaw + (fov.first / curropt->smoothvalue);
                if (newyaw < 0.f) newyaw += 360.f;
                else if (newyaw > 359.99f) newyaw -= 360.f;
                lp->yaw = newyaw;
                lp->pitch = lp->pitch + (fov.second / curropt->smoothvalue); // pitch can't go out of bounds, range is not circular
            }
            else {
                lp->yaw = angle.y;
                lp->pitch = angle.z;
            }

            if (curropt->autoshoot) {
                lp->attacking = true;
            }
        }
    }
}

void cheats::aimbot::on() noexcept
{
    this->options()->active = true;
}

void cheats::aimbot::off() noexcept
{
    this->options()->active = false;
}

configManager::config::aimbot_conf* cheats::aimbot::options() noexcept
{
    return &(*this->profile)->aimbot;
}

playerent* cheats::aimbot::findGoodTarget() noexcept
{
    playerent* lp = globals::MemoryManager->localPlayer;
    playerent* best = nullptr;
    std::pair<float, float> bestfov = { 10e5f, 10e5f };
    float bestdist = 10e5f;
    for (size_t i = 1; i < *globals::MemoryManager->entityCount; ++i) {

        playerent* curr = (*globals::MemoryManager->entityList)[i];

        if (this->isGoodTarget(curr)) {
            vec aimat = curr->origin;
            aimat.z -= curr->aboveeye; // not mistake, want to aim a bit lower
            auto fov = aimbot::viewAngleDist({ 1.f, lp->yaw, lp->pitch }, lp->origin, aimat);
            float dist = lp->origin.dist(aimat);
            // calc coefficient
            float currcoef = dist * 0.1f + fov.first * 0.45f + fov.second * 0.45f;
            float coef = bestdist * 0.1f + bestfov.first * 0.45f + bestfov.second * 0.45f;
            if (currcoef < coef) {
                bestfov = fov;
                bestdist = dist;
                best = curr;
            }
        }
    }
    return best;
}

bool cheats::aimbot::isGoodTarget(playerent* ent) noexcept
{
    playerent* lp = globals::MemoryManager->localPlayer;
    if (!ent) return false;
    if (ent == lp) return false;
    if (ent->state != CS_ALIVE) return false;
    if (ent->health <= 0) return false;
    if ((ent->team == lp->team) && !this->options()->friendlyfire) return false;
    if (!globals::MemoryManager->callIsVisible(lp->origin, ent->origin)) return false;
    vec dummy;
    if (!drawing::worldToScreen(ent->origin, dummy)) return false;
    return true;
}

std::pair<float, float> cheats::aimbot::viewAngleDist(const vec& angles, const vec& from, const vec& to, bool abs) noexcept
{
    // yaw range: ]-90, 90[
    // pitch range: ]-90, 90[

    vec angleTo = to;
    angleTo.sub(from);
    angleTo.normalize();
    angleTo = vectorAngle(angleTo);
    
    vec diff = angleTo;
    diff.sub(angles);

    diff = aimbot::normalizeAngles(diff);
    
    if (abs)
        return { fabs(diff.y), fabs(diff.z) };
    else
        return { diff.y, diff.z };
}

vec cheats::aimbot::getAngles(const vec& from, const vec& to) noexcept
{
    vec dir(to);
    dir.sub(from);
    dir.normalize();
    return vectorAngle(dir);
}

vec cheats::aimbot::normalizeAngles(const vec& angles) noexcept
{
    vec res = angles;
    if (res.y > 180.f) res.y -= 360.f;
    else if (res.y < -180.f) res.y += 360.f;
    if (res.z > 89.0f) res.z -= 180.0f;
    else if (res.z < -89.0f) res.z += 180.0f;
    return res;
}
