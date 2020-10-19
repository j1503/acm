#ifndef ACM_CHEATS_CHEATMANAGER_HPP
#define ACM_CHEATS_CHEATMANAGER_HPP
#pragma once

#include <memory>
#include "cheatbase.hpp"
#include "triggerbot.hpp"
#include "tracelines.hpp"
#include "aimbot.hpp"
#include "barrelesp.hpp"

namespace cheats {
	class cheatManager {
	public:
		cheatManager();
		~cheatManager() = default;
	public:
		cheats::triggerbot triggerbot;
		cheats::tracelines tracelines;
		cheats::barrelesp barrelesp;
		cheats::aimbot aimbot;
	};
}

namespace globals {
	inline std::unique_ptr<cheats::cheatManager> CheatManager;
}

#endif