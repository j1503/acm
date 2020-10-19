#include "hookedfunctions.hpp"

#include <gl/GL.h>
#pragma comment(lib,"opengl32.lib")
#include "sdk/geom.hpp"

#include "config.hpp"
#include "debugconsole.hpp"
#include "helper.hpp"
#include "input.hpp"
#include "hookmanager.hpp"
#include "memory.hpp"
#include "cheats/cheatmanager.hpp"
#include "gui/gui.hpp"
#include "drawing.hpp"

/*
TODO
- fix imgui input (done)
- fix triggerbot/threads (done)
- fix threads/remove threads and hook game logic function instead (DONE)
- dont use input to shoot in triggerbot but instead the shoot function (done/used "attacking" bool)
- change input to use sdl hooks instead of wndproc (done)
- finish ieat hook (prob. done)
- change cheats/gui interface to make interaction with imgui bettter (halfway done -> added ConstCheckbox to imgui)
- add world2screen (done)
- fix drawing (done)

!!! - fix triggerbot -> use execute(command) instead of input! 

- fix imgui weird text after resize (dont know how)
- got teammates check
- json use color struct instread of float arr
- better solution for general settings -> correct language for winapi strings (english) (maybe)
- find lookingAt function
- fix aimbot FOV & SMOOTHING!!!
- improve aimbot for specifc weapons: snipers, etc.. 
- add locked target indicator (string/box)
- improve aimbot to snap when circle is not just in range of origin point, but generally over the enemy
- add fun functions with supersecret settings/features
- fix triggerbox & aimbot interaction 
- add aimbot range (how close enemy must be) 
*/
//

int hookedfunctions::hkSDL_PollEvent(SDL_Event* event)
{
	auto r = globals::HookManager->pollevents.getGatewayCdecl<int, SDL_Event*>()(event);

	globals::InputManager->handleInput(event);
	
	return r;
}

void hookedfunctions::hkSDL_GL_SwapBuffers()
{
	float fontscale = *globals::MemoryManager->screenWidth * 0.0003f;
	drawing::drawString(colors::white, "Press [INSERT] to open the menu", int(*globals::MemoryManager->screenWidth * 0.45f),
		int(*globals::MemoryManager->screenHeight * 0.025f), fontscale);

	// draw gui
	globals::GUIManager->render();
	
	// order is important (fix?)
	globals::CheatManager->tracelines();
	globals::CheatManager->barrelesp();
	globals::CheatManager->aimbot();
	globals::CheatManager->triggerbot();

	// handle special input (menu open/eject)
	if (globals::InputManager->getAsyncKeyState(SDLKey(globals::ActiveProfile->general.ejectKey)))
		globals::HookManager->uninstall();
	if (globals::InputManager->getAsyncKeyState(SDLKey(globals::ActiveProfile->general.menuKey)))
		globals::GUIManager->toggle();

	return globals::HookManager->sdlswapbuffers.getOriginalCdecl<void>("SDL.dll", "SDL_GL_SwapBuffers")();
}
