#pragma once

#include <vector>
#include <math.h>

#include "Object.h"

struct structureSetSettings {
	int turret_min = 1;
	int gold_min = 0;
	int dome_min = 1;
	int factory_min = 0;
	int struct_sum_min = 4;
	int struct_sum_max = 6;
}struct_set;


std::vector<Object *> getRandomStructureSet(Point center, float radius, structureSetSettings struct_set) {
	int asteroid_lvl = struct_set.struct_sum_min + rand() % (struct_set.struct_sum_max - struct_set.struct_sum_min + 1);

	float add_str = asteroid_lvl - struct_set.dome_min - struct_set.factory_min - struct_set.gold_min - struct_set.turret_min;  // additional structures
	float turret_rand = rand(), gold_rand = rand(), dome_rand = rand(), factory_rand = rand();
	float sum_rand = turret_rand + gold_rand + dome_rand + factory_rand;


	// amount of structures
	int gold = struct_set.gold_min + round(add_str * (gold_rand / sum_rand));
	int dome = struct_set.dome_min + round(add_str * (dome_rand / sum_rand));
	int factory = struct_set.factory_min + round(add_str * (factory_rand / sum_rand));
	int turret = std::max(0, (int)asteroid_lvl - gold - dome - factory);

	std::vector<Object *> output(asteroid_lvl);

	float base_angle = (float)(rand() % 1024) / 512 * PI;
	for (int i = 0; i < turret; i++) {
		float cur_angle = i / turret * 2 * PI + base_angle;
		Point cur_pos = Point(sin(cur_angle), cos(cur_angle)) * (radius - 70) + center;

		Object * object = new Object
		(
			cur_pos,
			Point(),
			ObjectType::turret,
			CollisionType::turret_col,
			VisualInfo
			(
				SpriteType::turret_sprite,
				AnimationType::hold_anim,
				1
			)
		);
		object->getUnitInfo()->setFaction(FactionList::agressive_faction);
		object->setAutoOrigin();
		object->setAngle(cur_angle);

		output[i] = object;
	}
	for (int i = 0; i < gold; i++) {
		while (true) {
			float cur_angle = (float)(rand() % 1024) / 512 * PI;
			Point cur_pos = Point(sin(cur_angle), cos(cur_angle)) * (radius - 35) * (float)(rand() % 1024) / 1024 + center;
			bool place_ok = true;
			for (int j = 0; j < turret + i; j++) {
				if ((cur_pos - output[j]->getPosition()).getLength() < 100) {
					place_ok = false;
				}
			}
			if (place_ok) {
				Object * object = new Object
				(
					cur_pos,
					Point(),
					ObjectType::turret,
					CollisionType::turret_col,
					VisualInfo
					(
						SpriteType::turret_sprite,
						AnimationType::hold_anim,
						1
					)
				);
				object->getUnitInfo()->setFaction(FactionList::agressive_faction);
				object->setAutoOrigin();
				object->setAngle(cur_angle);

				output[turret + i] = object;
				break;
			}
		}
	}
	for (int i = 0; i < dome; i++) {
		while (true) {
			float cur_angle = (float)(rand() % 1024) / 512 * PI;
			Point cur_pos = Point(sin(cur_angle), cos(cur_angle)) * (radius - 35) * (float)(rand() % 1024) / 1024 + center;
			bool place_ok = true;
			for (int j = 0; j < turret + gold + i; j++) {
				if ((cur_pos - output[j]->getPosition()).getLength() < 100) {
					place_ok = false;
				}
			}
			if (place_ok) {
				Object * object = new Object
				(
					cur_pos,
					Point(),
					ObjectType::turret,
					CollisionType::turret_col,
					VisualInfo
					(
						SpriteType::turret_sprite,
						AnimationType::hold_anim,
						1
					)
				);
				object->getUnitInfo()->setFaction(FactionList::agressive_faction);
				object->setAutoOrigin();
				object->setAngle(cur_angle);

				output[turret + gold + i] = object;
				break;
			}
		}
	}
	for (int i = 0; i < factory; i++) {
		while (true) {
			float cur_angle = (float)(rand() % 1024) / 512 * PI;
			Point cur_pos = Point(sin(cur_angle), cos(cur_angle)) * (radius - 35) * (float)(rand() % 1024) / 1024 + center;
			bool place_ok = true;
			for (int j = 0; j < turret + gold + dome + i; j++) {
				if ((cur_pos - output[j]->getPosition()).getLength() < 100) {
					place_ok = false;
				}
			}
			if (place_ok) {
				Object * object = new Object
				(
					cur_pos,
					Point(),
					ObjectType::turret,
					CollisionType::turret_col,
					VisualInfo
					(
						SpriteType::turret_sprite,
						AnimationType::hold_anim,
						1
					)
				);
				object->getUnitInfo()->setFaction(FactionList::agressive_faction);
				object->setAutoOrigin();
				object->setAngle(cur_angle);

				output[turret + gold + dome + i] = object;
				break;
			}
		}
	}

	return output;
}