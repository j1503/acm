#include "config.hpp"

#include <fstream>
#include <sstream>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"

/*
Add a new cheat:
1. implement abstract class (hpp+cpp file, inherit from base)
2. add config 
3. add to cheatmanager
4. add run() to main/hk
(5. add to gui) 
*/

// custom to_json functions
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(config_manager::config::menu_conf, active, menuKey, ejectKey);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(config_manager::config::misc_conf, supersecret);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(config_manager::config::guns_conf, active, reload, rapidfire, nospread, infammo, recoil, kickback, fullauto); // ignore active but needs to be there for polymorphism to work
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(config_manager::config::esp_conf, active, teammates, healthbar, enemyColor, allyColor);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(config_manager::config::aimbot_conf, active, friendlyfire, showcircle, circlecolor, autoshoot, targetLock, smoothing,
	range, onkey, hotkey, rangevalue, smoothvalue, fov);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(config_manager::config::triggerbot_conf, active, friendlyfire);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(config_manager::config::snaplines_conf, active, enemyColor, allyColor, linewidth, teammates);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(config_manager::config::barrelesp_conf, active, enemyColor, allyColor, linewidth, linelength, teammates);

using config = config_manager::config;

config_manager::config::config(const config_manager& parent)
	: parent(parent) {}

config_manager::config::config(const config_manager& parent, std::string name)
	: parent(parent), name(name) {}

void config_manager::config::load(json& j)
{
	this->name = j["name"];
	//this->general = j["general"];
	this->misc = j["misc"];
	this->guns = j["guns"];
	this->esp = j["esp"];
	this->aimbot = j["aimbot"];
	this->triggerbot = j["triggerbot"];
	this->snaplines = j["snaplines"];
	this->barrelesp = j["barrelesp"];
}

void config_manager::config::save() const
{
	this->parent.save();
}

void config_manager::config::reset()
{
	this->general = menu_conf{};
	this->misc = misc_conf{};
	this->guns = guns_conf{};
	this->esp = esp_conf{};
	this->aimbot = aimbot_conf{};
	this->triggerbot = triggerbot_conf{};
	this->snaplines = snaplines_conf{};
	this->barrelesp = barrelesp_conf{};
}

nlohmann::json config_manager::config::get() const
{
	json j;
	j["name"] = this->name;
	//j["general"] = this->general;
	j["misc"] = this->misc;
	j["guns"] = this->guns;
	j["esp"] = this->esp;
	j["aimbot"] = this->aimbot;
	j["triggerbot"] = this->triggerbot;
	j["snaplines"] = this->snaplines;
	j["barrelesp"] = this->barrelesp;
	return j;
}

std::string config_manager::config::dump() const
{
	return this->get().dump(4, ' ', true);
}

config_manager::config_manager()
{
	this->path = config_manager::getProgramDataPath();
	this->path /= config_manager::foldername;
	this->path /= config_manager::filename;

	if (std::filesystem::exists(this->path)) {
		// already exists 
		this->load();
	}
	else {
		// first time running
		std::filesystem::create_directory(this->path.parent_path()); // create dir
		this->reset();
	}
}

config_manager::~config_manager()
{
	this->save();
}

void config_manager::load()
{
	std::ifstream conf(this->path);
	if (conf.eof()) return this->reset(); // empty file
	
	std::stringstream data;
	data << conf.rdbuf();
	
	try {
		json j = json::parse(data);

		this->activeidx = j["activeidx"].get<size_t>();

		for (auto& p : j["profiles"]) {
			this->profiles.emplace_back(*this).load(p);
			/* equal code :
			auto profile = config(*this);
			profile.load(p);
			this->profiles.push_back(profile);
			*/
		}
	}
	catch (...) { // reset on any error
		this->reset();
	}

	conf.close(); // not required, but feels right (close is called by destructor)
}

void config_manager::save() const
{
	std::ofstream conf(this->path);
	conf << this->dump();
	conf.close();
}

void config_manager::create(std::string name) noexcept
{
	this->profiles.emplace_back(*this, name);
}

config& config_manager::get(size_t idx)
{
	return this->profiles.at(idx);
}

const config& config_manager::get(size_t idx) const
{
	return this->profiles.at(idx);
}

config& config_manager::active()
{
	return this->profiles.at(this->activeidx);
}

const config& config_manager::active() const
{
	return this->profiles.at(this->activeidx);
}

void config_manager::setActive(size_t idx)
{
	auto boundsCheck = this->profiles.at(idx); // will throw if out of range
	this->activeidx = idx;
}

bool config_manager::remove()
{
	return false;
}

void config_manager::reset() 
{
	this->profiles.clear(); // clear current profiles

	// reset to default profile
	this->activeidx = 0;
	this->profiles.emplace_back(*this, "Default");
}

std::string config_manager::dump() const
{
	json j;
	
	// store active idx
	j["activeidx"] = this->activeidx;

	// store profiles as array
	for (size_t i = 0; i < this->profiles.size(); ++i)
		j["profiles"][i] = this->profiles[i].get();

	return j.dump(4, ' ', false);

}

std::string config_manager::getProgramDataPath()
{
	char buf[MAX_PATH+1];
	SHGetFolderPath(0, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, 0, 0, buf); 
	return buf;
}

