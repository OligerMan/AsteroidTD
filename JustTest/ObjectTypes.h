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
    bombard_bullet_col,
	alien_fighter_col,
	alien_turret1_col,
	alien_gunship_col,
	alien_turret2_col,
    alien_bombard_col,
    alien_turret3_col,
	drone_col,
	drone_turret_col,
	rocket_col,

	COUNT_COLLISION
};

std::vector<std::string> collision_type;


void collision_type_init() {
	collision_type.resize(COUNT_COLLISION);

	collision_type[null_col] = "null";
	collision_type[stone_col] = "stone";
	collision_type[unit_col] = "hero";
	collision_type[asteroid_col] = "asteroid";
	collision_type[redactor_back_col] = "redactor_back";
	collision_type[turret_col] = "turret";
	collision_type[dome_col] = "dome";
	collision_type[science_col] = "science";
	collision_type[gold_col] = "gold";
    collision_type[bullet_col] = "bullet";
    collision_type[bombard_bullet_col] = "bombard_bullet";
	collision_type[alien_fighter_col] = "alien_fighter";
	collision_type[alien_turret1_col] = "alien_turret1";
	collision_type[alien_gunship_col] = "alien_gunship";
	collision_type[alien_turret2_col] = "alien_turret2";
    collision_type[alien_bombard_col] = "alien_bombard";
    collision_type[alien_turret3_col] = "alien_turret3";
	collision_type[drone_col] = "drone";
	collision_type[drone_turret_col] = "drone_turret";
	collision_type[rocket_col] = "rocket";
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
    bombard_bullet,
	alien_fighter,
	alien_turret1,
	alien_gunship,
	alien_turret2,
    alien_bombard,
    alien_turret3,
	drone,
	drone_turret,
	rocket_launcher,

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
    object_type[bombard_bullet] = "bombard_bullet";
	object_type[alien_fighter] = "alien_fighter";
	object_type[alien_turret1] = "alien_turret1";
	object_type[alien_gunship] = "alien_gunship";
	object_type[alien_turret2] = "alien_turret2";
    object_type[alien_bombard] = "alien_bombard";
    object_type[alien_turret3] = "alien_turret3";
	object_type[drone] = "drone";
	object_type[drone_turret] = "drone_turret";
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
    2400, // bullet
    3000, // bombard_bullet
	-1, // alien_fighter
	-1, // alien_turret1
	-1, // alien_gunship1
	-1, // alien_turret2
    -1, // alien_bombard
    -1, // alien_turret3
	-1, // drone
	-1, // drone_turret
};