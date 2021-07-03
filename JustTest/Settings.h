#pragma once

#include <fstream>
#include <string>


class Settings {

public:
	struct Setting {
		std::string name;
		std::string value;
		int value_i;

		void setValue(std::string value) {
			this->value = value;
			try {
				value_i = stoi(value);
			}
			catch (std::invalid_argument exc) {
				value_i = 0;
			}
            catch (std::out_of_range exc) {
                value_i = 0;
            }
		}

		void setValue(int value) {
			this->value = std::to_string(value);
			this->value_i = value;
		}

		Setting(std::string name, int value) {
			this->name = name;
			setValue(value);
		}

		Setting(std::string name, std::string value) {
			this->name = name;
			setValue(value);
		}

		void operator=(int value) {
			this->value = std::to_string(value);
		}

		void operator=(std::string value) {
			this->value = value;
		}

	};

	enum settings_name {
		window_height,
		window_width,
        volume,
		sprt_debug,
		error_output,
		gamepad_debug,
		collision_debug,
		navigation_debug,
		unit_info_debug,
		generation_key,
		min_asteroid_distance,
		max_asteroid_distance,
		asteroid_amount,
		is_infinite,
		infinite_map_radius,
		ranking_server,
		start_gold,
		nickname,
        localisation_file
	};

private:
	std::vector<Setting> set = {
		Setting("window_height", 720),
        Setting("window_width", 1200),
        Setting("volume", 30),
		Setting("sprt_debug", 0),
		Setting("error_output", 0),
		Setting("gamepad_debug", 0),
		Setting("collision_debug", 0),
		Setting("navigation_debug", 0),
		Setting("unit_info_debug", 0),
		Setting("generation_key", 42),
		Setting("min_asteroid_distance", 200),
		Setting("max_asteroid_distance", 500),
		Setting("asteroid_amount", 50),
		Setting("is_infinite", 0),
		Setting("infinite_map_radius", 5000),
		Setting("ranking_server", "oliger.ddns.net"),
		Setting("start_gold", 25000),
		Setting("nickname", ""),
        Setting("localisation_file", "dialog.cfg"),
	};

	bool is_settings_loaded;

public:

	void setDefaults() {
		set[window_height] = 720;
		set[window_width] = 1200;
	}

	void saveSettings() {
		std::ofstream settings_input;
		settings_input.open("config.cfg");

		settings_input << "error_output " << set[error_output].value << std::endl;
		settings_input << "sprt_debug " << set[sprt_debug].value << std::endl;
		settings_input << "window_height " << set[window_height].value << std::endl;
        settings_input << "window_width " << set[window_width].value << std::endl;
        settings_input << "volume " << set[volume].value << std::endl;
		settings_input << "collision_debug " << set[collision_debug].value << std::endl;
		settings_input << "gamepad_debug " << set[gamepad_debug].value << std::endl;
		settings_input << "navigation_debug " << set[navigation_debug].value << std::endl;
		settings_input << "generation_key " << set[generation_key].value << std::endl;
		settings_input << "min_asteroid_distance " << set[min_asteroid_distance].value << std::endl;
		settings_input << "max_asteroid_distance " << set[max_asteroid_distance].value << std::endl;
		settings_input << "asteroid_amount " << set[asteroid_amount].value << std::endl;
		settings_input << "is_infinite " << set[is_infinite].value << std::endl;
		settings_input << "infinite_map_radius " << set[infinite_map_radius].value << std::endl;
		settings_input << "ranking_server " << set[ranking_server].value << std::endl;
        settings_input << "start_gold " << set[start_gold].value << std::endl;
        settings_input << "localisation_file " << set[localisation_file].value << std::endl;
		if (set[nickname].value.size()) {
			settings_input << "nickname " << set[nickname].value << std::endl;
		}
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
				if (setting != "start_gold") {
					setSetting(setting, value);
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

	int getSettingInt(std::string name) {
		for (int i = 0; i < set.size(); i++) {
			if (name == set[i].name) {
				return set[i].value_i;
			}
		}
		return 0;
	}

	std::string getSettingString(std::string name) {
		for (int i = 0; i < set.size(); i++) {
			if (name == set[i].name) {
				return set[i].value;
			}
		}
		return "";
	}

	void setSetting(std::string name, int value) {
		for (int i = 0; i < set.size(); i++) {
			if (name == set[i].name) {
				set[i].setValue(value);
				return;
			}
		}
	}

	void setSetting(std::string name, std::string value) {
		for (int i = 0; i < set.size(); i++) {
			if (name == set[i].name) {
				set[i].setValue(value);
                if (name == "volume") {
                    set[i].setValue(std::max(std::min(set[i].value_i, 100), 0));
                }
				return;
			}
		}
	}

	std::vector<std::string> getSettingsList() {
		std::vector<std::string> output;
		for (int i = 0; i < set.size(); i++) {
			output.push_back(set[i].name);
		}
	}

	std::vector<std::string> getChangeableSettingsList() {
		std::vector<std::string> output = {
			std::string("nickname"),
			std::string("ranking_server"),
			std::string("window_width"),
            std::string("window_height"),
            std::string("volume"),
            std::string("localisation_file")
		};
		return output;
	}

	bool isLoaded() {
		return is_settings_loaded;
	}

	int getWindowHeight() {
		return set[window_height].value_i;
	}

	void setWindowHeight(int new_window_height) {
		set[window_height].setValue(new_window_height);
	}

    int getWindowWidth() {
        return set[window_width].value_i;
    }

    void setWindowWidth(int new_window_width) {
        set[window_width].setValue(new_window_width);
    }

    int getVolume() {
        return set[volume].value_i;
    }

    void setVolume(int new_volume) {
        set[volume].setValue(new_volume);
    }

	bool isSpriteDebugOutputEnabled() {
		return set[sprt_debug].value_i;
	}

	bool isErrorOutputEnabled() {
		return set[error_output].value_i;
	}

	bool isGamepadDebugEnabled() {
		return set[gamepad_debug].value_i;
	}

	bool isCollisionDebugMode() {
		return set[collision_debug].value_i;
	}

	bool isNavigationGridMode() {
		return set[navigation_debug].value_i;
	}

	void switchNavigationGridMode() {
		set[navigation_debug].setValue(set[navigation_debug].value_i ^ 1);
	}

	bool isUnitInfoDebugEnabled() {
		return set[unit_info_debug].value_i;
	}

	int getStartGold() {
		return set[start_gold].value_i;
	}

	std::string getNickname() {
		return set[nickname].value;
	}

	void setNickname(std::string new_nickname) {
		set[nickname].setValue(new_nickname);
	}

	std::string getRankingServer() {
		return set[ranking_server].value;
	}

    std::string getLocalisationFile() {
        return set[localisation_file].value;
    }
};

Settings settings;