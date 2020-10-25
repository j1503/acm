#ifndef ACM_CHEATS_CHEATMANAGER_HPP
#define ACM_CHEATS_CHEATMANAGER_HPP
#pragma once

#include <memory>
#include "cheatbase.hpp"
#include "triggerbot.hpp"
#include "snaplines.hpp"
#include "aimbot.hpp"
#include "barrelesp.hpp"
#include "esp.hpp"
#include "guns.hpp"

namespace cheats {
	class cheat_manager {
	public:
		cheat_manager();
		~cheat_manager() = default;
	public:
		cheats::triggerbot triggerbot;
		cheats::snaplines snaplines;
		cheats::barrelesp barrelesp;
		cheats::aimbot aimbot;
		cheats::esp esp;
		cheats::guns guns;
	};
}

namespace globals {
	inline std::unique_ptr<cheats::cheat_manager> CheatManager;
}

#endif