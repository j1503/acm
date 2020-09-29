#include "config.hpp"
#include "hookmanager.hpp"
#include "json.hpp"
#include <Windows.h>
#include <exception>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"

#define CHEAT_NAME "ACM"
#define PROFILES_KEY_NAME "profiles"

using json = nlohmann::json;
using namespace std::string_literals;

// custom to_json functions
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configManager::config::general_conf, menuShown, menuKey, ejectKey);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configManager::config::esp_conf, active, enemyColor, allyColor);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configManager::config::aimbot_conf, active);

configManager::config::config(configManager& parent, std::string name)
	: parent(parent), name(name)
{}

void configManager::config::load(json& j)
{
	this->general = j["general"];
	this->esp = j["esp"];
	this->aimbot = j["aimbot"];
}

void configManager::config::save() noexcept
{
	this->parent.saveFile();
}

void configManager::config::reset() noexcept
{
	this->general = general_conf{};
	this->esp = esp_conf{};
	this->aimbot = aimbot_conf{};
}

std::string configManager::config::toString() const noexcept
{
	json j;
	j["general"] = this->general;
	j["esp"] = this->esp;
	j["aimbot"] = this->aimbot;
	return j.dump(4, ' ', true);
}

configManager::configManager()
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

configManager::~configManager()
{
	this->saveFile();
	CloseHandle(this->hFile);
}

void configManager::changeProfileName(std::string currentName, std::string newName) noexcept
{
	configManager::config& c = this->findProfileByName(currentName);
	c.name = newName;
	this->saveFile();
}

void configManager::setActiveProfile(std::string name) noexcept
{
	for (size_t i = 0; i < this->configs.size(); ++i) {
		if (this->configs[i]->name == name) this->activeIdx = i;
	}
}

configManager::config& configManager::getActiveProfile() noexcept
{
	return *this->configs[activeIdx];
}

configManager::config& configManager::getProfile(std::string name) noexcept
{
	return this->findProfileByName(name);
}

void configManager::createProfile(std::string name) noexcept
{
	if (!this->isUniqueProfileName(name)) return;
	else {
		this->configs.emplace_back(std::make_unique<configManager::config>(*this, name));
		this->activeIdx = this->configs.size() - 1;
	}
}

void configManager::deleteProfile(std::string name) noexcept
{
	for (size_t i = 0; i < this->configs.size(); ++i) {
		if (this->configs[i]->name == name) {
			this->configs.erase(this->configs.begin() + i);
			--this->activeIdx;
		}
	}
}

configManager::config& configManager::findProfileByName(std::string& name)
{
	for (auto& c : this->configs) {
		if (c->name == name) {
			return *c;
		}
	}
	throw std::runtime_error("findProfileByName: Profile not found");
}

bool configManager::isUniqueProfileName(std::string& name) const noexcept
{
	for (auto& c : this->configs) {
		if (c->name == name) return false;
	}
	return true;
}

void configManager::createDefault() noexcept
{
	this->activeIdx = 0;
	this->clearFile();
	this->configs.emplace_back(std::make_unique<configManager::config>(*this, "profile-1"));
	this->configs.emplace_back(std::make_unique<configManager::config>(*this, "profile-2"));
	this->configs.emplace_back(std::make_unique<configManager::config>(*this, "profile-3"));
	this->saveFile();
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
		if (j[PROFILES_KEY_NAME].empty()) throw std::exception();
		// getProfile active profile, if the key is found, else set it to the first profile
		auto active = j.find("active");
		if (active != j.end()) {
			this->activeIdx = active.value().get<size_t>();
		}
		else {
			this->activeIdx = 0;
		}
		this->configs.clear(); // clear current array of profiles, incase
		// fill profiles with data from json
		size_t idx = 0;
		for (auto& [key, value] : j[PROFILES_KEY_NAME].items()) {
			this->configs.emplace_back(std::make_unique<configManager::config>(*this, key));
			this->configs[idx]->load(value);
			++idx;
		}
	}
	catch (...) { /*json::parse_error& e*/
		std::string errorString = "Your configuration had to be reset due to a corruption in the config file at "s + std::string(this->path);
		MessageBox(globals::HookManager->hWnd, errorString.c_str(), "Information", MB_OK | MB_ICONEXCLAMATION);
		this->createDefault();
	}
}

void configManager::saveFile() noexcept
{
	// getProfile current settings as json dump
	json j;
	for (auto& c : this->configs) {
		j[PROFILES_KEY_NAME][c->name] = json::parse(c->toString());
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

bool configManager::fileExists(const char* path) noexcept
{
	DWORD dwAttrib = GetFileAttributes(path);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}