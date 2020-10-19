#include "drawing.hpp"

#include <Windows.h>
#include <gl/GL.h>
#include "hookmanager.hpp"


void drawing::before2D() noexcept
{
	assert(globals::MemoryManager);

	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.f, *globals::MemoryManager->screenWidth, *globals::MemoryManager->screenHeight, 0.f, 0.f, 1.f);
	glDepthMask(GL_FALSE);
	glDisable(GL_TEXTURE_2D);
}

void drawing::after2D() noexcept
{
	glEnable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

std::optional<vec> drawing::worldToScreen(const vec& wrld)
{
	assert(globals::MemoryManager);
	// opengl is column major
	using mat = float[4][4];
	mat& vm = *(mat*)globals::MemoryManager->viewMatrix;

	float x = vm[0][0] * wrld.x + vm[1][0] * wrld.y + vm[2][0] * wrld.z + vm[3][0];
	float y = vm[0][1] * wrld.x + vm[1][1] * wrld.y + vm[2][1] * wrld.z + vm[3][1];
	float w = vm[0][3] * wrld.x + vm[1][3] * wrld.y + vm[2][3] * wrld.z + vm[3][3];
	
	if (w < 0.01f) return {};

	float sH = *globals::MemoryManager->screenHeight / 2.0f;
	float sW = *globals::MemoryManager->screenWidth / 2.0f;

	vec res;
	res.x = sW + (sW * x / w);
	res.y = sH - (sH * y / w);
	res.z = w;

	//auto r = mat * vect;
	return res;
}

void drawing::drawLine(const vec2<float> from, const vec2<float> to, float width, const color& color)
{
	glScalef(1.f, 1.f, 1.f);

	glLineWidth(width);
	glColor4f(color.r, color.g, color.b, color.a);

	glBegin(GL_LINES);

	glVertex2f(from.x, from.y);
	glVertex2f(to.x, to.y);

	glEnd();
}

void drawing::drawLineWorld(const vec& from, const vec& to, float width, const color& color)
{
	auto f = drawing::worldToScreen(from);
	auto t = drawing::worldToScreen(to);
	if (f.has_value() && t.has_value()) {
		drawing::drawLine({ f->x, f->y }, { t->x, t->y }, width, color);
	}
}

void drawing::drawBox(const vec2<float>& bottomleft, const vec2<float>& topright, float borderwidth, const color& color)
{
	glScalef(1.f, 1.f, 1.f);

	glLineWidth(borderwidth);
	glColor4f(color.r, color.g, color.b, color.a);

	glBegin(GL_LINE_LOOP);

	int h = bottomleft.y - topright.y;
	int w = topright.x - bottomleft.x;

	glVertex2f(bottomleft.x, bottomleft.y);
	glVertex2f(bottomleft.x, bottomleft.y - h);
	glVertex2f(topright.x, topright.y);
	glVertex2f(topright.x, topright.y + h);

	glEnd();
}

void drawing::drawBoxWorld(const vec& bottomleft, const vec& topright, float borderwidth, const color& color)
{
	auto bl = drawing::worldToScreen(bottomleft);
	auto tr = drawing::worldToScreen(topright);
	if (bl.has_value() && tr.has_value()) {
		drawing::drawBox({ bl->x, bl->y }, { tr->x, tr->y }, borderwidth, color);
	}
}

void drawing::drawCircle(const vec2<float> center, float radius, float linewidth, const color& color)
{
	glScalef(1.f, 1.f, 1.f);

	glLineWidth(linewidth);
	glColor4f(color.r, color.g, color.b, color.a);

	glBegin(GL_LINE_LOOP);
	for (float deg = 0.f; deg < 360.f; ++deg) {
		float x = cosf(deg * RAD);
		float y = sinf(deg * RAD);
		glVertex2f(center.x+(radius*x), center.y+(radius*y));
	}

	glEnd();
}

void drawing::drawString(const color& color, const char* text, int x, int y, float scale)
{
	glLineWidth(1.f);
	glEnable(0xBE2u);
	glEnable(0xDE1u);
	glScalef(scale, scale, 1.f);
	unsigned char c[3] = { color.r*255, color.g * 255, color.b * 255 };
	globals::MemoryManager->callHudDrawString(c, text, x / scale, y / scale);
	glScalef(1 / scale, 1 / scale, 1.f);
	glDisable(0xDE1u);
	glDisable(0xBE2u);
}

color colors::toColor(float col[3])
{
	return color{ col[0], col[1], col[2] };
}

