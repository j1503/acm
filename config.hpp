#ifndef ACM_CONFIG_HPP
#define ACM_CONFIG_HPP
#pragma once

#include <Windows.h>
#include <memory>
#include <vector>
#include <string>
#include <SDL_events.h>
#include "json.hpp"

class configManager {
public:
	class config {
		friend class configManager;
	public:
		config() = default;
		config(configManager& parent);
		config(configManager& parent, std::string name);
		void load(nlohmann::json& j);
		void save() noexcept;
		void reset() noexcept;
	private:
		std::string toString() const noexcept;
	private:
	public:
		std::string name;
		// configs here - remember to add it to json too in options.cpp!
		struct base_conf {
			bool active = false;
		};
		struct menu_conf : public base_conf {
			size_t menuKey = SDLK_INSERT;
			size_t ejectKey = SDLK_END;
		} general;
		struct misc_conf {
			bool supersecret = false;
		} misc;
		struct esp_conf : public base_conf {
			float enemyColor[3] = { 1.f, 0.f, 0.f };
			float allyColor[3] = { 0.48f, 0.98f, 0.f };
		} esp;
		struct aimbot_conf : public base_conf {
			bool friendlyfire = false;
			bool showcircle = false;
			float circlecolor[3] = { 1.f, 1.f, 1.f };
			bool autoshoot = false;
			bool targetLock = false;
			bool smoothing = false;
			float smoothvalue = 10.f;
			float fov = 10.f;
		} aimbot;
		struct triggerbot_conf : public base_conf {
			bool friendlyfire = false;
		} triggerbot;
		struct tracelines_conf : public base_conf {
			float enemyColor[3] = { 1.f, 0.f, 0.f };
			float allyColor[3] = { 0.48f, 0.98f, 0.f };
			float linewidth = 1.f;
			bool teammates = false;
		} tracelines;
		struct barrelesp_conf : public base_conf {
			float enemyColor[3] = { 1.f, 0.f, 0.f };
			float allyColor[3] = { 0.48f, 0.98f, 0.f };
			float linewidth = 1.f;
			float linelength = 8.f;
			bool teammates = false;
		} barrelesp;
		//
	private:
		configManager& parent;
	};
public:
	configManager() = default;
	configManager(HWND hWnd);
	~configManager();
	// crud
	bool createProfile(std::string name) noexcept;
	configManager::config& getActiveProfile() noexcept;
	// will return the currently active profile if the item at the index does not exist
	configManager::config& getProfile(std::string name) noexcept;
	// will return the currently active profile if the item at the index does not exist
	configManager::config& getProfile(size_t index) noexcept;
	void setActiveProfile(std::string name) noexcept;
	void setActiveProfile(size_t index) noexcept;
	bool renameProfile(std::string currentName, std::string newName) noexcept;
	bool renameProfile(size_t index, std::string newName) noexcept;
	bool deleteProfile(std::string name) noexcept;
	bool deleteProfile(size_t index) noexcept;
private:
	configManager::config& findProfileByName(std::string& name);
	bool isUniqueProfileName(std::string& name) const noexcept;
	void createDefault() noexcept;
	void loadFile();
	void saveFile() noexcept;
	void clearFile() noexcept;
	void resetProfiles() noexcept;
	static bool fileExists(const char* path) noexcept;
public:
	std::vector<std::unique_ptr<configManager::config>> profiles;
private:
	int activeIdx;
	HANDLE hFile;
	HWND hWnd;
	char path[MAX_PATH];
};

namespace globals {
	inline std::unique_ptr<configManager> ConfigManager;
	inline configManager::config* ActiveProfile = nullptr;
}

#endif