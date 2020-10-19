#ifndef ACM_INPUT_HPP
#define ACM_INPUT_HPP
#pragma once

#include <memory>
#include <Windows.h>
#include <SDL.h>

class input {
public:
	input();
	~input() = default;
	/**
	* @param keycode: keycode from the SDLMod struct, e.g. SDLK_F1
	* @return true if key is down, false if not
	*/
	bool getKeyState(SDLKey keycode) const noexcept;

	/**
	* @param keycode: keycode from the SDLMod struct, e.g. SDLK_F1
	* @return true if key is down and was not down at a previous call to getAsyncKeyState, false if not
	*/
	bool getAsyncKeyState(SDLKey keycode) noexcept;

	// internal
	void handleInput(SDL_Event* event) noexcept;

	/**
	* @param state: SDL_PRESSED or SDL_RELEASED
	* @param keycode: keycode from the SDLMod struct, e.g. SDLK_F1
	* @return true if event was sent, false if not
	*/
	bool sendKeyboardEvent(uint8_t state, SDLKey keycode, bool onlyActive = true) noexcept;
	/**
	* @param button: SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, SDL_BUTTON_RIGHT
	* @param state: SDL_PRESSED or SDL_RELEASED
	* @return true if event was sent, false if not
	*/
	bool sendMouseEvent(uint8_t button, uint8_t state, bool onlyActive = true) noexcept;
public:
	uint16_t x, y;
	bool windowActive;
	bool leftMouseDown;
	bool rightMouseDown;
private:
	static constexpr size_t KEYCOUNT = 400u;
	bool keys[KEYCOUNT];
	bool prevkeys[KEYCOUNT]; // previous call 
};

namespace globals {
	inline std::unique_ptr<input> InputManager;
}

#endif