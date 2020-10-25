#ifndef ACM_CHEATS_BARRELESP_HPP
#define ACM_CHEATS_BARRELESP_HPP
#pragma once

#include "cheatbase.hpp"

namespace cheats {
	class barrelesp final : public cheats::cheat {
	public:
		barrelesp();
		~barrelesp();
		const char* name() const noexcept override;
		void run() noexcept override;
		void on() noexcept override;
		void off() noexcept override;
		config::barrelesp_conf& options() noexcept override;
	public:
		static constexpr float minlinelength = 1.f;
		static constexpr float maxlinelength = 20.f;
		static constexpr float minlinewidth = 1.f;
		static constexpr float maxlinewidth = 10.f;
	};
}

#endif