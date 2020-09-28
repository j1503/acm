#include "hookmanager.hpp"
#include "hookedfunctions.hpp"
#include "helper.hpp"

static LRESULT __stdcall wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static bool __stdcall unload(HMODULE hModule);

static LRESULT __stdcall wndProcThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)wndProc);

	// init stuff here
	globals::MemoryManager = std::make_unique<memory::memoryManager>();
	globals::Config = std::make_unique<configManager>();

	globals::HookManager->install();

	return wndProc(hWnd, msg, wParam, lParam);
}

static LRESULT __stdcall wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProc(globals::HookManager->originalWndProc, hWnd, msg, wParam, lParam);
}

hooks::hookManager::hookManager(HMODULE hModule)
	: hModule(hModule)
{
	this->hWnd = FindWindow(0, "AssaultCube");
	this->originalWndProc = WNDPROC(SetWindowLongPtr(this->hWnd, GWLP_WNDPROC, (LONG_PTR)wndProcThunk));
}

void hooks::hookManager::install()
{
	// create hooks
	this->swapbuffers = hooks::trampoline("opengl32.dll", "wglSwapBuffers", hookedfunctions::hkwglSwapBuffers, 0x5);

	// hook
	this->swapbuffers.hook();

	// set pointers
	this->wglSwapBuffersGateway = reinterpret_cast<bool(__stdcall*)(HDC)>(this->swapbuffers.getGateway());
}

void hooks::hookManager::uninstall()
{
	this->swapbuffers.restore();

	SetWindowLongPtr(this->hWnd, GWLP_WNDPROC, (LONG_PTR)this->originalWndProc);
	HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)unload, this->hModule, 0, nullptr);
	if (hThread) CloseHandle(hThread);
}

static bool __stdcall unload(HMODULE hModule) {
	Sleep(150);
	FreeLibraryAndExitThread(hModule, 0);
}