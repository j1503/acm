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
---------------- TODOS ----------------

---------------
#### DONE #####
---------------
- fix imgui input (done)
- fix triggerbot/threads (done)
- fix threads/remove threads and hook game logic function instead (DONE)
- dont use input to shoot in triggerbot but instead the shoot function (done/used "attacking" bool)
- change input to use sdl hooks instead of wndproc (done)
- finish ieat hook (prob. done)
- change cheats/gui interface to make interaction with imgui bettter (halfway done -> added ConstCheckbox to imgui)
- add world2screen (done)
- fix drawing (done)
- fix aimbot FOV & SMOOTHING!!!

----------------
### NOT DONE ###
----------------

# IMPORTANT:
- fix that weird esp bug

# NOT IMPORTANT (FOR NOW):
- fix triggerbox & aimbot interaction
- fix imgui weird text after resize (dont know how)
- find lookingAt function
- improve aimbot for specifc weapons: snipers, etc..
- add locked target indicator (string/box)
- improve aimbot to snap when circle is not just in range of origin point, but generally over the enemy
- config catch different exceptions instead of one and handle them correctly
- triggerbot -> use execute(command) instead of input!
- handle case when not alive/spectating

# IDEAS:
- add fun functions with supersecret settings/features
- json use color struct instread of float arr
- better solution for general settings -> correct language for winapi strings (english) (maybe)
- esp show if bot 
- esp show name
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

	// order is important (fix?)
	globals::CheatManager->snaplines();
	globals::CheatManager->barrelesp();
	globals::CheatManager->aimbot();
	globals::CheatManager->triggerbot();
	globals::CheatManager->esp();
	globals::CheatManager->guns();

	// draw gui last
	globals::GUIManager->render();

	// handle special input (menu open/eject)
	if (globals::InputManager->getAsyncKeyState(SDLKey(globals::ConfigManager->active().general.ejectKey)))
		globals::HookManager->uninstall();
	if (globals::InputManager->getAsyncKeyState(SDLKey(globals::ConfigManager->active().general.menuKey)))
		globals::GUIManager->toggle();

	return globals::HookManager->sdlswapbuffers.getOriginalCdecl<void>("SDL.dll", "SDL_GL_SwapBuffers")();
}
