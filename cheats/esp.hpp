#ifndef ACM_CHEATS_ESP_HPP
#define ACM_CHEATS_ESP_HPP
#pragma once

#include "cheatbase.hpp"

namespace cheats {
	class esp final : public cheats::cheat {
	public:
		esp();
		~esp();
		const char* name() const noexcept;
		void run() noexcept;
		void on() noexcept;
		void off() noexcept;
		config::esp_conf& options() noexcept;
	};
}

#endif