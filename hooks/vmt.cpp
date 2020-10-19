#include "vmt.hpp"

#include <Windows.h>
#include <algorithm>
#include <cassert>

#define VALID_INDEX(idx, len) (idx >= 0) && (idx < len)

hooks::vmt::vmt(void* vtableBase, bool virtualprotect)
	: vtable(vtableBase), virtualprotect(virtualprotect)
{
	assert(vtableBase);
	this->length = hooks::vmt::getVmtEntryCount(vtable);
	this->originalvTable = std::vector<uintptr_t>(this->length);
	this->active = std::vector<bool>(this->length);
	for (size_t i = 0; i < this->length; ++i) {
		this->originalvTable[i] = ((uintptr_t*)vtable)[i];
	}
}

hooks::vmt::~vmt()
{
	for (size_t i = 0; i < this->active.size(); ++i) {
		if (this->active[i]) {
			this->restore(i);
		}
	}
}

void hooks::vmt::hook(const size_t index, void* function) noexcept
{
	assert(VALID_INDEX(index, this->length));
	if (!this->active[index]) {
		void* vtablefuncaddr = (unsigned char*)this->vtable + index * sizeof(void*);
		if (this->virtualprotect) {
			DWORD dwOldProtec;
			VirtualProtect(vtablefuncaddr, sizeof(void*), PAGE_EXECUTE_READWRITE, &dwOldProtec);
			*(uintptr_t*)vtablefuncaddr = (uintptr_t)function;
			VirtualProtect(vtablefuncaddr, sizeof(void*), dwOldProtec, &dwOldProtec);
		}
		this->active[index] = true;
	}
}

void hooks::vmt::restore(const size_t index) noexcept
{
	assert(VALID_INDEX(index, this->length));
	if (this->active[index]) {
		void* vtablefuncaddr = (unsigned char*)this->vtable + index * sizeof(void*);
		if (this->virtualprotect) {
			DWORD dwOldProtec;
			VirtualProtect(vtablefuncaddr, sizeof(void*), PAGE_EXECUTE_READWRITE, &dwOldProtec);
			*(uintptr_t*)vtablefuncaddr = this->originalvTable[index];
			VirtualProtect(vtablefuncaddr, sizeof(void*), dwOldProtec, &dwOldProtec);
		}
		this->active[index] = false;
	}
}

size_t hooks::vmt::getVmtEntryCount(void* vtable)
{
	assert(vtable);
	size_t length = 0;
	uintptr_t* vtable2 = (uintptr_t*)vtable;
	MEMORY_BASIC_INFORMATION memoryInfo;
	while (VirtualQuery(LPCVOID(vtable2[length]), &memoryInfo, sizeof(memoryInfo)) && (memoryInfo.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))) {
		++length;
	}
	return length;
}

hooks::vmtswap::vmtswap(void* instance)
	: classinstance(instance)
{
	this->originalvTableBase = *(uintptr_t**)instance;
	this->originalvTableAddress = *(uintptr_t*)instance;
	this->length = hooks::vmtswap::getVmtEntryCount(this->originalvTableBase);
	this->active = std::vector<bool>(this->length);
	this->vtableswapped = std::vector<uintptr_t>(this->length);
	for (size_t i = 0; i < this->length; ++i) {
		this->vtableswapped[i] = this->originalvTableBase[i];
	}
	*(uintptr_t*)this->classinstance = (uintptr_t)this->vtableswapped.data();
}

hooks::vmtswap::~vmtswap()
{
	for (size_t i = 0; i < this->active.size(); ++i) {
		if (this->active[i]) {
			this->restore(i);
		}
	}
	*(uintptr_t*)this->classinstance = this->originalvTableAddress;
}

void hooks::vmtswap::hook(const size_t index, void* function)
{
	assert(VALID_INDEX(index, this->length));
	if (!this->active[index]) {
		this->vtableswapped[index] = (uintptr_t)function;
		this->active[index] = true;
	}
}

void hooks::vmtswap::restore(const size_t index)
{
	assert(VALID_INDEX(index, this->length));
	if (this->active[index]) {
		this->vtableswapped[index] = this->originalvTableBase[index];
		this->active[index] = false;
	}
}

//vmtswap
size_t hooks::vmtswap::getVmtEntryCount(void* vtableBase)
{
	return hooks::vmt::getVmtEntryCount(vtableBase);
}