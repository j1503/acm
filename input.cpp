#include "input.hpp"

#include <cassert>
#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")
#include "memory.hpp"

input::input()
	: x(0), y(0), windowActive(false), leftMouseDown(false), rightMouseDown(false), keys{}, prevkeys{}
{
	assert(globals::MemoryManager);
}

bool input::getKeyState(SDLKey keycode) const noexcept
{
	return this->keys[keycode];
}


bool input::getAsyncKeyState(SDLKey keycode) noexcept
{
	bool ret = false;
	if (this->keys[keycode] && (this->keys[keycode] != this->prevkeys[keycode]))
		ret = true;
	this->prevkeys[keycode] = this->keys[keycode];
	return ret;
}

void input::handleInput(SDL_Event* event) noexcept
{

	switch (event->type) {
	case SDL_MOUSEMOTION:
		this->x = event->motion.x;
		this->y = event->motion.y;
		break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		if (event->button.button == SDL_BUTTON_LEFT) {
			this->leftMouseDown = event->button.state;
		}
		else if (event->button.button == SDL_BUTTON_RIGHT) {
			this->rightMouseDown = event->button.state;
		}
		break;
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		if (event->key.keysym.sym < input::KEYCOUNT) {
			this->keys[event->key.keysym.sym] = event->key.state;
		}
		break;
	case SDL_ACTIVEEVENT:
		this->windowActive = event->active.state;
		break;
	}
}

bool input::sendKeyboardEvent(uint8_t state, SDLKey keycode, bool onlyActive) noexcept
{
	if ((onlyActive && this->windowActive) || !onlyActive) {
		SDL_Event event;
		SecureZeroMemory(&event, sizeof(event));
		if ((event.key.state = state) == SDL_PRESSED) 
			event.key.type = SDL_KEYDOWN;
		else if((event.key.state = state) == SDL_RELEASED) 
			event.key.type = SDL_KEYUP;
		else 
			return false;
		event.type = event.key.type;
		if (keycode >= input::KEYCOUNT) return false;
		event.key.keysym.sym = keycode;
		return globals::MemoryManager->SDL_PushEvent(&event);
	}
	return false;
}

bool input::sendMouseEvent(uint8_t button, uint8_t state, bool onlyActive) noexcept
{
	if ((onlyActive && this->windowActive) || !onlyActive) {
		SDL_Event event;
		SecureZeroMemory(&event, sizeof(event));
		if ((event.button.state = state) == SDL_PRESSED)
			event.type = event.button.type = SDL_MOUSEBUTTONDOWN;
		else if ((event.button.state = state) == SDL_RELEASED)
			event.type = event.button.type = SDL_MOUSEBUTTONUP;
		else
			return false;
		// between SDL_BUTTON_LEFT (=1) and SDL_BUTTON_RIGHT (=3)
		if ((button < 1) || (button > 3))
			return false;
		event.button.button = button;
		//event.button.x = this->x;
		//event.button.y = this->y;
		event.button.x = -1;
		event.button.y = -1;
		return globals::MemoryManager->SDL_PushEvent(&event);
	}
	return false;
}
