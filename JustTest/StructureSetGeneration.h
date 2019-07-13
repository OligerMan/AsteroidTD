#pragma once

#include <vector>
#include <math.h>

#include "Object.h"

struct structureSetSettings {
	int turret_min = 2;
	int gold_min = 0;
	int dome_min = 1;
	int factory_min = 0;
	int struct_sum_min = 4;
	int struct_sum_max = 7;
}struct_set;


std::vector<Object *> getRandomStructureSet(Point center, float radius, structureSetSettings struct_set, FactionList faction) {
	std::vector<Object *> output;
	switch (faction) {
	case null_faction:
		break;
	case friendly_faction:
	case neutral_faction:
	case aggressive_faction:
		int asteroid_lvl = struct_set.struct_sum_min + rand() % (struct_set.struct_sum_max - struct_set.struct_sum_min + 1);

		float add_str = asteroid_lvl - struct_set.dome_min - struct_set.factory_min - struct_set.gold_min - struct_set.turret_min;  // additional structures
		float turret_rand = rand(), gold_rand = rand(), dome_rand = rand(), science_rand = rand();
		float sum_rand = turret_rand + gold_rand + dome_rand + science_rand;


		// amount of structures
		int gold = struct_set.gold_min + round(add_str * (gold_rand / sum_rand));
		int dome = struct_set.dome_min + round(add_str * (dome_rand / sum_rand));
		int science = struct_set.factory_min + round(add_str * (science_rand / sum_rand));
		int turret = std::max(0, (int)asteroid_lvl - gold - dome - science);

		output.resize(asteroid_lvl);

		float base_angle = (float)(rand() % 1024) / 512 * PI;
		for (int i = 0; i < turret; i++) {
			float cur_angle = (float)i / turret * 2 * PI + base_angle;
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
					1000000000
				)
			);
			object->getUnitInfo()->setFaction(faction);
			object->setAutoOrigin();
			object->setAngle(cur_angle);

			output[i] = object;
		}

		base_angle = (float)(rand() % 1024) / 512 * PI;
		if (gold + dome + science == 1) {
			Object * object = new Object
			(
				center,
				Point(),
				ObjectType::dome,
				CollisionType::dome_col,
				VisualInfo
				(
					SpriteType::dome_sprite,
					AnimationType::hold_anim,
					1000000000
				)
			);
			object->getUnitInfo()->setFaction(faction);
			object->setAutoOrigin();

			output[output.size() - 1] = object;
			return output;
		}
		for (int i = 0; i < gold; i++) {
			float cur_angle = (float)i / (gold + dome + science) * 2 * PI + base_angle;
			Point cur_pos = Point(sin(cur_angle), cos(cur_angle)) * (radius - 210) + center;

			Object * object = new Object
			(
				cur_pos,
				Point(),
				ObjectType::gold,
				CollisionType::gold_col,
				VisualInfo
				(
					SpriteType::gold_sprite,
					AnimationType::hold_anim,
					15
				)
			);
			object->getUnitInfo()->setFaction(faction);
			object->setAutoOrigin();
			object->setAngle(0);

			output[turret + i] = object;
		}
		for (int i = 0; i < dome; i++) {
			float cur_angle = (float)(gold + i) / (gold + dome + science) * 2 * PI + base_angle;
			Point cur_pos = Point(sin(cur_angle), cos(cur_angle)) * (radius - 210) + center;

			Object * object = new Object
			(
				cur_pos,
				Point(),
				ObjectType::dome,
				CollisionType::dome_col,
				VisualInfo
				(
					SpriteType::dome_sprite,
					AnimationType::hold_anim,
					1000000000
				)
			);
			object->getUnitInfo()->setFaction(faction);
			object->setAutoOrigin();
			object->setAngle(0);

			output[turret + gold + i] = object;
		}
		for (int i = 0; i < science; i++) {
			float cur_angle = (float)(gold + dome + i) / (gold + dome + science) * 2 * PI + base_angle;
			Point cur_pos = Point(sin(cur_angle), cos(cur_angle)) * (radius - 210) + center;

			Object * object = new Object
			(
				cur_pos,
				Point(),
				ObjectType::science,
				CollisionType::science_col,
				VisualInfo
				(
					SpriteType::science_sprite,
					AnimationType::hold_anim,
					200
				)
			);
			object->getUnitInfo()->setFaction(faction);
			object->setAutoOrigin();
			object->setAngle(0);

			output[turret + gold + dome + i] = object;
		}
		break;
	}
	

	return output;
}