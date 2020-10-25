#ifndef ACM_CHEATS_TRACELINES_HPP
#define ACM_CHEATS_TRACELINES_HPP
#pragma once

#include "cheatbase.hpp"

namespace cheats {
	class snaplines final : public cheats::cheat {
	public:
		snaplines();
		~snaplines() ;
		const char* name() const noexcept override;
		void run() noexcept override;
		void on() noexcept override;
		void off() noexcept override;
		config::snaplines_conf& options() noexcept override;
	public:
		static constexpr float minlinewidth = 1.f;
		static constexpr float maxlinewidth = 10.f;
	};
}
#endif