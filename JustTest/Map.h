#pragma once

#include "Object.h"
#include "EventBuffer.h"
#include "MapParser.h"
#include "GameConstants.h"
#include "AStarAlgorithm.h"
#include "StructureSetGeneration.h"
#include "ConvexHull.h"
#include "FPS.h"
#include "ResourceManager.h"
#include "OnlineRank.h"

void fixCollision(Object * obj1, Object * obj2) {

	if (!obj1 || !obj2) {
		return;
	}

	if (obj1->getObjectType() == ObjectType::bullet || obj2->getObjectType() == ObjectType::bullet) {
		return;
	}
	bool force_move = obj1->getCollisionModel()->isStatic() && obj2->getCollisionModel()->isStatic();
	const double min_speed = 0.001;

	CollisionModel * col1 = obj1->getCollisionModel();
	CollisionModel * col2 = obj2->getCollisionModel();

	if (!col1 || !col2) {
		return;
	}

	Point tmp_speed1 = obj1->getSpeed();
	Point tmp_speed2 = obj2->getSpeed();

	Point speed1 = tmp_speed1, speed2 = tmp_speed2;
	Point origin1 = obj1->getOrigin(), origin2 = obj2->getOrigin();


	// set previous position and add to it half of speed

	tmp_speed1 /= 2;
	tmp_speed2 /= 2;
	obj1->changePosition(Point() - tmp_speed1 * consts.getFPSLock() / fps.getFPS());
	obj2->changePosition(Point() - tmp_speed2 * consts.getFPSLock() / fps.getFPS());
	if (force_move) {
		obj1->forceChangePosition(Point() - tmp_speed1 * consts.getFPSLock() / fps.getFPS());
		obj2->forceChangePosition(Point() - tmp_speed2 * consts.getFPSLock() / fps.getFPS());
	}
	
	while (tmp_speed1.getLength() > min_speed || tmp_speed2.getLength() > min_speed) {
		tmp_speed1 /= 2;
		tmp_speed2 /= 2;
		if (checkModelCollision(col1, col2)) {
			obj1->changePosition(Point() - tmp_speed1 * consts.getFPSLock() / fps.getFPS());
			obj2->changePosition(Point() - tmp_speed2 * consts.getFPSLock() / fps.getFPS());
			if (force_move) {
				obj1->forceChangePosition(Point() - tmp_speed1 * consts.getFPSLock() / fps.getFPS());
				obj2->forceChangePosition(Point() - tmp_speed2 * consts.getFPSLock() / fps.getFPS());
			}
		}
		else {
			obj1->changePosition(Point() + tmp_speed1 * consts.getFPSLock() / fps.getFPS());
			obj2->changePosition(Point() + tmp_speed2 * consts.getFPSLock() / fps.getFPS());
			if (force_move) {
				obj1->forceChangePosition(Point() + tmp_speed1 * consts.getFPSLock() / fps.getFPS());
				obj2->forceChangePosition(Point() + tmp_speed2 * consts.getFPSLock() / fps.getFPS());
			}
		}
	}

	Point pos1 = obj1->getPosition();
	Point pos2 = obj2->getPosition();

	Point normal_vect = (pos1 - pos2).getNormal();
	normal_vect = Point(normal_vect.y, -normal_vect.x);

	Point normal_speed1 = (normal_vect * (normal_vect.x * (speed1 - tmp_speed1).x + normal_vect.y * (speed1 - tmp_speed1).y));
	obj1->changePosition(normal_speed1 * consts.getFPSLock() / fps.getFPS());
	Point normal_speed2 = (normal_vect * (normal_vect.x * (speed2 - tmp_speed2).x + normal_vect.y * (speed2 - tmp_speed2).y));
	obj2->changePosition(normal_speed2 * consts.getFPSLock() / fps.getFPS());
	if (force_move) {
		obj1->forceChangePosition(normal_speed1 * consts.getFPSLock() / fps.getFPS());
		obj2->forceChangePosition(normal_speed2 * consts.getFPSLock() / fps.getFPS());
	}
	Point pos_diff = obj1->getPosition() - obj2->getPosition();
	if (pos_diff.getLength() < 0.00001) {    // approximately zero
		pos_diff = Point(0, -1);
	}
	if (checkModelCollision(col1, col2)) {
		obj1->getCollisionModel()->changeDelayedCollisionFix(pos_diff);
		obj2->getCollisionModel()->changeDelayedCollisionFix(Point() - pos_diff);
		if (force_move) {
			obj1->getCollisionModel()->changeDelayedCollisionForceFix(pos_diff);
			obj2->getCollisionModel()->changeDelayedCollisionForceFix(Point()-pos_diff);
		}
	}
}

class Map {
	std::vector<std::vector<Object *>> objects;                // vector of objects layers, it is defining order for render and reducing amount of collisions
	std::vector<std::string> animation_type;

