#ifndef ACM_GUI_GUI_HPP
#define ACM_GUI_GUI_HPP
#pragma once

#include "../config.hpp"

#include "imgui/imgui.h"
#include <memory>
#include <Windows.h>
#include <array>
#include <string>

namespace drawing {
	class gui {
	public:
		using config = config_manager::config;
		gui() = default;
		gui(HWND hWnd);
		~gui();
		gui(const gui&) = delete;
		gui& operator=(const gui&) = delete;
		void render() noexcept;
		void show() const noexcept;
		void hide() const noexcept;
		void toggle() const noexcept;
		config::menu_conf& options() const noexcept;
	private:
		void initStyle() const noexcept;
	private:
		static constexpr size_t tabcount = 7u;
		const HWND hWnd;
		size_t page;
		const char * tabs[tabcount];
	};
}

namespace globals {
	inline std::unique_ptr<drawing::gui> GUIManager;
}

#endif