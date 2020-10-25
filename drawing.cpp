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

bool drawing::worldToScreen(const vec& wrld, vec& screen)
{
	assert(globals::MemoryManager);
	// opengl is column major
	using mat = float[4][4];
	mat& vm = *(mat*)globals::MemoryManager->viewMatrix;
	float width = (float)*globals::MemoryManager->screenWidth;
	float height = (float)*globals::MemoryManager->screenHeight;

	screen.x = vm[0][0] * wrld.x + vm[1][0] * wrld.y + vm[2][0] * wrld.z + vm[3][0];
	screen.y = vm[0][1] * wrld.x + vm[1][1] * wrld.y + vm[2][1] * wrld.z + vm[3][1];
	screen.z = vm[0][2] * wrld.x + vm[1][2] * wrld.y + vm[2][2] * wrld.z + vm[3][2];
	float w = vm[0][3] * wrld.x + vm[1][3] * wrld.y + vm[2][3] * wrld.z + vm[3][3];
	
	if (w < 0.1f) return false;

	vec ndc;
	ndc.x = screen.x / w;
	ndc.y = screen.y / w;
	ndc.z = screen.z / w;

	screen.x = (width / 2 * ndc.x) + (ndc.x + width / 2);
	screen.y = -(height / 2 * ndc.y) + (ndc.y + height / 2);

	return true;
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
	vec f, t;
	if (drawing::worldToScreen(from, f) && drawing::worldToScreen(to, t)) {
		drawing::drawLine({ f.x, f.y }, { t.x, t.y }, width, color);

	}
}

void drawing::drawRect(float x1, float y1, float x2, float y2, const color& color, bool filled)
{
	glColor4f(color.r, color.g, color.b, color.a);

	glBegin(filled ? GL_QUADS : GL_LINE_LOOP);

	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);

	glEnd();
}

void drawing::drawESPBox(const vec& feet, const vec& head, float width, const color& color)
{
	vec sf, sh;//screen feet , screen head
	if (drawing::worldToScreen(feet, sf) && drawing::worldToScreen(head, sh)) {
		const float h = sf.y - sh.y;
		const float w = h / 1.9f;
		drawing::drawRect(sh.x - w / 2 - 1, sf.y - 1, sh.x + w / 2 + 1, sh.y + 1, colors::black);
		drawing::drawRect(sh.x - w / 2, sf.y, sh.x + w / 2, sh.y, color);
	}
}

void drawing::drawHealthBar(const vec& feet, const vec& head, const int32_t health, float relheight)
{
	vec sf, sh;//screen feet , screen head
	if (drawing::worldToScreen(feet, sf) && drawing::worldToScreen(head, sh)) {
		const float h = sf.y - sh.y;
		const float w = h / 1.9f;

		drawing::drawRect(sh.x - w / 2 - 1, sh.y - h * relheight - h * (relheight * 2.5f) - 1, sh.x + w / 2 + 1,
			sh.y - h * (relheight * 2.5f) + 1, colors::black, false); //outline
		drawing::drawRect(sh.x - w / 2, sh.y - h * relheight - h * (relheight * 2.5f), sh.x + w / 2,
			sh.y - h * (relheight * 2.5f), colors::red, true); //red
		drawing::drawRect(sh.x - w / 2, sh.y - h * relheight - h * (relheight * 2.5f), (sh.x - w / 2) + (w * (float)health / 100.f),
			sh.y - h * (relheight * 2.5f), colors::green, true); //green 
	}
}

void drawing::drawBox(const vec2<float>& bottomleft, const vec2<float>& topright, float borderwidth, const color& color, bool filled)
{
	glScalef(1.f, 1.f, 1.f);

	glLineWidth(borderwidth);
	glColor4f(color.r, color.g, color.b, color.a);

	glBegin(filled ? GL_QUADS : GL_LINE_LOOP);

	// fabs prob not needed - just to be sure
	float h = fabs(bottomleft.y - topright.y);
	float w = fabs(topright.x - bottomleft.x);

	glVertex2f(bottomleft.x, bottomleft.y);
	glVertex2f(bottomleft.x, bottomleft.y - h);
	glVertex2f(topright.x, topright.y);
	glVertex2f(topright.x, topright.y + h);

	glEnd();
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
	unsigned char c[3] = { char(color.r*255.f), char(color.g * 255.f), char(color.b * 255.f) };
	globals::MemoryManager->callHudDrawString(c, text, int(x / scale), int(y / scale));
	glScalef(1 / scale, 1 / scale, 1.f);
	glDisable(0xDE1u);
	glDisable(0xBE2u);
}

color colors::toColor(float col[3])
{
	return { col[0], col[1], col[2] };
}

