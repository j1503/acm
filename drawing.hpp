#ifndef ACM_DRAWING_HPP
#define ACM_DRAWING_HPP
#pragma once
#include <optional>
#include <cassert>
#include "sdk/geom.hpp"
#include "memory.hpp"

struct color {
	float r, g, b, a;
};

// constants & helpers
inline namespace colors {
	constexpr color black = { 0.0,0.0,0.0,1.0 };
	constexpr color white = { 1.0,1.0,1.0,1.0 };
	constexpr color red = { 1.0,0.0,0.0,1.0 };
	constexpr color green = { 0.0,1.0,0.0,1.0 };
	constexpr color blue = { 0.0,0.0,1.0,1.0 };
	
	color toColor(float col[3]);
}

namespace drawing {
	// call before and after rendering anything in 2d (or any block)!
	void before2D() noexcept;
	void after2D() noexcept;
	template <class T>
	inline auto makeNDC(const vec2<T>& point) {
		assert(globals::MemoryManager);
		
		vec2<float> r = { point.x, point.y };
		
		size_t w, h;
		w = *globals::MemoryManager->screenWidth;
		h = *globals::MemoryManager->screenHeight;

		r.x = 2.f * r.x / (float)w - 1.f;
		r.y = 2.f * r.y / (float)h - 1.f;

		return r;
	}
	std::optional<vec> worldToScreen(const vec& world);
	void drawLine(const vec2<float> from, const vec2<float> to, float width = 1.f, const color& color = colors::white);
	void drawLineWorld(const vec& from, const vec& to, float width = 1.f, const color& color = colors::white);
	void drawBox(const vec2<float>& bottomleft, const vec2<float>& topright, float borderwidth = 1.f, const color& color = colors::white);
	void drawBoxWorld(const vec& bottomleft, const vec& topright, float borderwidth = 1.f, const color& color = colors::white);
	void drawCircle(const vec2<float> center, float radius, float linewidth = 1.f, const color& color = colors::white);
	void drawString(const color& color, const char* text, int x, int y, float scale);
}

#endif