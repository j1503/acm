#include "memory.hpp"

#include <gl/GL.h>
#include <type_traits>
#pragma comment(lib,"opengl32.lib")
#include "helper.hpp"

inline static bool patternSearchMemcmp(const unsigned char* word, const unsigned char* text, size_t size) {
	while (size) {
		if (*word != '?') {
			if (*word != *text) return false;
		}
		--size;
		++word;
		++text;
	}
	return true;
};

template <class T, size_t OFFSET = 0u>
static T findPatternNaiive(const char* moduleName, const char* pattern) noexcept
{
	HMODULE hModule = GetModuleHandle(moduleName);
	if (hModule) {
		MODULEINFO moduleInfo;
		if (GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo))) {
			char* start = static_cast<char*>(moduleInfo.lpBaseOfDll);
			const char* end = start + moduleInfo.SizeOfImage;

			char* first = start;
			char* second = const_cast<char*>(pattern);

			while (first < end && *second) {
				if (*first == *second || *second == '?') {
					++first;
					++second;
				}
				else {
					first = ++start;
					second = const_cast<char*>(pattern);
				}
			}

			if (!*second)
				return reinterpret_cast<T>(start + OFFSET);
		}
	}
	return 0;
}

// ~7.5x faster than findPatternNaiive
template <class T, size_t OFFSET = 0u>
static T findPatternBMH(const char* moduleName, const char* pattern) noexcept {
	HMODULE hModule = GetModuleHandle(moduleName);
	if (hModule) {
		MODULEINFO moduleInfo;
		if (GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo))) {
			const unsigned char* text = (const unsigned char*)moduleInfo.lpBaseOfDll;
			const unsigned char* word = (const unsigned char*)pattern;

			const size_t textLength = moduleInfo.SizeOfImage;
			const size_t wordLength = strlen((char*)word);
			const size_t maxWordIdx = wordLength - 1;

			size_t table[256];
			std::fill_n(table, 256, wordLength);
			for (size_t i = 0; i < maxWordIdx; ++i) {
				table[word[i]] = maxWordIdx - i;
			}

			const unsigned char lastWordChar = word[maxWordIdx];
			size_t textPos = 0;

			while (textPos <= (textLength - wordLength)) {
				const unsigned char curr = text[textPos + maxWordIdx];
				if (((curr == lastWordChar) || lastWordChar == '?') && patternSearchMemcmp(word, text + textPos, maxWordIdx)) {
					return (T)(text + textPos + OFFSET);
				}
				textPos += table[curr];
			}
		}
	}
	return (T)0;
}

static bool isPatternUnique(const char* moduleName, const char* pattern) noexcept
{
	size_t occ = 0;
	HMODULE hModule = GetModuleHandle(moduleName);
	if (hModule) {
		MODULEINFO moduleInfo;
		if (GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo))) {
			const unsigned char* text = (const unsigned char*)moduleInfo.lpBaseOfDll;
			const unsigned char* word = (const unsigned char*)pattern;

			const size_t textLength = moduleInfo.SizeOfImage;
			const size_t wordLength = strlen((char*)word);
			const size_t maxWordIdx = wordLength - 1;

			size_t table[256];
			std::fill_n(table, 256, wordLength);
			for (size_t i = 0; i < maxWordIdx; ++i) {
				table[word[i]] = maxWordIdx - i;
			}

			const unsigned char lastWordChar = word[maxWordIdx];
			size_t textPos = 0;

			while (textPos <= (textLength - wordLength)) {
				const unsigned char curr = text[textPos + maxWordIdx];
				if (((curr == lastWordChar) || lastWordChar == '?') && patternSearchMemcmp(word, text + textPos, maxWordIdx)) {
					++occ;
				}
				textPos += table[curr];
			}
		}
	}
	return occ == 1;
}

template <class FunctionPointerType, size_t OPCODE_SIZE = 1u> // opcode size in bytes (e.g. 1u for "call") 
// requires Pointer<FunctionPointerType>
inline static FunctionPointerType getAbsoluteAddress(void* EIP)
{
	static_assert(std::is_pointer<FunctionPointerType>::value, "violation of requirement Pointer<FunctionPointerType>");
	constexpr size_t JMP_LEN = 5u;
	return (FunctionPointerType)(*(uintptr_t*)(uintptr_t(EIP) + OPCODE_SIZE) + JMP_LEN + uintptr_t(EIP));
}

// FunctionPointerType = function pointer type ( e.g. int(__cdecl*)(int) )
// OFFSET = offset to call instruction from address returned by findPattern() (0 most of the time)
// OPCODE_SIZE = opcode size in bytes (e.g. 1u for "call") 
template <class FunctionPointerType, size_t OFFSET = 0u, size_t OPCODE_SIZE = 1u>
// requires Pointer<FunctionPointerType>
static FunctionPointerType getFunctionPointerByRelativeCall(const char * moduleName, const char * patternToCall) {
	static_assert(std::is_pointer<FunctionPointerType>::value, "violation of requirement Pointer<FunctionPointerType>");
	auto relative = findPatternBMH<void*, OFFSET>(moduleName, patternToCall);
	if (!relative) return nullptr;
	return getAbsoluteAddress<FunctionPointerType, OPCODE_SIZE>(relative);
}


