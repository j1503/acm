#include "config.hpp"
#include "hookmanager.hpp"
#include "json.hpp"
#include <Windows.h>
#include <string>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"

#define CHEAT_NAME "ACM"

using json = nlohmann::json;
using namespace std::string_literals;

// custom to_json functions
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configManager::general_conf, menuShown, menuKey, ejectKey);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configManager::esp_conf, active, enemyColor, allyColor);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configManager::aimbot_conf, active);

configManager::configManager()
{
	SHGetFolderPath(0, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, 0, 0, this->path);
	PathAppend(this->path, CHEAT_NAME);
	CreateDirectory(this->path, 0);
	PathAppend(this->path, "config.json");
	if (!configManager::fileExists(this->path)) {
		this->hFile = CreateFile(this->path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		this->save(); // fills default values
	}
	else {
		this->hFile = CreateFile(this->path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		this->load();
	}
}

configManager::~configManager()
{
	globals::Config->save();
	CloseHandle(this->hFile);
}

void configManager::load() noexcept
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
		j = json::parse(buf.get());
		this->general = j["general"];
		this->esp = j["esp"];
		this->aimbot = j["aimbot"];
	}
	catch (...) { /*json::parse_error& e*/
		std::string errorString = "Your configuration had to be reset due to a corruption in the config file at "s + std::string(this->path);
		MessageBox(globals::HookManager->hWnd, errorString.c_str(), "Information", MB_OK | MB_ICONEXCLAMATION);
		this->reset();
		this->save();
	}
}

void configManager::save() noexcept
{
	// get current settings as json dump
	json j;
	j["general"] = this->general;
	j["esp"] = this->esp;
	j["aimbot"] = this->aimbot;
	std::string x = j.dump(4, ' ', true);
	const char* buf = x.c_str();
	// write to file
	this->clearFile();
	WriteFile(this->hFile, buf, x.length(), 0, 0);
}

void configManager::reset() noexcept
{
	this->general = general_conf{};
	this->esp = esp_conf{};
	this->aimbot = aimbot_conf{};
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