#include "cheatbase.hpp"


cheats::cheat::cheat()
{
	assert(globals::ConfigManager != nullptr);
	assert(globals::InputManager != nullptr);
	assert(globals::MemoryManager != nullptr);
}

cheats::cheat::~cheat()
{
	// TODO?
}

void cheats::cheat::toggle() noexcept
{
	if (this->options().active) {
		return this->off();
	}
	this->on();
}

void cheats::cheat::operator()() noexcept
{
	this->run();
}
