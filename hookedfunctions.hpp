#ifndef ACM_HOOKEDFUNCTIONS_HPP
#define ACM_HOOKEDFUNCTIONS_HPP
#pragma once

#include <Windows.h>
#include <SDL.h>

namespace hookedfunctions {
	//bool hkwglSwapBuffers(HDC hdC);
	int hkSDL_PollEvent(SDL_Event* event);
	void hkSDL_GL_SwapBuffers();
}

#endif