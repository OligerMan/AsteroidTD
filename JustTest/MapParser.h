#pragma once

#include <fstream>
#include <iostream>

#include "Object.h"
#include "Settings.h"

/*
I'm planning to create a map from file and
creating file from map on this template:

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
			- frame_duration <duration of frames in animation>
			- faction <one of strings, defined in UnitInfo.h>
 object_end
 -- Objects block end
*/

std::vector<std::vector<Object *>> parseMap(std::string path) {
	std::ifstream map_file;
	map_file.open(path);

	std::vector<std::vector<Object *>> output;
	int layers_amount = 0;

	std::string input;
	map_file >> input;
	if (input == "settings_start") {
		while (input != "settings_end") {
			map_file >> input;
			if (input == "") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Settings block is crashed in map " << path << std::endl;
				}
				return output;
			}
			if (input == "layers_amount") {
				map_file >> input;
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

	output.resize(layers_amount);

	// object block parsing
	map_file >> input;
	if (input == "object_start") {
		while (input != "object_end") {
			map_file >> input;
			if (input == "object_end") {
				break;
			}
			if (input == "") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Objects block is crashed in map " << path << std::endl;
				}
				return output;
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
					animation_type,
					frame_duration_string,
					faction_string;

				map_file >> object_name;
				map_file >> input;
				if (input == "layer") {
					map_file >> layer;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- position block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				map_file >> input;
				if (input == "position") {
					map_file >> position.x >> position.y;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- position block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				map_file >> input;
				if (input == "origin_mode") {
					map_file >> origin_mode;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- origin_mode block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				map_file >> input;
				if (input == "origin") {
					map_file >> origin.x >> origin.y;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- origin block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				map_file >> input;
				if (input == "object_type") {
					map_file >> object_type;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- object_type block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				map_file >> input;
				if (input == "collision_type") {
					map_file >> collision_type;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- collision_type block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				map_file >> input;
				if (input == "sprite_type") {
					map_file >> sprite_type;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- sprite_type block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				map_file >> input;
				if (input == "animation_type") {
					map_file >> animation_type;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- animation_type block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				map_file >> input;
				if (input == "frame_duration") {
					map_file >> frame_duration_string;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- frame_duration block is missing in object " << object_name << std::endl;
						continue;
					}
				}
				map_file >> input;
				if (input == "faction") {
					map_file >> faction_string;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Objects block is crashed in map " << path << std::endl;
						std::cout << " -- faction block is missing in object " << object_name << std::endl;
						continue;
					}
				}

				int
					object_type_index = is_object_type_exists(object_type),
					collision_type_index = is_collision_type_exists(collision_type),
					sprite_type_index = is_sprite_type_exists(sprite_type),
					animation_type_index = is_animation_type_exists(animation_type),
					frame_duration = std::stoi(frame_duration_string),
					faction = is_faction_type_exists(faction_string);

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
						frame_duration
					)
				);
				object->getUnitInfo()->setFaction(faction);

				if (origin_mode == "auto") {
					object->setAutoOrigin();
				}

				if (layer < layers_amount && layer >= 0) {
					output[layer].push_back(object);
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
		}
	}

	return output;
}

void saveMap(std::string path, std::vector<std::vector<Object *>> * objects) {
	std::ofstream new_map(path);

	new_map.clear();
	new_map << "settings_start" << std::endl;
	new_map << "    layers_amount " << objects->size() << std::endl;
	new_map << "settings_end" << std::endl;
	new_map << "object_start" << std::endl;
	for (int layer = 0; layer < objects->size(); layer++) {
		for (int i = 0; i < (*objects)[layer].size(); i++) {
			Object * object = (*objects)[layer][i];
			new_map << "    object " << object_type[object->getObjectType()] << i << std::endl;
			new_map << "        layer " << layer << std::endl;
			new_map << "        position " << object->getPosition().x << " " << object->getPosition().y << std::endl;
			new_map << "        origin_mode auto" << std::endl;
			new_map << "        origin " << object->getOrigin().x << " " << object->getPosition().y << std::endl;
			new_map << "        object_type " << object_type[object->getObjectType()] << std::endl;
			new_map << "        collision_type " << collision_type[object->getObjectCollisionType()] << std::endl;
			new_map << "        sprite_type " << sprite_type[object->getObjectSpriteType()] << std::endl;
			new_map << "        animation_type " << animation_type[object->getObjectAnimationType()] << std::endl;
			new_map << "        frame_duration " << object->getFrameDuration() << std::endl;
			if (object->getUnitInfo() != nullptr) {
				new_map << "        faction " << faction_type[object->getUnitInfo()->getFaction()] << std::endl;
			}
			else {
				new_map << "        faction 0" << std::endl;
			}
			new_map << std::endl;
		}
	}
	new_map << "object_end" << std::endl;
	new_map.close();
}