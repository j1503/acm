#include "config.hpp"
#include "hookmanager.hpp"

#include "json.hpp"
#include <Windows.h>
#include <exception>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"

#define VALID_INDEX(idx, container) (idx >= 0) && (idx < container.size())

#define CHEAT_NAME "ACM"
#define PROFILES_KEY_NAME "profiles"

using json = nlohmann::json;
using namespace std::string_literals;

/*
Add a new cheat:
1. implement abstract class (hpp+cpp file, inherit from base)
2. add config 
3. add to cheatmanager
4. add run() to main/hk
(5. add to gui) 
*/

// custom to_json functions
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configManager::config::menu_conf, active, menuKey, ejectKey);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configManager::config::misc_conf, supersecret);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configManager::config::esp_conf, active, teammates, healthbar, enemyColor, allyColor);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configManager::config::aimbot_conf, active, friendlyfire, showcircle, circlecolor, autoshoot, targetLock, smoothing,
	range, onkey, hotkey, rangevalue, smoothvalue, fov);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configManager::config::triggerbot_conf, active, friendlyfire);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configManager::config::snaplines_conf, active, enemyColor, allyColor, linewidth, teammates);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configManager::config::barrelesp_conf, active, enemyColor, allyColor, linewidth, linelength, teammates);

// options
configManager::config::config(configManager& parent)
	: parent(parent)
{}

configManager::config::config(configManager& parent, std::string name)
	: parent(parent), name(name) {}

void configManager::config::load(json& j)
{
	this->name = j["name"];
	//this->general = j["general"];
	this->misc = j["misc"];
	this->esp = j["esp"];
	this->aimbot = j["aimbot"];
	this->triggerbot = j["triggerbot"];
	this->snaplines = j["snaplines"];
	this->barrelesp = j["barrelesp"];
}

void configManager::config::save() noexcept
{
	this->parent.saveFile();
}

void configManager::config::reset() noexcept
{
	this->general = menu_conf{};
	this->misc = misc_conf{};
	this->esp = esp_conf{};
	this->aimbot = aimbot_conf{};
	this->triggerbot = triggerbot_conf{};
	this->snaplines = snaplines_conf{};
	this->barrelesp = barrelesp_conf{};
}

std::string configManager::config::toString() const noexcept
{
	json j;
	j["name"] = this->name;
	//j["general"] = this->general;
	j["misc"] = this->misc;
	j["esp"] = this->esp;
	j["aimbot"] = this->aimbot;
	j["triggerbot"] = this->triggerbot;
	j["snaplines"] = this->snaplines;
	j["barrelesp"] = this->barrelesp;
	return j.dump(4, ' ', true);
}

