#ifndef ACM_HOOKMANAGER_HPP
#define ACM_HOOKMANAGER_HPP
#pragma once

#include <Windows.h>
#include <memory>

#include "memory.hpp"
#include "config.hpp"

#include "hooks/vmt.hpp"
#include "hooks/bytepatch.hpp"

namespace hooks
{
	class hookManager
	{
	public:
		hookManager(HMODULE hModule);
		void install();
		void uninstall();
		// original function ptrs
		bool(__stdcall* wglSwapBuffersGateway)(HDC hdC);
		WNDPROC originalWndProc;

		// other attributes
		HWND hWnd;
		HMODULE hModule;
		//hooks
		hooks::trampoline swapbuffers;
	};
}
// global
namespace globals {
	inline std::unique_ptr<hooks::hookManager> HookManager;
}
#endif