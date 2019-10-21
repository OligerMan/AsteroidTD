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
	int min_asteroid_distance = 200;
	int max_asteroid_distance = 500;
	int asteroid_amount = 100;
	bool is_infinite = false;
	int infinite_map_radius = 2000;
	
	// service variable
	bool is_settings_loaded = false;

	// balance settings
	int start_gold = 1500;

public:

	void setDefaults() {
		window_height = 720;
		window_width = 1200;
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
				if (setting == "start_gold") {
					start_gold = std::stoi(value);
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
};

Settings settings;
