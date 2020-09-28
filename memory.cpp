#include "memory.hpp"
#include <Windows.h>
#include <Psapi.h>

uintptr_t memory::findPatternNaiive(const char* moduleName, const char* pattern) noexcept
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
				return reinterpret_cast<uintptr_t>(start);
		}
	}
	return 0;
}

// ~7.5x faster than findPatternNaiive
uintptr_t memory::findPatternBMH(const char* moduleName, const char* pattern) noexcept
{
	HMODULE hModule = GetModuleHandle(moduleName);
	if (hModule) {
		MODULEINFO moduleInfo;
		if (GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo))) {
			auto patternSearchMemcmp = [](const unsigned char* word, const unsigned char* text, size_t size) -> bool {
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
					return (uintptr_t)text + textPos;
				}
				textPos += table[curr];
			}
		}
	}
	return 0;
}

bool memory::isPatternUnique(const char* moduleName, const char* pattern) noexcept
{
	size_t occ = 0;
	HMODULE hModule = GetModuleHandle(moduleName);
	if (hModule) {
		MODULEINFO moduleInfo;
		if (GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo))) {
			auto patternSearchMemcmp = [](const unsigned char* word, const unsigned char* text, size_t size) -> bool {
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

memory::memoryManager::memoryManager()
{
	this->localPlayer = **(playerent***)(memory::findPatternBMH("ac_client.exe", "\x8B\x35????\x83\xC4\x04\xE8????\x8B\xCD") + 2);
	this->entityList = **(playerent****)(memory::findPatternBMH("ac_client.exe", "\x8B\x0D????\x8B\x34\x91\x85\xF6\x0F\x84????") + 2);
	this->entityCount = *(size_t**)(memory::findPatternBMH("ac_client.exe", "\x39\x15????\x89\x54\x24\x14") + 2);
	this->viewMatrix = (glm::mat4*)(memory::findPatternBMH("ac_client.exe", "\xE8????\x38\x5C\x24\x3C") + 1);
}

memory::memoryManager::~memoryManager()
{
}