configManager::configManager(HWND hWnd)
	: hWnd(hWnd)
{
	SHGetFolderPath(0, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, 0, 0, this->path);
	PathAppend(this->path, CHEAT_NAME);
	CreateDirectory(this->path, 0);
	PathAppend(this->path, "config.json");
	if (!configManager::fileExists(this->path)) {
		this->hFile = CreateFile(this->path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		this->createDefault();
	}
	else {
		this->hFile = CreateFile(this->path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		this->loadFile();
	}
}

// configManager
configManager::~configManager()
{
	this->saveFile();
	CloseHandle(this->hFile);
}

bool configManager::renameProfile(std::string currentName, std::string newName) noexcept
{
	if (!this->isUniqueProfileName(newName)) return false;
	auto& c = this->findProfileByName(currentName);
	c.name = newName;
	this->saveFile();
	return true;
}

bool configManager::renameProfile(size_t index, std::string newName) noexcept
{
	if (VALID_INDEX(index, this->profiles)) {
		this->profiles[index]->name = newName;
		this->saveFile();
		return true;
	}
	return false;
}

void configManager::setActiveProfile(std::string name) noexcept
{
	for (size_t i = 0; i < this->profiles.size(); ++i) {
		if (this->profiles[i]->name == name) {
			this->activeIdx = i;
			globals::ActiveProfile = this->profiles[this->activeIdx].get();
			return;
		}
	}
}

void configManager::setActiveProfile(size_t index) noexcept
{
	if (VALID_INDEX(index, this->profiles)) {
		this->activeIdx = index;
		globals::ActiveProfile = this->profiles[this->activeIdx].get();
	}
}

configManager::config& configManager::getActiveProfile() noexcept
{
	return *this->profiles[activeIdx];
}

configManager::config& configManager::getProfile(std::string name) noexcept
{
	return this->findProfileByName(name);
}

configManager::config& configManager::getProfile(size_t index) noexcept
{
	if (VALID_INDEX(index, this->profiles)) {
		return *this->profiles[index];
	}
	return this->getActiveProfile();
}

bool configManager::createProfile(std::string name) noexcept
{
	if (!this->isUniqueProfileName(name)) return false;
	else {
		this->profiles.emplace_back(std::make_unique<configManager::config>(*this, name));
		this->activeIdx = this->profiles.size() - 1;
		globals::ActiveProfile = this->profiles[this->activeIdx].get();
		return true;
	}
}

bool configManager::deleteProfile(std::string name) noexcept
{
	for (size_t i = 0; i < this->profiles.size(); ++i) {
		if (this->profiles[i]->name == name) {
			this->profiles.erase(this->profiles.begin() + i);
			--this->activeIdx;
			if (this->activeIdx < 0) this->activeIdx = 0;
			globals::ActiveProfile = this->profiles[this->activeIdx].get();
			return true;
		}
	}
	return false;
}

bool configManager::deleteProfile(size_t index) noexcept
{
	if (VALID_INDEX(index, this->profiles)) {
		this->profiles.erase(this->profiles.begin() + index);
		--this->activeIdx;
		if (this->activeIdx < 0) this->activeIdx = 0;
		globals::ActiveProfile = this->profiles[this->activeIdx].get();
		return true;
	}
	return false;
}

configManager::config& configManager::findProfileByName(std::string& name)
{
	for (auto& c : this->profiles) {
		if (c->name == name) {
			return *c;
		}
	}
	return this->getActiveProfile();
}

bool configManager::isUniqueProfileName(std::string& name) const noexcept
{
	for (auto& c : this->profiles) {
		if (c->name == name) return false;
	}
	return true;
}

void configManager::createDefault() noexcept
{
	this->activeIdx = 0;
	this->clearFile();
	this->profiles.clear();
	this->profiles.emplace_back(std::make_unique<configManager::config>(*this, "profile-1"));
	this->profiles.emplace_back(std::make_unique<configManager::config>(*this, "profile-2"));
	this->profiles.emplace_back(std::make_unique<configManager::config>(*this, "profile-3"));
	this->saveFile();
	globals::ActiveProfile = this->profiles[this->activeIdx].get();
}

void configManager::loadFile()
{
	size_t len = GetFileSize(this->hFile, 0);
	std::unique_ptr<char[]> buf(new char[len + 1]);
	std::fill_n(buf.get(), len + 1, 0);
	if (!ReadFile(this->hFile, buf.get(), len, 0, 0)) {
		globals::HookManager->uninstall();
		return;
	}
	json j;
	try {
		// parse json and throw (-> create default) if empty
		j = json::parse(buf.get());
		if (j[PROFILES_KEY_NAME].empty()) throw std::length_error("");
		// getProfile active profile, if the key is found, else set it to the first profile
		auto active = j.find("active");
		if (active != j.end()) {
			this->activeIdx = active.value().get<int>();
		}
		else {
			this->activeIdx = 0;
		}
		this->profiles.clear(); // clear current array of profiles
		// fill profiles with data from json
		for (auto& c : j[PROFILES_KEY_NAME]) {
			this->profiles.emplace_back(std::make_unique<configManager::config>(*this))->load(c);
		}
		globals::ActiveProfile = this->profiles[this->activeIdx].get();
	}
	catch (...) { /*json::parse_error& e*/
		this->resetProfiles();
	}
}

void configManager::saveFile() noexcept
{
	// getProfile current settings as json dump
	json j;
	for (auto& c : this->profiles) {
		j[PROFILES_KEY_NAME].push_back(json::parse(c->toString()));
	}
	j["active"] = this->activeIdx;
	std::string x = j.dump(4, ' ', true);
	const char* buf = x.c_str();
	// write to file
	this->clearFile();
	WriteFile(this->hFile, buf, x.length(), 0, 0);
}

void configManager::clearFile() noexcept
{
	CloseHandle(this->hFile);
	this->hFile = CreateFile(this->path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
}

void configManager::resetProfiles() noexcept
{
	std::string errorString = "Your configuration had to be reset due to a corruption in the config file at "s + std::string(this->path);
	this->createDefault();
	LONG_PTR wndprocb4 = GetWindowLongPtr(hWnd, GWLP_WNDPROC);
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)globals::HookManager->originalWndProc); // fallback to originalWndProc to click messagebox
	// this is required because the custom wndproc depends on ConfigManager and will otherwise crash
	int id = MessageBox(this->hWnd, errorString.c_str(), "Information", MB_OK | MB_ICONEXCLAMATION);
	if (id != IDOK)
		return globals::HookManager->uninstall();
	else
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, wndprocb4);
}

bool configManager::fileExists(const char* path) noexcept
{
	DWORD dwAttrib = GetFileAttributes(path);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}