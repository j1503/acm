#ifndef ACM_HOOKMANAGER_HPP
#define ACM_HOOKMANAGER_HPP
#pragma once

#include <Windows.h>
#include <memory>
#include <SDL.h>

#include "hooks/vmt.hpp"
#include "hooks/bytepatch.hpp"
#include "hooks/ieat.hpp"

namespace hooks
{
	class hookManager
	{
	public:
		hookManager() = default;
		hookManager(HMODULE hModule);
		void install();
		void uninstall();
		// original function ptrs
		WNDPROC originalWndProc;

		// other attributes
		HWND hWnd;
		HMODULE hModule;
		//hooks
		//hooks::trampoline swapbuffers;
		hooks::trampoline pollevents;
		hooks::iat sdlswapbuffers;
	};
}
// global
namespace globals {
	inline std::unique_ptr<hooks::hookManager> HookManager;
}
#endif