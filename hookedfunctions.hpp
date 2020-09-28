#ifndef ACM_HOOKEDFUNCTIONS_HPP
#define ACM_HOOKEDFUNCTIONS_HPP
#pragma once

#include <Windows.h>
#include "hookmanager.hpp"

namespace hookedfunctions {
	bool hkwglSwapBuffers(HDC hdC);
}

#endif