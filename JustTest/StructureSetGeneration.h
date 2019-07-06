#pragma once

#include <vector>

#include "Object.h"

struct structureSetSettings {
	int turret_min = 1;
	int gold_min = 0;
	int dome_min = 1;
	int factory_min = 0;
	int struct_sum_min = 3;
	int struct_sum_max = 8;
}struct_set;


std::vector<Object *> getRandomStructureSet(Point center, float radius, structureSetSettings struct_set) {
	int asteroid_lvl = struct_set.struct_sum_min + rand() % (struct_set.struct_sum_max - struct_set.struct_sum_min + 1);

	float add_str = asteroid_lvl - struct_set.dome_min - struct_set.factory_min - struct_set.gold_min - struct_set.turret_min;  // additional structures
	float turret_rand = rand(), gold_rand = rand(), dome_rand = rand(), factory_rand = rand();
	float sum_rand = turret_rand + gold_rand + dome_rand + factory_rand;

	int gold = struct_set.gold_min + round(add_str * (gold_rand / sum_rand));
	int dome = struct_set.dome_min + round(add_str * (dome_rand / sum_rand));
	int factory = struct_set.factory_min + round(add_str * (factory_rand / sum_rand));
	int turret = struct_set.turret_min + std::max(0, (int)add_str - gold - dome - factory);

	std::vector<Object *> output(asteroid_lvl);
}