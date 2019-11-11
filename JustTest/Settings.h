#pragma once

#include <fstream>
#include <string>


class Settings {
	// window settings
	int window_height;
	int window_width;

	// runtime mode settings
	bool sprite_loader_debug_output_enabled = false;
	bool console_enabled = false;
	bool error_output_enabled = false;
	bool gamepad_debug_output = false;
	bool collision_debug_mode = false;
	bool navigation_debug_mode = false;
	bool unit_info_debug = false;

	// map generation settings
	int generation_key = 42;
	int min_asteroid_distance;
	int max_asteroid_distance;
	int asteroid_amount;
	bool is_infinite;
	int infinite_map_radius;
	
	// service variable
	bool is_settings_loaded;

	// balance settings
	int start_gold = 1500;

	// online ranking settings
	std::string rank_server;

	// user settings
	std::string nickname;

public:

	void setDefaults() {
		window_height = 720;
		window_width = 1200;
	}

	void saveSettings() {
		std::ofstream settings_input;
		settings_input.open("config.cfg");

		settings_input << "error_output " << error_output_enabled << std::endl;
		settings_input << "sprt_debug " << sprite_loader_debug_output_enabled << std::endl;
		settings_input << "window_height " << window_height << std::endl;
		settings_input << "window_width " << window_width << std::endl;
		settings_input << "collision_debug " << collision_debug_mode << std::endl;
		settings_input << "gamepad_debug " << gamepad_debug_output << std::endl;
		settings_input << "navigation_debug " << navigation_debug_mode << std::endl;
		settings_input << "generation_key " << generation_key << std::endl;
		settings_input << "min_asteroid_distance " << min_asteroid_distance << std::endl;
		settings_input << "max_asteroid_distance " << max_asteroid_distance << std::endl;
		settings_input << "asteroid_amount " << asteroid_amount << std::endl;
		settings_input << "is_infinite " << is_infinite << std::endl;
		settings_input << "infinite_map_radius " << infinite_map_radius << std::endl;
		settings_input << "ranking_server " << rank_server << std::endl;
		settings_input << "start_gold " << start_gold << std::endl;
		settings_input << "nickname " << nickname << std::endl;
		settings_input << "config_end" << std::endl;
	}

	Settings() {

		setDefaults();

		std::ifstream settings_input;
		settings_input.open("config.cfg");

		if (settings_input.is_open()) {
			std::string setting, value;
			is_settings_loaded = true;
			while (true) {
				settings_input >> setting;
				settings_input >> value;
				if (setting == "") {
					break;
				}
				if (setting == "error_output") {
					error_output_enabled = std::stoi(value);
				}
				if (setting == "sprt_debug") {
					sprite_loader_debug_output_enabled = std::stoi(value);
				}
				if (setting == "window_height") {
					window_height = std::stoi(value);
				}
				if (setting == "window_width") {
					window_width = std::stoi(value);
				}
				if (setting == "collision_debug") {
					collision_debug_mode = std::stoi(value);
				}
				if (setting == "gamepad_debug") {
					gamepad_debug_output = std::stoi(value);
				}
				if (setting == "navigation_debug") {
					navigation_debug_mode = std::stoi(value);
				}
				if (setting == "generation_key") {
					generation_key = std::stoi(value);
				}
				if (setting == "min_asteroid_distance") {
					min_asteroid_distance = std::stoi(value);
				}
				if (setting == "max_asteroid_distance") {
					max_asteroid_distance = std::stoi(value);
				}
				if (setting == "asteroid_amount") {
					asteroid_amount = std::stoi(value);
				}
				if (setting == "is_infinite") {
					is_infinite = std::stoi(value);
				}
				if (setting == "infinite_map_radius") {
					infinite_map_radius = std::stoi(value);
				}
				if (setting == "ranking_server") {
					rank_server = value;
				}
				if (setting == "nickname") {
					nickname = value;
				}
				if (setting == "start_gold") {
					//start_gold = std::stoi(value);
				}
				if (setting == "config_end") {
					is_settings_loaded = true;
					break;
				}
			}
		}
		else {
			is_settings_loaded = false;
		}
		
		settings_input.close();
	}

	~Settings() {
		saveSettings();
	}

	bool isLoaded() {
		return is_settings_loaded;
	}

	int getWindowHeight() {
		return window_height;
	}

	int getWindowWidth() {
		return window_width;
	}

	bool isSpriteDebugOutputEnabled() {
		return sprite_loader_debug_output_enabled;
	}

	bool isErrorOutputEnabled() {
		return error_output_enabled;
	}

	bool isGamepadDebugEnabled() {
		return gamepad_debug_output;
	}

	bool isCollisionDebugMode() {
		return collision_debug_mode;
	}

	bool isNavigationGridMode() {
		return navigation_debug_mode;
	}

	void switchNavigationGridMode() {
		navigation_debug_mode = navigation_debug_mode ^ true;
	}

	bool isUnitInfoDebugEnabled() {
		return unit_info_debug;
	}

	int getStartGold() {
		return start_gold;
	}

	std::string getNickname() {
		return nickname;
	}

	std::string getRankingServer() {
		return rank_server;
	}
};

Settings settings;
