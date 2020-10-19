#include "bytepatch.hpp"
#include "misc.hpp"
#include "../helper.hpp"
#include <cassert>
#include <Windows.h>

hooks::trampoline::trampoline(const char* moduleName, const char* functionName, void* dst, const size_t bytes)
	: dst(uintptr_t(dst)), length(bytes), active(false), gateway(0)
{
	assert(bytes > 4);
	this->src = (uintptr_t)helper::getExportedProcAddress(moduleName, functionName);
}

hooks::trampoline::trampoline(void* src, void* dst, const size_t bytes)
	: src(uintptr_t(src)), dst(uintptr_t(dst)), length(bytes), active(false), gateway(0)
{
	assert(bytes > 4);
}

hooks::trampoline::~trampoline()
{
	if (this->active) {
		this->restore();
	}
}

void hooks::trampoline::hook() noexcept
{
	if (!this->active) {
		this->gateway = VirtualAlloc(0, this->length, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		memcpy_s(this->gateway, this->length, (void*)this->src, this->length);
		hooks::makeRelativeJump((unsigned char*)this->gateway + this->length, (unsigned char*)this->src + 0x5);
		DWORD dwOldProtec;
		VirtualProtect((void*)this->src, this->length, PAGE_EXECUTE_READWRITE, &dwOldProtec);
		hooks::makeRelativeJump((void*)this->src, (void*)this->dst);
		for (size_t i = 0x5; i < this->length; ++i) {
			*((unsigned char*)this->src + i) = 0x90;
		}
		VirtualProtect((void*)this->src, this->length, dwOldProtec, &dwOldProtec);
		this->active = true;
	}
}

void hooks::trampoline::restore() noexcept
{
	if (this->active) {
		DWORD dwOldProtec;
		VirtualProtect((void*)this->src, this->length, PAGE_EXECUTE_READWRITE, &dwOldProtec);
		memcpy_s((void*)this->src, this->length, this->gateway, this->length);
		VirtualProtect((void*)this->src, this->length, dwOldProtec, &dwOldProtec);
		VirtualFree(this->gateway, 0, MEM_RELEASE);
		this->active = false;
	}
}

void* hooks::trampoline::getGateway() const noexcept
{
	return this->gateway;
}