#ifndef ACM_CHEATS_TRACELINES_HPP
#define ACM_CHEATS_TRACELINES_HPP
#pragma once

#include "cheatbase.hpp"

namespace cheats {
	class tracelines final : public cheats::cheat {
	public:
		tracelines();
		~tracelines() ;
		const char* name() const noexcept override;
		void run() noexcept override;
		void on() noexcept override;
		void off() noexcept override;
		config::tracelines_conf* options() noexcept override;
	public:
		static constexpr float minlinewidth = 1.f;
		static constexpr float maxlinewidth = 10.f;
	};
}
#endif