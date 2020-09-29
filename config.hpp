#ifndef ACM_CONFIG_HPP
#define ACM_CONFIG_HPP
#pragma once

#include <Windows.h>
#include <memory>
#include <vector>
#include <string>
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
	public:
		std::string name;
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
		configManager& parent;
	};

public:
	configManager();
	~configManager();
	// crud
	bool createProfile(std::string name) noexcept;
	configManager::config& getActiveProfile() noexcept;
	configManager::config& getProfile(std::string name) noexcept;
	void setActiveProfile(std::string name) noexcept;
	bool renameProfile(std::string currentName, std::string newName) noexcept;
	bool deleteProfile(std::string name) noexcept;
private:
	configManager::config& findProfileByName(std::string& name);
	bool isUniqueProfileName(std::string& name) const noexcept;
	void createDefault() noexcept;
	void loadFile();
	void saveFile() noexcept;
	void clearFile() noexcept;
	static bool fileExists(const char* path) noexcept;
public:
	std::vector<std::unique_ptr<configManager::config>> profiles;
private:
	size_t activeIdx;
	HANDLE hFile;
	char path[MAX_PATH];
};

namespace globals {
	inline std::unique_ptr<configManager> ConfigManager;
}

#endif