#ifndef ACM_DRAWING_HPP
#define ACM_DRAWING_HPP
#pragma once
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
	bool worldToScreen(const vec& world, vec& screen);
	void drawLine(const vec2<float> from, const vec2<float> to, float width = 1.f, const color& color = colors::white);
	void drawLineWorld(const vec& from, const vec& to, float width = 1.f, const color& color = colors::white);
	void drawRect(float x1, float y1, float x2, float y2, const color& color = colors::white, bool filled = false);

	void drawESPBox(const vec& feet, const vec& head, float width = 1.f, const color& color = colors::white);
	void drawHealthBar(const vec& feet, const vec& head, const int32_t health, float relheight = .035f);
	void drawBox(const vec2<float>& bottomleft, const vec2<float>& topright, float borderwidth = 1.f, const color& color = colors::white, bool filled = false);
	void drawCircle(const vec2<float> center, float radius, float linewidth = 1.f, const color& color = colors::white);
	void drawString(const color& color, const char* text, int x, int y, float scale);
}

#endif