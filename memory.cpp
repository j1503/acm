#include "memory.hpp"

#include <gl/GL.h>
#pragma comment(lib,"opengl32.lib")
#include "helper.hpp"
#include "hookmanager.hpp"

// pfind class
memory::pfind::pfind(const char* moduleName)
{
	auto hMod = GetModuleHandle(moduleName);
	if (!hMod) return;
	GetModuleInformation(GetCurrentProcess(), hMod, &this->moduleInfo, sizeof(this->moduleInfo));
}


bool memory::pfind::isUnique(const char* pattern) noexcept
{
	size_t occ = 0;
	const unsigned char* text = (const unsigned char*)this->moduleInfo.lpBaseOfDll;
	const unsigned char* word = (const unsigned char*)pattern;

	const size_t textLength = this->moduleInfo.SizeOfImage;
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
		if (((curr == lastWordChar) || lastWordChar == '?') && pfind::patternSearchMemcmp(word, text + textPos, maxWordIdx)) {
			++occ;
		}
		textPos += table[curr];
	}
	return occ == 1;
}

bool memory::pfind::patternSearchMemcmp(const unsigned char* word, const unsigned char* text, size_t size)
{
	while (size) {
		if (*word != '?') {
			if (*word != *text) return false;
		}
		--size;
		++word;
		++text;
	}
	return true;
}

memory::memoryManager::memoryManager()
	: ac_client("ac_client.exe")
{
	this->gamemode = *this->ac_client.find<int32_t**, 2>("\x89\x0D????\x83\xFE\xFF\x75\x04\xB0\x01\xEB\x16");
	this->localPlayer = **this->ac_client.find<playerent***, 2>("\x8B\x35????\x83\xC4\x04\xE8????\x8B\xCD");
	this->entityList = *this->ac_client.find<playerent****, 2>("\x8B\x0D????\x8B\x34\x91\x85\xF6\x0F\x84????");
	this->entityCount = *this->ac_client.find<size_t**, 2>("\x39\x15????\x89\x54\x24\x14");
	this->viewMatrix = *this->ac_client.find<float***, 1>("\xB8????\xB9????\x5E\x83\xC4\x10");
	this->lookingAt = *this->ac_client.find<vec**, 1>("\xA3????\xD8\xC9\xD9\x5C\x24\x08\x8B\x4C\x24\x08");
	this->screenWidth = *this->ac_client.find<size_t**,2>("\x8B\x0D????\x83\xEC\x38\x56\x57\x50\x51");
	this->screenHeight = *this->ac_client.find<size_t**, 1>("\xA1????\x8B\x0D????\x83\xEC\x38");

	this->SDL_WM_GrabInput = helper::getExportedCdeclFunction<SDL_GrabMode, SDL_GrabMode>("SDL.dll", "SDL_WM_GrabInput");
	this->SDL_ShowCursor = helper::getExportedCdeclFunction<int, int>("SDL.dll", "SDL_ShowCursor");
	this->SDL_PollEvent = helper::getExportedCdeclFunction<int, SDL_Event*>("SDL.dll", "SDL_PollEvent");
	this->SDL_PushEvent = helper::getExportedCdeclFunction<int, SDL_Event*>("SDL.dll", "SDL_PushEvent");
	this->SDL_EventState = helper::getExportedCdeclFunction<Uint8, Uint32, int>("SDL.dll", "SDL_EventState");
	this->SDL_PeepEvents = helper::getExportedCdeclFunction<int, SDL_Event*, int, SDL_eventaction, Uint32, Uint32>("SDL.dll", "SDL_PeepEvents");

	// FIX!
	this->executeRet = this->ac_client.getFunctionPointerByRelativeCall<int(__cdecl*)(const char*)>("\xE8????\x83\xC4\x04\x85\xC0\x74\x04\x89\x44\x24\x54");
	
	this->rayIntersectEnt = this->ac_client.getFunctionPointerByRelativeCall<void*>("\xE8????\x83\xC4\x14\x89\x03");
	this->isVisible = this->ac_client.getFunctionPointerByRelativeCall<void*>("\xE8????\x83\xC4\x18\x84\xC0\x74\x0C\xB8????");
	this->hudDrawString = this->ac_client.getFunctionPointerByRelativeCall<void*, 5>("\xB8????\xE8????\x83\xC4\x1C\x55\x8D\x54\x24\x3C");
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
