#ifndef ACM_MEMORY_HPP
#define ACM_MEMORY_HPP
#pragma once

#include <memory>
#include <SDL.h>
#define NOMINMAX
#include <Windows.h>
#include <Psapi.h>
#include "sdk/geom.hpp"
#include "sdk/entity.hpp"

namespace memory
{
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
	};
}

namespace globals {
	inline std::unique_ptr<memory::memoryManager> MemoryManager;
}
#endif