memory::memoryManager::memoryManager()
{
	this->localPlayer = **findPatternBMH<playerent***, 2>("ac_client.exe", "\x8B\x35????\x83\xC4\x04\xE8????\x8B\xCD");
	this->entityList = *findPatternBMH<playerent****, 2>("ac_client.exe", "\x8B\x0D????\x8B\x34\x91\x85\xF6\x0F\x84????");
	this->entityCount = *findPatternBMH<size_t**, 2>("ac_client.exe", "\x39\x15????\x89\x54\x24\x14");
	this->viewMatrix = *findPatternBMH<float***, 1>("ac_client.exe", "\xB8????\xB9????\x5E\x83\xC4\x10");
	this->lookingAt = *findPatternBMH<vec**, 1>("ac_client.exe", "\xA3????\xD8\xC9\xD9\x5C\x24\x08\x8B\x4C\x24\x08");
	this->screenWidth = *findPatternBMH<size_t**,2>("ac_client.exe", "\x8B\x0D????\x83\xEC\x38\x56\x57\x50\x51");
	this->screenHeight = *findPatternBMH<size_t**, 1>("ac_client.exe", "\xA1????\x8B\x0D????\x83\xEC\x38");

	this->SDL_WM_GrabInput = helper::getExportedCdeclFunction<SDL_GrabMode, SDL_GrabMode>("SDL.dll", "SDL_WM_GrabInput");
	this->SDL_ShowCursor = helper::getExportedCdeclFunction<int, int>("SDL.dll", "SDL_ShowCursor");
	this->SDL_PollEvent = helper::getExportedCdeclFunction<int, SDL_Event*>("SDL.dll", "SDL_PollEvent");
	this->SDL_PushEvent = helper::getExportedCdeclFunction<int, SDL_Event*>("SDL.dll", "SDL_PushEvent");
	this->SDL_EventState = helper::getExportedCdeclFunction<Uint8, Uint32, int>("SDL.dll", "SDL_EventState");
	this->SDL_PeepEvents = helper::getExportedCdeclFunction<int, SDL_Event*, int, SDL_eventaction, Uint32, Uint32>("SDL.dll", "SDL_PeepEvents");

	// FIX!
	this->executeRet = getFunctionPointerByRelativeCall<int(__cdecl*)(const char*)>("ac_client.exe","\xE8????\x83\xC4\x04\x85\xC0\x74\x04\x89\x44\x24\x54");
	this->rayIntersectEnt = getFunctionPointerByRelativeCall<void*>("ac_client.exe", "\xE8????\x83\xC4\x14\x89\x03");
	this->isVisible = getFunctionPointerByRelativeCall<void*>("ac_client.exe", "\xE8????\x83\xC4\x18\x84\xC0\x74\x0C\xB8????");
	this->hudDrawString = getFunctionPointerByRelativeCall<void*, 5>("ac_client.exe", "\xB8????\xE8????\x83\xC4\x1C\x55\x8D\x54\x24\x3C");

	//auto relativeCallInstructionAddr = findPatternBMH<void*>("ac_client.exe", "\xE8????\x83\xC4\x14\x89\x03");
	//this->rayIntersectEnt = getAbsoluteAddress<void*>(relativeCallInstructionAddr);

	//relativeCallInstructionAddr = findPatternBMH<void*>("ac_client.exe", "\xE8????\x83\xC4\x18\x84\xC0\x74\x0C\xB8????");
	//this->isVisible = getAbsoluteAddress<uintptr_t>(relativeCallInstructionAddr);

	//relativeCallInstructionAddr = findPatternBMH<void*,5>("ac_client.exe", "\xB8????\xE8????\x83\xC4\x1C\x55\x8D\x54\x24\x3C");
	//this->hudDrawString = getAbsoluteAddress<uintptr_t>(relativeCallInstructionAddr);
}

playerent* memory::memoryManager::callRayIntersectEnt(float* distance, vec* from, vec* to, playerent* ent, int* bone)
{
	auto addr = this->rayIntersectEnt;
	__asm { // custom calling convention
		push 1; unk
		push bone
		push ent
		push to
		push from
		mov edi, distance
		call addr
		add esp, 0x14
	}
	// returns eax
}

bool memory::memoryManager::callIsVisible(const vec from, const vec to)
{
	auto addr = this->isVisible;
	__asm {
		push to.z
		push to.y
		push to.x
		push from.z
		push from.y
		push from.x

		xor cl,cl
		xor eax, eax

		call addr
		add esp, 0x18
		// result in al
	}
}

void memory::memoryManager::callHudDrawString(unsigned char color[3], const char* text, int x, int y)
{
	unsigned char red = color[0];
	unsigned char green = color[1];
	unsigned char blue = color[2];
	auto addr = this->hudDrawString;
	__asm {
		push -1
		push -1
		push blue
		push red
		push y
		push x
		push text
		mov al, green
		call addr
		add esp, 0x1C
	}
}
