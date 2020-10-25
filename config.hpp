#ifndef ACM_CONFIG_HPP
#define ACM_CONFIG_HPP
#pragma once

#include "json.hpp" // json 
#include <vector> // vector
#include <string> // string
#include <filesystem> // path, exists(), etc.
#include <SDL_events.h> // SDLK_INSERT, SDLK_END, etc.

class config_manager {
	using json = nlohmann::json;
public:
	class config {
		friend class config_manager;
	public:
		config() = default;
		config(const config_manager& parent);
		config(const config_manager& parent, std::string name);
		void load(json& j);
		void save() const;
		void reset();
	private:
		// get config as json
		json get() const; 
		// get config as json-string-dump
		std::string dump() const; 
	public:
		std::string name;
		// configs here - remember to add it to json too in config.cpp!
#pragma region cheat_configs
		struct base_conf {
			bool active = false;
		};
		struct menu_conf final : public base_conf {
			size_t menuKey = SDLK_INSERT;
			size_t ejectKey = SDLK_END;
		} general;
		struct misc_conf final {
			bool supersecret = false;
		} misc;
		struct guns_conf final : public base_conf {
			bool reload = false;
			bool rapidfire = false;
			bool nospread = false;
			bool infammo = false;
			bool recoil = false;
			bool kickback = false;
			bool fullauto = false;
		} guns;
		struct esp_conf final : public base_conf {
			bool teammates = false;
			bool healthbar = false;
			float enemyColor[3] = { 1.f, 0.f, 0.f };
			float allyColor[3] = { 0.48f, 0.98f, 0.f };
		} esp;
		struct aimbot_conf final : public base_conf {
			bool friendlyfire = false;
			bool showcircle = false;
			float circlecolor[3] = { 1.f, 1.f, 1.f };
			bool autoshoot = false;
			bool targetLock = false;
			bool smoothing = false;
			bool range = false;
			bool onkey = false;
			size_t hotkey = SDLK_LSHIFT;
			float rangevalue = 10.f;
			float smoothvalue = 10.f;
			float fov = 10.f;
		} aimbot;
		struct triggerbot_conf final : public base_conf {
			bool friendlyfire = false;
		} triggerbot;
		struct snaplines_conf final : public base_conf {
			float enemyColor[3] = { 1.f, 0.f, 0.f };
			float allyColor[3] = { 0.48f, 0.98f, 0.f };
			float linewidth = 1.f;
			bool teammates = false;
		} snaplines;
		struct barrelesp_conf final : public base_conf {
			float enemyColor[3] = { 1.f, 0.f, 0.f };
			float allyColor[3] = { 0.48f, 0.98f, 0.f };
			float linewidth = 1.f;
			float linelength = 8.f;
			bool teammates = false;
		} barrelesp;
#pragma endregion
	private:
		const config_manager& parent;
	};
public:
	config_manager();
	~config_manager();
	config_manager(const config_manager&) = delete;
	config_manager& operator=(const config_manager&) = delete;
	// load all
	void load();
	// save all
	void save() const;
	// create
	void create(std::string name) noexcept;
	// read/update
	config& get(size_t idx);
	const config& get(size_t idx) const;
	config& active();
	const config& active() const;
	void setActive(size_t idx);
	// delete 
	bool remove();
private:
	// resets all profiles and the current profile is set to a default profile (which is created)
	void reset();
	std::string dump() const;
	
	static std::string getProgramDataPath();
private:
	std::vector<config> profiles;
	size_t activeidx;
	std::filesystem::path path;
	static constexpr const char foldername[] = "ACM";
	static constexpr const char filename[] = "config.json";
};

namespace globals {
	inline std::unique_ptr<config_manager> ConfigManager;
}

#endif