#include "helper.hpp"
#include <cassert>
#include "hookmanager.hpp"

void* helper::getExportedProcAddress(const char* moduleName, const char* functionName)
{
	HMODULE hModule = GetModuleHandleA(moduleName);
	if (!hModule) return 0;
	return GetProcAddress(hModule, functionName);
}
