#ifndef ACM_CHEATS_TRIGGERBOT_HPP
#define ACM_CHEATS_TRIGGERBOT_HPP
#pragma once

#include "cheatbase.hpp"

namespace cheats {
	class triggerbot final : public cheats::cheat {
	public:
		triggerbot();
		~triggerbot();
		const char * name() const noexcept override;
		void run() noexcept override;
		void on() noexcept override;
		void off() noexcept override;
		config::triggerbot_conf* options() noexcept override;
	};
}

#endif