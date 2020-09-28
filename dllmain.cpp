#include <Windows.h>
#include "hookmanager.hpp"

bool __stdcall DllInit(HINSTANCE hInstance, DWORD dwReasonForCall, LPVOID lpReserved)
{
	if (dwReasonForCall == DLL_PROCESS_ATTACH) {
		globals::HookManager = std::make_unique<hooks::hookManager>(hInstance);
	}
	return true;
}

bool __stdcall DllMain(HINSTANCE hInstance, DWORD dwReasonForCall, LPVOID lpReserved)
{
	bool success;
	if (dwReasonForCall == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hInstance);
	}

	success = DllInit(hInstance, dwReasonForCall, lpReserved);

	return success;
}