	int landscape_layer = 0;
	int main_layer = 1;

	std::vector<std::vector<int>> navigation_grid;
	std::vector<std::vector<Point>> navigation_paths;
	double grid_size = 10;
	Point grid_offset;

	Object * hero_object = nullptr;
	Object * closest_asteroid = nullptr;

	EventBuffer event_buffer;

	Object * last_clicked_object = nullptr;

	Point gen_basis;
	float gen_radius = 6000, cam_radius = 2500, min_range = 1500, max_range = 2700, asteroid_speed = 0;
	int asteroid_amount = 20, max_try_count = 100;
	bool save_out_range = true, fixed_asteroids = true;

	//////////////////////////////////////////////

	void createNavigationGrid() {
		Point left_up(1e9, 1e9), right_down(-1e9, -1e9);

		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				Object * object = objects[layer][i];
				//if (object->getCollisionModel()->isStatic()) {
					if (-object->getOrigin().x + object->getPosition().x < left_up.x) {
						left_up.x = -object->getOrigin().x + object->getPosition().x;
					}
					if (-object->getOrigin().y + object->getPosition().y < left_up.y) {
						left_up.y = -object->getOrigin().y + object->getPosition().y;
					}
					if (-object->getOrigin().x + object->getSquareBorder().x + object->getPosition().x > right_down.x) {
						right_down.x = -object->getOrigin().x + object->getSquareBorder().x + object->getPosition().x;
					}
					if (-object->getOrigin().y + object->getSquareBorder().y + object->getPosition().y > right_down.y) {
						right_down.y = -object->getOrigin().y + object->getSquareBorder().y + object->getPosition().y;
					}
				//}
			}
		}

		grid_offset = left_up * 1.5;

		// init empty navigation grid
		navigation_grid.resize((right_down.x - left_up.x) / grid_size * 1.5 + 1);
		if (!navigation_grid.empty()) {
			for (int i = 0; i < navigation_grid.size(); i++) {
				navigation_grid[i].resize((right_down.y - left_up.y) / grid_size * 1.5 + 1);
			}
			for (int i = 0; i < navigation_grid.size(); i++) {
				navigation_grid[i][0] = 1;
				navigation_grid[i][(right_down.y - left_up.y) / grid_size * 1.5] = 1;
			}
			for (int i = 0; i < int((right_down.y - left_up.y) / grid_size * 1.5 + 1); i++) {
				navigation_grid[0][i] = 1;
				navigation_grid[navigation_grid.size() - 1][i] = 1;
			}
		}
		// grid initialisation end

		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				Object * object = objects[layer][i];

				if (object->getCollisionModel()->isStatic()) {
					double grid_width = object->getCollisionModel()->getSquareBorder().x / grid_size * 1.5 + 1;
					double grid_height = object->getCollisionModel()->getSquareBorder().y / grid_size * 1.5 + 1;
					
					Point start_object_grid = object->getPosition() - grid_offset - (object->getOrigin() * 1.5);
					for (double i = 0; i < grid_width; i++) {
						for (double j = 0; j < grid_height; j++) {
							Point cur_point = start_object_grid + Point(i * grid_size, j * grid_size);
							Object * grid_elem = new Object();
							grid_elem->getCollisionModel()->addCircle(Point(), grid_size * 3);
							grid_elem->setPosition(cur_point + grid_offset);
							if (checkObjectCollision(grid_elem, object) && navigation_grid[cur_point.x / grid_size][cur_point.y / grid_size] != 1) {
								navigation_grid[cur_point.x / grid_size][cur_point.y / grid_size] = 1;
							}
							delete grid_elem;
						}
					}
				}
			}
		}
	}

	void processCollisionFrame(){
		for (int cnt = 0; cnt < objects.size(); cnt++) {
			for (int i = 0; i < objects[cnt].size(); i++) {
				if (objects[cnt][i]->getCollisionModel()->isStatic()) {
					continue;
				}
				if (objects[cnt][i] != hero_object) {
					if (checkObjectCollision(hero_object, objects[cnt][i])) {
						event_buffer.addEvent(EventType::default_collision, objects[cnt][i], hero_object);
					}
				}
				if (objects[cnt][i]->getObjectType() != ObjectType::bullet) {
					for (int j = 0; j < objects[cnt].size(); j++) {
						if (i != j) {
							if (checkObjectCollision(objects[cnt][i], objects[cnt][j])) {
								event_buffer.addEvent(EventType::default_collision, objects[cnt][i], objects[cnt][j]);
								fixCollision(objects[cnt][i], objects[cnt][j]);
							}
						}
					}
				}
				else {
					for (int j = 0; j < objects[landscape_layer].size(); j++) {
						if (objects[landscape_layer][j]->getObjectType() != asteroid) {
							if ((objects[landscape_layer][j]->getUnitInfo()->getFaction() != objects[cnt][i]->getUnitInfo()->getFaction()) && checkObjectCollision(objects[cnt][i], objects[landscape_layer][j])) {
								event_buffer.addEvent(EventType::default_collision, objects[cnt][i], objects[landscape_layer][j]);
							}
						}
					}
				}
			}
		}

		for (int cnt = 0; cnt < objects.size(); cnt++) {
			for (int i = 0; i < objects[cnt].size(); i++) {
				if (objects[cnt][i]->getCollisionModel()->isStatic()) {
					continue;
				}

				const int collision_fix_speed = 3;
				Point 
					force_fix = objects[cnt][i]->getCollisionModel()->getDelayedCollisionForceFix(), 
					fix = objects[cnt][i]->getCollisionModel()->getDelayedCollisionForceFix();

				objects[cnt][i]->forceChangePosition(force_fix.getNormal() * collision_fix_speed * consts.getFPSLock() / fps.getFPS());
				objects[cnt][i]->changePosition(fix * collision_fix_speed * consts.getFPSLock() / fps.getFPS());
			}
		}
	}

	Point convertToNavigation(Point input) {
		Point output = (input - grid_offset) / grid_size;
		output.x = int(output.x);
		output.y = int(output.y);

		return output;
	}

	Point convertFromNavigation(Point input) {
		return input * grid_size + grid_offset;
	}

	void processUnitAI() {
		for (int layer1 = 0; layer1 < objects.size(); layer1++) {

			for (int i = 0; i < objects[layer1].size(); i++) {
				Object * object1 = objects[layer1][i];
				if (object1->getUnitInfo() == nullptr) {
					continue;
				}

				if (object1->getUnitInfo() != nullptr) {
					object1->getUnitInfo()->processCooldown();
					object1->getUnitInfo()->processEffects();
				}

				int faction1 = object1->getUnitInfo()->getFaction();


				// turn to closest enemy
				if (faction1 != FactionList::null_faction) {
					for (int layer2 = 0; layer2 < objects.size(); layer2++) {
						for (int j = 0; j < objects[layer2].size(); j++) {
							if (i != j || layer1 != layer2) {
								Object * object2 = objects[layer2][j];
								if (object2->getUnitInfo() == nullptr) {
									continue;
								}

								int faction2 = object2->getUnitInfo()->getFaction();

								if (areEnemies((FactionList)faction1, (FactionList)faction2) && object1->getObjectType() != bullet && object2->getObjectType() != bullet) {

									
									if (!(object1->getObjectType() == ObjectType::hero) && (object1->canObjectAttack() || object1->getUnitInfo()->getDefaultSpeed() > 0.0001)) {
										
										if (object2->getObjectType() == asteroid) {
											continue;
										}
										Point vect = (object2->getPosition() - object1->getPosition()).getNormal();
										Object * prev_enemy = (Object *)object1->getUnitInfo()->getEnemy();
										if (prev_enemy != nullptr) {
											if ((prev_enemy->getPosition() - object1->getPosition()).getLength() > (object2->getPosition() - object1->getPosition()).getLength()) {
												object1->getUnitInfo()->setEnemy((void *)object2);
											}
											else {
												vect = (prev_enemy->getPosition() - object1->getPosition()).getNormal();
											}
											if ((object1->getPosition() - prev_enemy->getPosition()).getLength() > object1->getUnitInfo()->getAngerRange() * 
												((object1->getObjectType() == turret && object1->getUnitInfo()->getFaction() == hero_faction) ? research_manager.getTurretAttackRangeCoef() : 1)) {
												
												object1->getUnitInfo()->setEnemy(nullptr);
											}
											else {
												double angle_diff = -(std::atan2(vect.x, vect.y) * 180.0 / PI + object1->getAngle() + 180);
												if (abs(angle_diff) > 0.000001) {     // angle_diff is not close to zero
													if (abs(angle_diff) > abs(angle_diff + 360.0)) {
														angle_diff += 360.0;
													}
													if (abs(angle_diff) > abs(angle_diff - 360.0)) {
														angle_diff -= 360.0;
													}
													object1->changeAngle(angle_diff / 10.0 * consts.getFPSLock() / fps.getFPS());
												}
												if (!object1->getCollisionModel()->isStatic() && (((Object *)object1->getUnitInfo()->getEnemy())->getPosition() - object1->getPosition()).getLength() > consts.getMinimalFlightRange()) {
													Point speed(-cos(object1->getAngle() / 180 * PI + PI / 2), -sin(object1->getAngle() / 180 * PI + PI / 2));
													object1->setSpeed(speed.getNormal()*object1->getUnitInfo()->getDefaultSpeed());
												}
												else if((((Object *)object1->getUnitInfo()->getEnemy())->getPosition() - object1->getPosition()).getLength() < consts.getMinimalFlightRange()){
													object1->setSpeed(Point());
												}
											}
										}
										else if((object1->getPosition() - object2->getPosition()).getLength() <= object1->getUnitInfo()->getAngerRange() * 
											((object1->getObjectType() == turret && object1->getUnitInfo()->getFaction() == hero_faction) ? research_manager.getTurretAttackRangeCoef() : 1)){
											
											object1->getUnitInfo()->setEnemy((void *)object2);
										}
									}
								}
							}
						}
					}
				}

				// shoot if enemy is on your way and in enough range
				if (object1->getUnitInfo()->getEnemy() != nullptr && object1->canObjectAttack()) {
					Object * enemy = (Object *)object1->getUnitInfo()->getEnemy();
					Point vect = object1->getPosition() - enemy->getPosition();
					float angle_diff = abs((object1->getAngle() + 90) / 180 * PI - (-atan2(vect.x, vect.y) + PI / 2));
					if (angle_diff > 0.001) {
						if (abs(angle_diff) > abs(angle_diff + PI * 2)) {
							angle_diff += PI * 2;
						}
						if (abs(angle_diff) > abs(angle_diff - PI * 2)) {
							angle_diff -= PI * 2;
						}
					}
					if (abs(angle_diff) < 0.05) {
						if (object1->getUnitInfo()->attackReady(1)) {
							Point bullet_pos = object1->getPosition() + Point(cos((object1->getAngle() - 90) / 180 * PI), sin((object1->getAngle() - 90) / 180 * PI)) * 95;
							Object * object = new Object
							(
								bullet_pos,
								Point(),
								ObjectType::bullet,
								CollisionType::bullet_col,
								VisualInfo
								(
									SpriteType::bullet_sprite,
									AnimationType::hold_anim,
									1000000000
								)
							);
							object->getUnitInfo()->setFaction(object1->getUnitInfo()->getFaction());
							object->getUnitInfo()->setEnemy(object1);     // to remember who is shooting
							object->setAutoOrigin();
							object->setAngle(object1->getAngle());
							object->setSpeed(Point(cos((object1->getAngle() - 90) / 180 * PI), sin((object1->getAngle() - 90) / 180 * PI)) * 15);
							
							addObject(object, main_layer);

							object1->attachObject(object);
						}
						
					}
				}
			}
		}
	}

	void garbageCollector() {
		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				objects[layer][i]->garbageCollector();
				Object * enemy = (Object *)objects[layer][i]->getUnitInfo()->getEnemy();
				if (enemy != nullptr && (enemy->isDeleted() || !(enemy->getUnitInfo() != nullptr && !enemy->getUnitInfo()->isDead()))) {
					objects[layer][i]->getUnitInfo()->setEnemy(nullptr);
				}
			}
		}
		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				if (objects[layer][i]->isDeleted() || !(objects[layer][i]->getUnitInfo() != nullptr && !objects[layer][i]->getUnitInfo()->isDead())) {
					if (objects[layer][i] == hero_object) {
						hero_object = nullptr;
					}
					if (objects[layer][i] == closest_asteroid) {
						closest_asteroid = nullptr;
					}
					if (objects[layer][i] == last_clicked_object) {
						last_clicked_object = nullptr;
					}
					if (objects[layer][i]->getObjectType() == ObjectType::alien_fighter) {
						rank.addKills(1);
					}
					delete objects[layer][i];
					objects[layer].erase(objects[layer].begin() + i);
				}
			}
		}
	}

	void processEventBuffer() {
		while (true) {
			Event buffer_elem = event_buffer.getEvent();

			if (buffer_elem.getFirstObject() == nullptr) {
				break;
			}
			Object
				* obj1 = buffer_elem.getFirstObject(),
				* obj2 = buffer_elem.getSecondObject();
			double thresh = consts.getSpeedDamageThreshold();
			double coef = consts.getSpeedDamageCoef();

			UnitInfo * unit1 = obj1 ? obj1->getUnitInfo() : nullptr, * unit2 = obj2 ? obj2->getUnitInfo() : nullptr;
			switch (buffer_elem.getEventType()) {
			case null:
				break;
			case default_collision:
				if (obj1->getObjectType() == ObjectType::bullet && obj2->getObjectType() == ObjectType::bullet) {
					break;
				}
				if (obj1->getObjectType() == ObjectType::bullet && obj2->getObjectType() != ObjectType::asteroid) {
					obj1->deleteObject();
					event_buffer.addEvent(EventType::attack, (Object *)obj1->getUnitInfo()->getEnemy(), obj2);
					
				}
				if (obj2->getObjectType() == ObjectType::bullet && obj1->getObjectType() != ObjectType::asteroid && (obj1->getUnitInfo()->getFaction() != obj2->getUnitInfo()->getFaction())) {
					obj2->deleteObject();
					event_buffer.addEvent(EventType::attack, (Object *)obj2->getUnitInfo()->getEnemy(), obj1);
				}
				break;
			case attack:
				if (unit1 != nullptr && unit2 != nullptr) {
					float damage = unit1->getAttackDamage(1) * (unit1->isAffected(damage_buff) ? 2 : 1);
					unit2->dealDamage(damage);
					if ((obj1->getObjectType() == turret && obj1->getUnitInfo()->getFaction() == hero_faction)) {
						unit2->dealDamage(damage * research_manager.getDomeLocalDamageBonusCoef() * unit1->getDomeCount());
						unit1->grantHeal((1 + research_manager.getDomeLocalDamageBonusCoef() * unit1->getDomeCount()) * unit1->getAttackDamage(1) * research_manager.getTurretLifestealCoef() * (unit1->isAffected(damage_buff) ? 2 : 1));
					}
				}
				break;
			};
		}
	}

	bool processClick(Point click) {
		Object * cursor = new Object(click);
		if (!objects.empty()) {
			for (int layer = objects.size() - 1; layer >= 0; layer--) {
				for (int i = 0; i < objects[layer].size(); i++) {
					if (checkObjectCollision(objects[layer][i], cursor)) {

						if (objects[layer][i] != last_clicked_object) {

							event_buffer.addEvent(clicked, objects[layer][i], nullptr);

							last_clicked_object = objects[layer][i];
						}
						else {
							event_buffer.addEvent(pressed, objects[layer][i], nullptr);
						}
						delete cursor;
						return true;
					}
				}
			}
		}
		
		event_buffer.addEvent(released, last_clicked_object, nullptr);
		last_clicked_object = nullptr;

		delete cursor;
		return false;
	}

	void processObjectSpeed() {
		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				objects[layer][i]->forceChangePosition(objects[layer][i]->getSpeed() * consts.getFPSLock() / fps.getFPS());
			}
		}
	}

	void rebuildMap(Point gen_basis, float gen_radius, float cam_radius, bool save_out_range, bool fixed_asteroids, int asteroid_amount, float min_range, float max_range) {   // map is rebuilding around point gen_basis 
		int obj_count = 0, total_obj_count = 0;
		for (int i = 0; i < objects[landscape_layer].size(); i++) {
			if ((gen_basis - objects[landscape_layer][i]->getPosition()).getLength() > gen_radius) {
				if (!save_out_range) {
					objects[landscape_layer][i]->deleteObject();
				}
			}
			else {
				if (objects[landscape_layer][i]->getObjectType() == ObjectType::asteroid) {
					obj_count++;
				}
				total_obj_count++;
			}
		}

		int cnt, try_count = max_try_count;
		if (!fixed_asteroids) {
			cnt = max_try_count;
		}
		else {
			cnt = asteroid_amount - obj_count;
		}

		if (!obj_count) {
			FactionList faction = null_faction;

			Point new_pos = Point(0, 0);
			Object * object = new Object
			(
				new_pos,
				Point(),
				ObjectType::asteroid,
				CollisionType::asteroid_col,
				VisualInfo
				(
					SpriteType::asteroid_sprite,
					AnimationType::hold_anim,
					1000000000
				)
			);
			float angle = rand() / 16384.0 * PI * 2;
			float range = asteroid_speed;
			float x = cos(angle)*range, y = sin(angle)*range;
			object->setSpeed(Point(x, y));
			object->setAutoOrigin();
			object->getUnitInfo()->setFaction(faction);
			addObject(object, landscape_layer);

			std::vector<Object *> struct_arr = getRandomStructureSet(new_pos, object->getCollisionModel()->getModelElem(0)->collision_radius, struct_set, faction);
			for (int i = 0; i < struct_arr.size(); i++) {
				angle = rand() / 16384.0 * PI * 2;
				range = asteroid_speed;
				x = cos(angle)*range, y = sin(angle)*range;
				struct_arr[i]->setSpeed(Point(x, y));
				addObject(struct_arr[i], landscape_layer);
				object->attachObject(struct_arr[i]);
			}
		}
		
		while (cnt > 0) {
			bool in_min = false, in_max = false;
			try_count = max_try_count;
			float angle, range, x, y;
			while ((in_min || !in_max) && (cnt > 0) && (try_count > 0)) {
				in_min = false, in_max = false;
				angle = rand() / 16384.0 * PI * 2;
				range = cam_radius + rand() / 16384.0 * (gen_radius - cam_radius);
				x = cos(angle)*range + gen_basis.x, y = sin(angle)*range + gen_basis.y;
				for (int j = 0; j < objects[landscape_layer].size(); j++) {
					int cur_dist = (objects[landscape_layer][j]->getPosition() - Point(x, y)).getLength();
					if (cur_dist <= min_range) {
						in_min = true;
					}
					if (cur_dist <= max_range) {
						in_max = true;
					}
				}
				if (!fixed_asteroids) {
					cnt--;
				}
				else {
					try_count--;
				}
			}
			if (!cnt || !try_count) {
				continue;
			}
			if (fixed_asteroids) {
				cnt--;
			}

			FactionList faction = null_faction;
			int faction_score = rand() % 1000;
			if (faction_score > 900) {
				//faction = friendly_faction;
			}
			if (faction_score > 933) {
				//faction = neutral_faction;
			}
			if (faction_score > 966) {
				//faction = aggressive_faction;
			}
			Point new_pos = Point(x,y);
			Object * object = new Object
			(
				new_pos, 
				Point(), 
				ObjectType::asteroid, 
				CollisionType::asteroid_col, 
				VisualInfo
				(
					SpriteType::asteroid_sprite, 
					AnimationType::hold_anim, 
					1000000000
				)
			);
			object->setAutoOrigin();
			angle = rand() / 16384.0 * PI * 2;
			range = asteroid_speed;
			x = cos(angle)*range, y = sin(angle)*range;
			object->setSpeed(Point(x,y));
			object->getUnitInfo()->setFaction(faction);
			addObject(object, landscape_layer);
			

			std::vector<Object *> struct_arr = getRandomStructureSet(new_pos, object->getCollisionModel()->getModelElem(0)->collision_radius, struct_set, faction);
			for (int i = 0; i < struct_arr.size(); i++) {
				angle = rand() / 16384.0 * PI * 2;
				range = asteroid_speed;
				x = cos(angle)*range, y = sin(angle)*range;
				struct_arr[i]->setSpeed(Point(x, y));
				addObject(struct_arr[i], landscape_layer);
				object->attachObject(struct_arr[i]);
			}
		}
	}

	void spawnEnemyGroup(int enemy_lvl, Point pos) {
		int enemy_amount = sqrt(enemy_lvl);

		for (int i = 0; i < enemy_amount; i++) {
			float angle = ((float)i / enemy_amount) * 2 * PI;
			Point new_pos = pos + Point(cos(angle), sin(angle)) * (350 + enemy_amount * 10);

			Object * object = new Object
			(
				new_pos,
				Point(),
				ObjectType::alien_fighter,
				CollisionType::alien_fighter_col,
				VisualInfo
				(
					SpriteType::alien_fighter_sprite,
					AnimationType::hold_anim,
					1000000000
				)
			);
			object->setAutoOrigin();
			object->getUnitInfo()->setFaction(aggressive_faction);
			addObject(object, landscape_layer);
			setTurretArray(object);
		}
	}

	void setTurretArray(Object * base) {
		switch (base->getObjectType()) {
		case alien_fighter:
			Object * object = new Object
			(
				base->getPosition(),
				Point(),
				ObjectType::alien_turret1,
				CollisionType::alien_turret1_col,
				VisualInfo
				(
					SpriteType::alien_turret1_sprite,
					AnimationType::hold_anim,
					1000000000
				)
			);
			object->setAutoOrigin();
			object->getUnitInfo()->setFaction(aggressive_faction);
			addObject(object, main_layer);
			base->attachObject(object);
		}
	}

	void processDomeBonuses() {
		hero_object->getUnitInfo()->grantHeal(consts.getHeroHeal());
		// regeneration of structures for amount of domes on asteroid
		for (int i = 0; i < objects[landscape_layer].size(); i++) {
			if (objects[landscape_layer][i]->getObjectType() != asteroid) {
				continue;
			}
			int cnt = 0;
			std::vector<Object *> * attach = objects[landscape_layer][i]->getAttached();
			for (int j = 0; j < attach->size(); j++) {
				if ((*attach)[j]->getObjectType() == dome) {
					cnt++;
				}
			}
			for (int j = 0; j < attach->size(); j++) {
				(*attach)[j]->getUnitInfo()->grantHeal(cnt * research_manager.getDomeLocalRegenCoef() * consts.getDomeHeal());
				switch ((*attach)[j]->getObjectType()) {
				case gold:
					(*attach)[j]->getUnitInfo()->grantHeal(consts.getDomeHeal() * research_manager.getGoldRegenCoef());
					resource_manager.addGold(consts.getBaseGoldIncome() * (research_manager.getGoldIncomeCoef() + cnt * research_manager.getDomeLocalGoldIncomeCoef()) * consts.getFPSLock() / fps.getFPS());
					break;
				case science:
					(*attach)[j]->getUnitInfo()->grantHeal(consts.getDomeHeal() * research_manager.getScienceRegenCoef());
					resource_manager.addResearch(consts.getBaseResearchIncome() * (research_manager.getScienceIncomeCoef() + cnt * research_manager.getDomeLocalResearchIncomeCoef()) * consts.getFPSLock() / fps.getFPS());
					break;
				case turret:
					(*attach)[j]->getUnitInfo()->grantHeal(consts.getDomeHeal() * research_manager.getTurretRegenCoef());
					break;
				}
			}
		}
	}

