#include <Windows.h>
#include "hookmanager.hpp"

bool __stdcall DllInit(HINSTANCE hInstance, DWORD dwReasonForCall, LPVOID lpReserved)
{
	if (dwReasonForCall == DLL_PROCESS_ATTACH) {
		globals::HookManager = std::make_unique<hooks::hook_manager>(hInstance); // rest is initialized within hooks
	}
	return true;
}

bool __stdcall DllMain(HINSTANCE hInstance, DWORD dwReasonForCall, LPVOID lpReserved)
{
	if (dwReasonForCall == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hInstance);
	}

	return DllInit(hInstance, dwReasonForCall, lpReserved);
}