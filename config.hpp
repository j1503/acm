#ifndef ACM_CONFIG_HPP
#define ACM_CONFIG_HPP
#pragma once

#include <Windows.h>
#include <memory>

class configManager {
public:
	configManager();
	~configManager();
	void load() noexcept;
	void save() noexcept;
	void reset() noexcept;
public:
	struct general_conf {
		bool menuShown = false;
		unsigned char menuKey = VK_INSERT;
		unsigned char ejectKey = VK_END;
	} general;
	struct esp_conf {
		bool active = false;
		int enemyColor[3] = { 255,0,0 };
		int allyColor[3] = { 124,252,0 };
	} esp;
	struct aimbot_conf {
		bool active = false;
	} aimbot;
private:
	void clearFile() noexcept;
	static bool fileExists(const char* path) noexcept;
private:
	char path[MAX_PATH];
	HANDLE hFile;
};

namespace globals {
	inline std::unique_ptr<configManager> Config;
}

#endif