#include "entity.hpp"
#include "geom.hpp"
#include "../memory.hpp"

playerent* rayIntersectEnt(int* bone, float* dist, float raylength)
{
	playerent* lp = globals::MemoryManager->localPlayer;
	vec rayend = lp->origin;
	vec dirvec(1.f, lp->yaw, lp->pitch);
	dirvec = angleVector(dirvec);
	dirvec.mul(raylength);
	rayend.add(dirvec);

	return globals::MemoryManager->callRayIntersectEnt(dist, &lp->origin, &rayend, lp, bone);
}