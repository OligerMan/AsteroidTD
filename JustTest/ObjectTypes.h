#pragma once

#include <vector>
#include <string>

enum CollisionType {
	null_col,
	stone_col,
	unit_col,
	asteroid_col,
	redactor_back_col,
	turret_col,
	dome_col,
	science_col,
	gold_col,
	bullet_col,
	alien_fighter_col,
	alien_turret1_col,

	COUNT_COLLISION
};

std::vector<std::string> collision_type;


void collision_type_init() {
	collision_type.resize(COUNT_COLLISION);

	collision_type[null_col] = "null";
	collision_type[stone_col] = "stone";
	collision_type[unit_col] = "unit";
	collision_type[asteroid_col] = "asteroid";
	collision_type[redactor_back_col] = "redactor_back";
	collision_type[turret_col] = "turret";
	collision_type[dome_col] = "dome";
	collision_type[science_col] = "science";
	collision_type[gold_col] = "gold"; 
	collision_type[bullet_col] = "bullet";
	collision_type[alien_fighter_col] = "alien_fighter";
	collision_type[alien_turret1_col] = "alien_turret1";
}

int is_collision_type_exists(std::string example) {
	for (int i = 0; i < COUNT_COLLISION; i++) {
		if (collision_type[i] == example) {
			return i;
		}
	}
	return -1;
}

enum ObjectType {
	null,
	stone,
	hero,
	asteroid,
	redactor_back,
	turret,
	dome,
	science,
	gold,
	bullet,
	alien_fighter,
	alien_turret1,

	COUNT_OBJECT
};

std::vector<std::string> object_type;

void object_type_init() {
	object_type.resize(COUNT_OBJECT);

	object_type[null] = "null";
	object_type[stone] = "stone";
	object_type[hero] = "hero";
	object_type[asteroid] = "asteroid";
	object_type[redactor_back] = "redactor_back";
	object_type[turret] = "turret";
	object_type[dome] = "dome";
	object_type[science] = "science";
	object_type[gold] = "gold"; 
	object_type[bullet] = "bullet";
	object_type[alien_fighter] = "alien_fighter";
	object_type[alien_turret1] = "alien_turret1";

}

int is_object_type_exists(std::string example) {
	for (int i = 0; i < COUNT_OBJECT; i++) {
		if (object_type[i] == example) {
			return i;
		}
	}
	return -1;
}

std::vector<int> lifetime{
	-1, // null
	-1, // stone
	-1, // hero
	-1, // asteroid
	-1, // redactor_back
	-1, // turret
	-1, // dome
	-1, // science
	-1, // gold
	480, // bullet
	-1, // alien_fighter
	-1, // alien_turret1
};