public:

	Map() {}

	Map(std::string path) : objects(parseMap(path)) {
		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				if (objects[layer][i]->getObjectType() == ObjectType::hero) {
					hero_object = objects[layer][i];
				}
			}
		}
		createNavigationGrid();
		rebuildMap(hero_object->getPosition(), gen_radius, 0, save_out_range, 1, asteroid_amount, min_range, max_range);
	}

	~Map() {
		for (int i = 0; i < objects.size(); i++) {
			for (int j = 0; j < objects[i].size(); j++) {
				delete objects[i][j];
			}
		}
	}

	bool isClickable(Point click) {
		Object * cursor = new Object(click);
		if (!objects.empty()) {
			for (int layer = objects.size() - 1; layer >= 0; layer--) {
				for (int i = 0; i < objects[layer].size(); i++) {
					if (checkObjectCollision(objects[layer][i], cursor)) {
						delete cursor;
						return true;
					}
				}
			}
		}
		delete cursor;
		return false;
	}

	std::vector<std::vector<Object *>> * getObjectsBuffer() {
		return &objects;
	}

	void addObject(Object * object, int layer) {
		if (objects.size() <= layer) {
			objects.resize(layer + 1);
		}
		objects[layer].push_back(object);
		last_clicked_object = object;
	}

	void processFrame(Point click) {
		processClick(click);
		processObjectSpeed();
		processCollisionFrame();
		updateClosestAsteroid();
		processDomeBonuses();
		processUnitAI();
		processEventBuffer();
		garbageCollector();

		if (hero_object == nullptr) {
			return;
		}

		rebuildMap(hero_object->getPosition(), gen_radius, cam_radius, save_out_range, 1, asteroid_amount, min_range, max_range);
	}

	Object * getHero() {
		return hero_object;
	}

	Point getNavigationGridOffset() {
		return grid_offset;
	}

	int getNavigationGridElem(int x, int y) {
		if (!navigation_grid.empty() && x >= 0 && y >= 0 && x < navigation_grid.size() && y < navigation_grid[0].size()) {
			return navigation_grid[x][y];
		}
		else {
			return 0;
		}
	}

	std::vector<std::vector<Point>> getNavigationPaths() {
		return navigation_paths;
	}

	double getNavigationGridSize() {
		return grid_size;
	}

	void updateClosestAsteroid() {
		if ((closest_asteroid && ((hero_object->getPosition() - closest_asteroid->getPosition()).getLength() > consts.getInteractionDistance())) || !closest_asteroid) {
			for (int i = 0; i < objects[landscape_layer].size(); i++) {
				if (objects[0][i]->getObjectType() == ObjectType::asteroid) {
					if (((hero_object->getPosition() - objects[landscape_layer][i]->getPosition()).getLength() < consts.getInteractionDistance())) {
						closest_asteroid = objects[landscape_layer][i];
						return;
					}
				}
			}
			closest_asteroid = nullptr;
		}
	}

	Object * getClosestAsteroid() {
		return closest_asteroid;
	}

	bool addStructure(Object * base, ObjectType type) {
		if (base->getObjectType() != asteroid) {
			return false;
		}

		std::vector<Object *> outer_ring;
		std::vector<Object *> inner_ring;
		for (int i = 0; i < base->getAttached()->size(); i++) {
			if ((*base->getAttached())[i]->getObjectType() == turret) {
				outer_ring.push_back((*base->getAttached())[i]);
			}
			else {
				inner_ring.push_back((*base->getAttached())[i]);
			}
		}
		if (inner_ring.size() >= 7 && type != turret) {
			return false;
		}
		if (outer_ring.size() >= 10 && type == turret) {
			return false;
		}
		Object * object = nullptr;

		int dome_amount = 0;
		for (int i = 0; i < base->getAttached()->size(); i++) {
			if ((*base->getAttached())[i]->getObjectType() == dome) {
				dome_amount++;
			}
		}
		switch (type) {
		case turret:
			object = new Object
			(
				base->getPosition(),
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
			object->setAutoOrigin();
			object->getUnitInfo()->setMaxHealth(object->getUnitInfo()->getMaxHealth() * (research_manager.getTurretMaxHealthCoef() + dome_amount * research_manager.getDomeLocalMaxHealthCoef()));
			object->setSpeed(base->getSpeed());
			object->getUnitInfo()->setFaction(base->getUnitInfo()->getFaction());
			outer_ring.push_back(object);
			break;
		case dome:
			object = new Object
			(
				base->getPosition(),
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
			object->setAutoOrigin();
			object->getUnitInfo()->setMaxHealth(object->getUnitInfo()->getMaxHealth() * (research_manager.getDomeMaxHealthCoef() + dome_amount * research_manager.getDomeLocalMaxHealthCoef()));
			object->setSpeed(base->getSpeed());
			object->getUnitInfo()->setFaction(base->getUnitInfo()->getFaction());
			inner_ring.push_back(object);
			break;
		case science:
			object = new Object
			(
				base->getPosition(),
				Point(),
				ObjectType::science,
				CollisionType::science_col,
				VisualInfo
				(
					SpriteType::science_sprite,
					AnimationType::hold_anim,
					1000000000
				)
			);
			object->setAutoOrigin();
			object->getUnitInfo()->setMaxHealth(object->getUnitInfo()->getMaxHealth() * (research_manager.getScienceMaxHealthCoef() + dome_amount * research_manager.getDomeLocalMaxHealthCoef()));
			object->setSpeed(base->getSpeed());
			object->getUnitInfo()->setFaction(base->getUnitInfo()->getFaction());
			inner_ring.push_back(object);
			break;
		case gold:
			object = new Object
			(
				base->getPosition(),
				Point(),
				ObjectType::gold,
				CollisionType::gold_col,
				VisualInfo
				(
					SpriteType::gold_sprite,
					AnimationType::hold_anim,
					1000000000
				)
			);
			object->setAutoOrigin();
			object->getUnitInfo()->setMaxHealth(object->getUnitInfo()->getMaxHealth() * (research_manager.getGoldMaxHealthCoef() + dome_amount * research_manager.getDomeLocalMaxHealthCoef()));
			object->setSpeed(base->getSpeed());
			object->getUnitInfo()->setFaction(base->getUnitInfo()->getFaction());
			inner_ring.push_back(object);
			break;
		}
		if (object == nullptr) {
			return false;
		}
		base->attachObject(object);
		// rebuild structures list
		float base_angle = 0;
		if (type == turret) {

			base_angle = (float)(rand() % 1024) / 512 * PI;
			for (int i = 0; i < outer_ring.size(); i++) {
				float cur_angle = (float)i / outer_ring.size() * 2 * PI + base_angle;
				outer_ring[i]->setPosition(base->getPosition() + Point(sin(cur_angle), cos(cur_angle)) * (base->getCollisionModel()->getModelElem(0)->collision_radius - 60));
			}
		}
		else {
			base_angle = (float)(rand() % 1024) / 512 * PI;
			for (int i = 0; i < inner_ring.size(); i++) {
				float cur_angle = (float)i / std::min(6, (int)inner_ring.size()) * 2 * PI + base_angle;
				inner_ring[i]->setPosition(base->getPosition() + Point(sin(cur_angle), cos(cur_angle)) * (base->getCollisionModel()->getModelElem(0)->collision_radius - 180));
			}
		}

		if (inner_ring.size() % 6 == 1) {
			inner_ring[inner_ring.size()-1]->setPosition(base->getPosition());
		}
		addObject(object, landscape_layer);

		return true;
	}

	void spawnEnemy(int enemy_lvl, Point camera_pos) {
		std::vector<Point> convex;   // building convex hull on players asteroids and player position
		if ((camera_pos - hero_object->getPosition()).getLength() > 10) {
			convex.push_back(hero_object->getPosition());
		}
		for (int i = 0; i < objects[landscape_layer].size(); i++) {
			if (objects[landscape_layer][i]->getUnitInfo()->getFaction() == hero_faction) {
				convex.push_back(objects[landscape_layer][i]->getPosition());
			}
		}
		if (convex.size() > 1) {
			convex_hull(convex);
		}

		int group_amount = enemy_lvl;

		while (group_amount > 0) {
			int nearest_point = rand() % convex.size();
			float angle = (float)(rand() % 1024) / 512 * PI;
			Point new_spawn_point = convex[nearest_point] + Point(cos(angle), sin(angle)) * (consts.getEnemySpawnRange() + 0.1);
			bool outofrange = true;
			for (int i = 0; i < convex.size(); i++) {
				if ((convex[i] - new_spawn_point).getLength() < consts.getEnemySpawnRange()) {
					outofrange = false;
					break;
				}
			}
			if (!outofrange) {
				continue;
			}
			if (ConvexHull::in_convex(convex, new_spawn_point)) {
				continue;
			}

			spawnEnemyGroup(enemy_lvl, new_spawn_point);
			group_amount--;
		}
		

	}

	void setAsteroidBuff(Effect effect, Object * asteroid) {
		if (asteroid->getObjectType() != ObjectType::asteroid) {
			return;
		}
		std::vector<Object *> * attached = asteroid->getAttached();

		for (int i = 0; i < attached->size(); i++) {
			(*attached)[i]->setEffect(effect);
		}
	}
};