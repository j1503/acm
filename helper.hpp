#ifndef ACM_HOOKS_HELPER_HPP
#define ACM_HOOKS_HELPER_HPP
#pragma once

#include <Windows.h>

namespace helper
{
	[[nodiscard]] void* getExportedProcAddress(const char* moduleName, const char* functionName);
	template<class RET, class ...ARGS>
	[[nodiscard]] constexpr inline auto getExportedCdeclFunction(const char* moduleName, const char* functionName) {
		HMODULE hModule = GetModuleHandleA(moduleName);
		auto addr = GetProcAddress(hModule, functionName);
		return reinterpret_cast<RET(__cdecl*)(ARGS...)>(addr);
	}
	template<class RET, class ...ARGS>
	[[nodiscard]] constexpr inline auto getExportedStdcallFunction(const char* moduleName, const char* functionName) {
		HMODULE hModule = GetModuleHandleA(moduleName);
		auto addr = GetProcAddress(hModule, functionName);
		return reinterpret_cast<RET(__stdcall*)(ARGS...)>(addr);
	}
}

#endif