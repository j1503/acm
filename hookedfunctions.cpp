#include "hookedfunctions.hpp"

#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib, "glu32.lib")

void drawBox(float x, float y, float width, float height, int* RGB)
{
	glLineWidth(1);
	glBegin(GL_LINE_LOOP);
	glColor4f(RGB[0], RGB[1], RGB[2], 1);
	glVertex2f(x - (width / 2), y);
	glVertex2f(x - (width / 2), y - height);
	glVertex2f(x + (width / 2), y - height);
	glVertex2f(x + (width / 2), y);
	glEnd();
}

bool hookedfunctions::hkwglSwapBuffers(HDC hdC)
{
	glBegin(GL_LINES);
	glVertex2f(10, 10);
	glVertex2f(20, 20);
	glEnd();

	int red[3] = { 255,0,0 };
	drawBox(100, 100, 100, 20, red);

	// screen coordinate = projectionMatrix * modelViewMatrix * worldCoordinates
	globals::MemoryManager->viewMatrix;

	if (globals::MemoryManager->entityList) {
		for (size_t i = 1; i < *globals::MemoryManager->entityCount; ++i) {
			auto& o = globals::MemoryManager->entityList[i]->origin;
		}
	}
	if (GetAsyncKeyState(globals::Config->general.ejectKey) & 1) {
		globals::HookManager->uninstall();
		return true;
	}
	return globals::HookManager->wglSwapBuffersGateway(hdC);
}