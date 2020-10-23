#ifndef ACM_MEMORY_HPP
#define ACM_MEMORY_HPP
#pragma once

#include <memory>
#include <SDL.h>
#include <type_traits>
#define NOMINMAX
#include <Windows.h>
#include <Psapi.h>
#include "sdk/geom.hpp"
#include "sdk/entity.hpp"

namespace memory
{
	// pattern scanning
	class pfind {
	public:
		pfind() = default;
		explicit pfind(const char* moduleName);
		~pfind() = default;

		// ~7.5x faster than findNaiive (using boyer-moore-horspool)
		template <class T, size_t OFFSET = 0u>
		T find(const char* pattern) noexcept {
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
					return (T)(text + textPos + OFFSET);
				}
				textPos += table[curr];
			}
			return (T)0;
		}

		template <class T, size_t OFFSET = 0u>
		T findNaiive(const char* pattern) noexcept
		{
			char* start = static_cast<char*>(this->moduleInfo.lpBaseOfDll);
			const char* end = start + this->moduleInfo.SizeOfImage;

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
			return 0;
		}

		bool isUnique(const char* pattern) noexcept;

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
		inline FunctionPointerType getFunctionPointerByRelativeCall(const char* patternToCall) {
			static_assert(std::is_pointer<FunctionPointerType>::value, "violation of requirement Pointer<FunctionPointerType>");
			auto relative = this->find<void*, OFFSET>(patternToCall);
			if (!relative) return nullptr;
			return getAbsoluteAddress<FunctionPointerType, OPCODE_SIZE>(relative);
		}
	private:
		static bool patternSearchMemcmp(const unsigned char* word, const unsigned char* text, size_t size);
	private:
		MODULEINFO moduleInfo;
	};
	class memoryManager {
	public:
		memoryManager();
		~memoryManager() = default;
		// pointers
		playerent* localPlayer;
		playerent*** entityList;
		size_t* entityCount;
		float** viewMatrix;
		vec* lookingAt;
		size_t* screenWidth;
		size_t* screenHeight;
		// function pointers
		// sdl
		SDL_GrabMode(__cdecl* SDL_WM_GrabInput)(SDL_GrabMode);
		int(__cdecl* SDL_ShowCursor)(int toggle);
		Uint8(__cdecl* SDL_EventState)(Uint32 type, int state);
		int(__cdecl*SDL_PollEvent)(SDL_Event* event);
		int(__cdecl* SDL_PushEvent)(SDL_Event* event);
		int(__cdecl* SDL_PeepEvents)(SDL_Event* events, int numevents, SDL_eventaction action, Uint32 mintype, Uint32 maxType);
		// other
		int(__cdecl* executeRet)(const char* p);
		void* rayIntersectEnt; // __usercall (ukn1@<edi>) -> cant use function pointer
		void* isVisible;
		void* hudDrawString; // __usercall (GLubyte green@<al>) 
		// extra (helper) functions
		playerent* callRayIntersectEnt(float* distance, vec* from, vec* to, playerent* ent, int* bone);
		bool callIsVisible(const vec from, const vec to);
		void callHudDrawString(unsigned char color[3], const char* text, int x, int y);
	private:
		pfind ac_client;
	};
}

namespace globals {
	inline std::unique_ptr<memory::memoryManager> MemoryManager;
}
#endif