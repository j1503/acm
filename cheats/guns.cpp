#include "guns.hpp"

cheats::guns::guns()
{
	assert(globals::MemoryManager);
	auto lp = globals::MemoryManager->localPlayer;
	for (size_t i = 0; i < NUMGUNS; ++i) {
		this->defaults[i] = lp->weapons[i]->info;
	}
}

cheats::guns::~guns()
{
	this->off();
}

const char* cheats::guns::name() const noexcept
{
	return "Guns";
}

void cheats::guns::run() noexcept
{
	auto& curropt = this->options();
	auto lp = globals::MemoryManager->localPlayer;

	for (size_t i = 0; i < NUMGUNS; ++i) {
		// reloadtime 
		if (curropt.reload)
			lp->weapons[i]->info.reloadtime = 0;
		else
			lp->weapons[i]->info.reloadtime = this->defaults[i].reloadtime;
		// attackdelay/rapidfire 
		if (curropt.rapidfire)
			lp->weapons[i]->info.attackdelay = 0;
		else
			lp->weapons[i]->info.attackdelay = this->defaults[i].attackdelay;
		// spread
		if (curropt.nospread)
			lp->weapons[i]->info.spread = 0;
		else
			lp->weapons[i]->info.spread = this->defaults[i].spread;
		// ammo (magsize)
		if (curropt.infammo)
			lp->weapons[i]->mag = this->defaults[i].magsize;
		// recoil
		if(curropt.recoil)
			lp->weapons[i]->info.maxrecoil = 0;
		else 
			lp->weapons[i]->info.maxrecoil = this->defaults[i].maxrecoil;
		// kickback 
		if (curropt.kickback)
			lp->weapons[i]->info.recoil = 0;
		else
			lp->weapons[i]->info.recoil = this->defaults[i].recoil;
		// auto
		if (curropt.fullauto)
			lp->weapons[i]->info.isauto = true;
		else
			lp->weapons[i]->info.isauto = this->defaults[i].isauto;
	}
}

void cheats::guns::on() noexcept
{
	// dont need to do anything, but needed for polymorphism to work correctly (maybe change?)
}

void cheats::guns::off() noexcept
{
	auto lp = globals::MemoryManager->localPlayer;
	for (size_t i = 0; i < NUMGUNS; ++i) {
		lp->weapons[i]->info = this->defaults[i];
	}
}

config_manager::config::guns_conf& cheats::guns::options() noexcept
{
	return globals::ConfigManager->active().guns;
}