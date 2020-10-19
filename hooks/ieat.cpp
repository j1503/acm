#include "ieat.hpp"

#pragma comment(lib,"dbghelp.lib")
#include <stdexcept>

// src = from, dst = to
// => *dst++ = *src++ for _ in range(len) 
// returns false on failure
inline static bool protec(void* dst, void* src, size_t len, uint32_t protec) {
	DWORD dwOldProtec;
	if (!VirtualProtect(dst, len, protec, &dwOldProtec)) return false;
	if (memcpy_s(dst, len, src, len)) return false;
	if (!VirtualProtect(dst, len, dwOldProtec, &dwOldProtec)) return false;
	return true;
}


hooks::iat::iat(void* imageBase)
	: imageBase{ uintptr_t(imageBase) }
{
	PIMAGE_NT_HEADERS headers = ImageNtHeader(imageBase);
	this->iatbase = (uintptr_t**)(this->imageBase + headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress);
	size_t iatsize = headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size / sizeof(void*);
	this->original = std::vector<uintptr_t*>(this->iatbase, this->iatbase+iatsize);
	this->active = std::vector<bool>(iatsize, false);

	this->importbase = PIMAGE_IMPORT_DESCRIPTOR(this->imageBase + headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
}

hooks::iat::iat(const char* imageName)
	: hooks::iat::iat(GetModuleHandle(imageName)) {}

hooks::iat::~iat()
{
	for (size_t i = 0; i < this->active.size(); ++i) {
		if (this->active[i]) {
			this->restore(i);
		}
	}
}

int32_t hooks::iat::findIndex(const char* moduleName, const char* functionName) const
{
	if (!moduleName || !functionName) throw std::invalid_argument("nullptr was passed as parameter"); // weird arguments
	
	PIMAGE_IMPORT_DESCRIPTOR piid = this->importbase;
	while (piid->Name) {
		if (!lstrcmpiA(moduleName, (char*)(this->imageBase + piid->Name))) {
			break;
		}
		++piid;
	}
	
	PIMAGE_THUNK_DATA pitd;
	if (piid->OriginalFirstThunk) {
		// find by name
		pitd = PIMAGE_THUNK_DATA(this->imageBase + piid->OriginalFirstThunk);
		PIMAGE_THUNK_DATA pfirst = PIMAGE_THUNK_DATA(this->imageBase + piid->FirstThunk);
		while (!(pitd->u1.Ordinal & 0x80000000) && pitd->u1.AddressOfData) {
			PIMAGE_IMPORT_BY_NAME pibn = PIMAGE_IMPORT_BY_NAME(this->imageBase + pitd->u1.AddressOfData);
			if (!lstrcmpiA(functionName, (char*)pibn->Name)) {
				return ((uintptr_t)pfirst - (uintptr_t)this->iatbase) / sizeof(void*);
			}
			++pfirst;
			++pitd;
		}
	}
	else if (piid->FirstThunk) {
		// find by ordinal
		HMODULE h = GetModuleHandle(moduleName);
		if (!h) return false;
		uintptr_t lookingFor = uintptr_t(GetProcAddress(h, functionName));
		if (!lookingFor) return false;
		pitd = PIMAGE_THUNK_DATA(this->imageBase + piid->FirstThunk);
		while (pitd->u1.AddressOfData) {
			auto X = (uintptr_t)pitd->u1.Function; // ?? dont know why it has to be like that 
			if (X == lookingFor) {
				return ((uintptr_t)pitd - (uintptr_t)this->iatbase) / sizeof(void*);
			}
			++pitd;
		}
	}
	return -1;

}


int32_t hooks::iat::hook(const char* moduleName, const char* functionName, void* hk)
{
	if (auto i = findIndex(moduleName, functionName); i != -1) {
		if (!protec(&this->iatbase[i], &hk, sizeof(void*), PAGE_READWRITE)) {
			return -1;
		}
		this->active[i] = true;
		return i;
	}
	return -1;
}

bool hooks::iat::hook(size_t index, void* hk)
{
	if (!protec(&this->iatbase[index], &hk, sizeof(void*), PAGE_READWRITE)) {
		return false;
	}
	this->active[index] = true;
	return false;
}

bool hooks::iat::restore(const char* moduleName, const char* functionName)
{
	if (auto i = findIndex(moduleName, functionName); i != -1) {
		if (!protec(&this->iatbase[i], &this->original[i], sizeof(void*), PAGE_READWRITE)) {
			return false;
		}
		this->active[i] = false;
		return true;
	}
	return false;
}

bool hooks::iat::restore(size_t index)
{
	if (!protec(&this->iatbase[index], &this->original[index], sizeof(void*), PAGE_READWRITE)) {
		return false;
	}
	this->active[index] = false;
	return true;
}

