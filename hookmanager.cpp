#include "hookmanager.hpp"
#include "hookedfunctions.hpp"

#include "gui/imgui/imgui.h"
#include "helper.hpp"
#include "debugconsole.hpp"
#include "input.hpp"
#include "memory.hpp"
#include "config.hpp"
#include "cheats/cheatmanager.hpp"
#include "gui/gui.hpp"

static LRESULT __stdcall wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static bool __stdcall unload(HMODULE hModule);
extern IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT __stdcall wndProcThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)wndProc);
#ifdef _DEBUG
	globals::DebugConsole = std::make_unique<debug::console>();
#endif
	globals::ConfigManager = std::make_unique<configManager>(hWnd);
	globals::MemoryManager = std::make_unique<memory::memoryManager>();
	globals::InputManager = std::make_unique<input>();
	globals::CheatManager = std::make_unique<cheats::cheatManager>();
	globals::GUIManager = std::make_unique<drawing::gui>(hWnd);

	globals::HookManager->install();

	return wndProc(hWnd, msg, wParam, lParam);
}

static LRESULT __stdcall wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// imgui
	if (globals::ActiveProfile->general.active && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
		return 1l;
	}
	switch (msg) {
	case WM_QUIT:
		globals::HookManager->uninstall(); // cleanup, even on program termination
		break;
	}

	return CallWindowProc(globals::HookManager->originalWndProc, hWnd, msg, wParam, lParam);
}

hooks::hookManager::hookManager(HMODULE hModule)
	: hModule(hModule), sdlswapbuffers(), pollevents()
{
	this->hWnd = FindWindow(nullptr, "AssaultCube");

	this->originalWndProc = WNDPROC(SetWindowLongPtr(this->hWnd, GWLP_WNDPROC, (LONG_PTR)wndProcThunk));
}

void hooks::hookManager::install()
{
	// create hooks
	//this->swapbuffers = hooks::trampoline("opengl32.dll", "wglSwapBuffers", hookedfunctions::hkwglSwapBuffers, 0x5);
	this->pollevents = hooks::trampoline("SDL.dll", "SDL_PollEvent", hookedfunctions::hkSDL_PollEvent, 0x6);
	this->sdlswapbuffers = hooks::iat("ac_client.exe");
	
	// hook
	this->sdlswapbuffers.hook("SDL.dll", "SDL_GL_SwapBuffers", hookedfunctions::hkSDL_GL_SwapBuffers);
	//this->swapbuffers.hook();
	this->pollevents.hook();

	this->installed = true;
}

void hooks::hookManager::uninstall()
{
	// restore hooks
	// prevent errors if hooks haven't been initalized yet
	// e.g. if an error occurs during global object initialization and uninstall() is called. 
	// might still crash if some global objects have no been initialized properly.
	if(this->installed){
		this->pollevents.restore();
		this->sdlswapbuffers.restore("SDL.dll", "SDL_GL_SwapBuffers");

		this->installed = false; // not neccessary but w/e
	}

	// restore wndproc & unload
	SetWindowLongPtr(this->hWnd, GWLP_WNDPROC, (LONG_PTR)this->originalWndProc);
	HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)unload, this->hModule, 0, nullptr);
	if (hThread) CloseHandle(hThread);
}

static bool __stdcall unload(HMODULE hModule) {
	Sleep(150); // safety
	FreeLibraryAndExitThread(hModule, 0);
}