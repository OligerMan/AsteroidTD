#pragma once

#include <fstream>
#include <vector>

#include "Object.h"

/*
I'm planning to create a GUI from file and
creating file from GUI on this template:

-- Settings block start --
settings_start
    - Game mode(such as strategy, shooter, RPG, cut scene
    and so on because of it change settings for GUI
    and another elements)                                  // not necessary
    - Weather type                                           // not necessary
    - layers_amount <max number of layer>
settings_end
-- Settings block end --
-- Objects block start --
object_start
- Objects must be described by such template:
object <object name>
    - layer <object's layer number, can't be more than layers_amount>
    - position <coor.x> <coor.y>
    - origin_mode <auto or custom>
    - origin <not considered if origin mode is auto
    but there in all situations must be 2 numbers>
    - object_type <one of strings, defined in ObjectTypes.h>
    - collision_type <one of strings, defined in ObjectTypes.h>
    - sprite_type <one of strings, defined in VisualInfo.h>
    - animation_type <one of strings, defined in VisualInfo.h>
object_end
-- Objects block end
*/

bool gui_parser(std::string path, std::vector<std::vector<Object *>> * output) {
	std::ifstream gui_file(path);

	if (!gui_file) {
		return false;
	}

	int layers_amount = 0;

	std::string input;
	gui_file >> input;
	if (input == "settings_start") {
		while (input != "settings_end") {
			gui_file >> input;
			if (input == "") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Settings block is crashed in map " << path << std::endl;
				}
				return false;
			}
			if (input == "layers_amount") {
				gui_file >> input;
				layers_amount = std::stoi(input);
			}
		}
	}
	else {
		if (settings.isErrorOutputEnabled()) {
			std::cout << "Settings block is missing in map " << path << std::endl;
		}
	}
	// settings block end

	output->resize(layers_amount);

	// object block parsing
	gui_file >> input;
	if (input == "object_start") {
		while (input != "object_end") {
			gui_file >> input;
			if (input == "object_end") {
				break;
			}
			if (input == "") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Objects block is crashed in map " << path << std::endl;
				}
				return &output;
			}
			if (input == "object") {
				std::string object_name;
				int layer;
				Point
					position,
					origin;
				std::string
					origin_mode,
					object_type,
					collision_type,
					sprite_type,
					animation_type;


				gui_file >> object_name;
				gui_file >> input;
				if (input == "layer") {
					gui_file >> layer;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- position block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				gui_file >> input;
				if (input == "position") {
					gui_file >> position.x >> position.y;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- position block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				gui_file >> input;
				if (input == "origin_mode") {
					gui_file >> origin_mode;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- origin_mode block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				gui_file >> input;
				if (input == "origin") {
					gui_file >> origin.x >> origin.y;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- origin block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				gui_file >> input;
				if (input == "object_type") {
					gui_file >> object_type;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- object_type block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				gui_file >> input;
				if (input == "collision_type") {
					gui_file >> collision_type;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- collision_type block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				gui_file >> input;
				if (input == "sprite_type") {
					gui_file >> sprite_type;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- sprite_type block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				gui_file >> input;
				if (input == "animation_type") {
					gui_file >> animation_type;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- animation_type block is missing in object " << object_name << std::endl;
						continue;
					}
				}

				int
					object_type_index = is_object_type_exists(object_type),
					collision_type_index = is_collision_type_exists(collision_type),
					sprite_type_index = is_sprite_type_exists(sprite_type),
					animation_type_index = is_animation_type_exists(animation_type);

				if (object_type_index == -1 || collision_type_index == -1 || sprite_type_index == -1 || animation_type_index == -1) {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block's values is crashed in map " << path << " in object " << object_name << std::endl;
						continue;
					}
				}

				Object * object = new Object(
					position,
					origin,
					(ObjectType)object_type_index,
					(CollisionType)collision_type_index,
					VisualInfo(
					(SpriteType)sprite_type_index,
						(AnimationType)animation_type_index,
						1
					)
				);

				if (origin_mode == "auto") {
					object->setAutoOrigin();
				}

				if (layer < layers_amount && layer >= 0) {
					(*output)[layer].push_back(object);
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Layer number is invalid in map " << path << " in object " << object_name << std::endl;
						continue;
					}
				}
			}
		}
	}
	else {
		if (settings.isErrorOutputEnabled()) {
			std::cout << "Objects block is missing in map " << path << std::endl;
			return false;
		}
	}

	return true;
}