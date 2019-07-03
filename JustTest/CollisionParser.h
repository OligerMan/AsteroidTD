#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "GeometryComponents.h"
#include "Settings.h"

/*
I'm planning to create collision model from file
Template for file:

 -- Start of description -- 
 settings_start
	is_static <0 or 1>
 settings_end
 collision_start
     - collision models must be described by such template:
	     element <element name>
		     position <pos.x> <pos.y>
			 radius <radius>

 collision_end
 -- End of description --
*/

std::vector<Circle> collisionParse(std::string path, bool & is_static, bool * status) {
	std::ifstream collision_file;
	collision_file.open(path);

	std::vector<Circle> output;

	*status = true;

	if (!collision_file.is_open()) {
		if (settings.isErrorOutputEnabled()) {
			std::cout << "Collision file " << path << " not found" << std::endl;
		}
		*status = false;
		return output;
	}

	std::string input;
	collision_file >> input;
	if (input == "settings_start") {
		collision_file >> input;
		if (input == "is_static") {
			collision_file >> input;
			is_static = std::stoi(input);
		}
		else {
			if (settings.isErrorOutputEnabled()) {
				std::cout << "Collision file " << path << " is invalid" << std::endl;
			}
		}
		collision_file >> input;
		if (input == "settings_end") {

		}
		else {
			if (settings.isErrorOutputEnabled()) {
				std::cout << "Collision file " << path << " is invalid" << std::endl;
			}
		}
	}
	else {
		if (settings.isErrorOutputEnabled()) {
			std::cout << "Collision file's settings " << path << " are invalid" << std::endl;
		}
	}
	collision_file >> input;
	if (input == "collision_start") {
		while (input != "collision_end") {
			collision_file >> input;

			if (input == "") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Collision file " << path << " is invalid" << std::endl;
				}
			}
			if (input == "element") {
				std::string element_name;

				Circle circle;

				collision_file >> element_name;

				collision_file >> input;
				if (input == "position") {
					collision_file >> circle.center.x >> circle.center.y;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Position block is missing in element " << element_name << " in map " << path << " is invalid" << std::endl;
						continue;
					}
				}
				collision_file >> input;
				if (input == "radius") {
					collision_file >> circle.collision_radius;
				}
				else {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Radius block is missing in element " << element_name << " in map " << path << " is invalid" << std::endl;
						continue;
					}
				}

				output.push_back(circle);
			}
		}
	}
	else {
		if (settings.isErrorOutputEnabled()) {
			std::cout << "Collision file " << path << " is invalid" << std::endl;
		}
	}
	return output;
}