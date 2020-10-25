#ifndef ACM_CHEATS_AIMBOT_HPP
#define ACM_CHEATS_AIMBOT_HPP
#pragma once

#include "cheatbase.hpp"
#include "../sdk/entity.hpp"

#include <utility>

namespace cheats {
	class aimbot final : public cheats::cheat {
	public:
		aimbot();
		~aimbot();
		const char* name() const noexcept override;
		void run() noexcept override;
		void on() noexcept override;
		void off() noexcept override;
		config::aimbot_conf& options() noexcept override;
	private:
		playerent* findGoodTarget() noexcept;
		bool isGoodTarget(playerent* ent) noexcept;
		// @return { fovDiffYaw, fovDiffPitch }
		static std::pair<float, float> viewAngleDist(const vec& angles, const vec& from, const vec& to, bool abs = true) noexcept;
		static vec getAngles(const vec& from, const vec& to) noexcept;
		static vec normalizeAngles(const vec& angles) noexcept;
	public:
		static constexpr float minfov = 1.f;
		static constexpr float maxfov = 45.f;
		static constexpr float minsmooth = 1.f;
		static constexpr float maxsmooth = 50.f;
	private:
		playerent* lockedOn = nullptr;
	};
}

#endif