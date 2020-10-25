#ifndef ACM_CHEATS_GUNS_HPP
#define ACM_CHEATS_GUNS_HPP
#pragma once

#include "cheatbase.hpp"
#include "../sdk/weapon.hpp"

namespace cheats {
	class guns final : public cheats::cheat {
	public:
		guns();
		~guns();
		const char* name() const noexcept;
		void run() noexcept;
		void on() noexcept;
		void off() noexcept;
		config::guns_conf& options() noexcept;
	private:
		guninfo defaults[NUMGUNS];
	};
}

#endif 
