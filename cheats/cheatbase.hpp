#ifndef ACM_CHEATS_CHEATBASE_HPP
#define ACM_CHEATS_CHEATBASE_HPP
#pragma once

#include "../config.hpp"
#include "../input.hpp"
#include "../memory.hpp"
#include <memory>

namespace cheats {
	class cheat {
	public:
		using config = configManager::config;
		cheat();
		virtual ~cheat();
		virtual const char * name() const noexcept = 0;
		virtual void run() noexcept = 0;
		virtual void on() noexcept = 0; 
		virtual void off() noexcept = 0;
		void toggle() noexcept;
		void operator()() noexcept;
		virtual config::base_conf* options() noexcept = 0;
	protected:
		config** profile;
	};
}

#endif