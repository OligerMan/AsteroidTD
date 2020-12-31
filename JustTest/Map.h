#pragma once

#include <map>

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
#include "Mission.h"
#include "NPCInfo.h"
#include "PhraseContainer.h"
#include "AsteroidGenerator.h"
#include "DialogAdditionalInfo.h"

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
    std::map<unsigned long long, void *> mission_info;
public:
	int landscape_layer = 0;
	int main_layer = 1;
private:

	std::vector<std::vector<int>> navigation_grid;
	std::vector<std::vector<Point>> navigation_paths;
	double grid_size = 10;
	Point grid_offset;

	Object * hero_object = nullptr;
	Object * closest_asteroid = nullptr;
	std::vector<Object *> discovered_asteroid_list;
	std::vector<Object *> npc_list;
    std::map<unsigned long long, std::vector<Object *>> enemy_wave_list;

	EventBuffer event_buffer;

	Object * last_clicked_object = nullptr;

	AsteroidGenerator asteroid_gen;
	Point gen_basis;
	float gen_radius = 8500, cam_radius = 5500, min_range = 1500, max_range = 2700, asteroid_speed = 0;
	int asteroid_amount = 50, max_try_count = 100;
	bool save_out_range = true, fixed_asteroids = true;

    MissionStageInfo mission_stage_info;

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
				if (objects[cnt][i]->getCollisionModel()->isStatic() || objects[cnt][i]->getObjectType() == alien_turret1) {
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
							ObjectType type = objects[cnt][j]->getObjectType();
							if (type == bullet || type == dome || type == turret || type == science || type == gold || type == alien_turret1) {
								continue;
							}
							if (checkObjectCollision(objects[cnt][i], objects[cnt][j])) {
								event_buffer.addEvent(EventType::default_collision, objects[cnt][i], objects[cnt][j]);
								fixCollision(objects[cnt][i], objects[cnt][j]);
							}
						}
					}
				}
				else {
					for (int j = 0; j < objects[landscape_layer].size(); j++) {
						if (objects[landscape_layer][j]->getObjectType() != asteroid && objects[landscape_layer][j]->getObjectType() != bullet) {
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

				ObjectType type = object1->getObjectType();
				if (type == dome || type == science || type == gold || object1->getObjectSpriteType() == bullet_sprite) {
					continue;
				}

				// turn to closest enemy
				if (faction1 != FactionList::null_faction) {
					for (int layer2 = 0; layer2 < objects.size(); layer2++) {
						for (int j = 0; j < objects[layer2].size(); j++) {
							if (i != j || layer1 != layer2) {
								Object * object2 = objects[layer2][j];
								if (object2->getUnitInfo() == nullptr) {
									continue;
								}
								if (object1->getObjectType() == turret) {
									if ((object1->getPosition() - object2->getPosition()).getLength() > research_manager.getTurretAttackRangeCoef()) {
										//continue;
									}
								}
								int faction2 = object2->getUnitInfo()->getFaction();

								if (areEnemies((FactionList)faction1, (FactionList)faction2)) {

									
									if (!(object1->getObjectType() == ObjectType::hero) && (object1->canObjectAttack() || object1->getUnitInfo()->getDefaultSpeed() > 0.0001)) {
										
										if (object2->getObjectType() == asteroid || object2->getObjectType() == bullet) {
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
												if (!object1->getCollisionModel()->isStatic() && (((Object *)object1->getUnitInfo()->getEnemy())->getPosition() - object1->getPosition()).getLength() > object1->getUnitInfo()->getMinimalFlightRange()) {
													Point speed(-cos(object1->getAngle() / 180 * PI + PI / 2), -sin(object1->getAngle() / 180 * PI + PI / 2));
													if (object1->getObjectSpriteType() != rocket_sprite) {
														object1->setSpeed(speed.getNormal()*object1->getUnitInfo()->getDefaultSpeed());
													}
													else {
														object1->changeSpeed(speed.getNormal()*(object1->getUnitInfo()->getDefaultSpeed() + std::max(0.4f, sqrt(1 / object1->getLifetime() * 2))));
														object1->setSpeed(object1->getSpeed().getNormal() * std::min(object1->getSpeed().getLength(), 15.0));
													}
												}
												else if((((Object *)object1->getUnitInfo()->getEnemy())->getPosition() - object1->getPosition()).getLength() < object1->getUnitInfo()->getMinimalFlightRange()){
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
					angle_diff = fixAngle(angle_diff);
					if (abs(angle_diff) < 0.05) {
						if (object1->getUnitInfo()->attackReady(1)) {
							Point bullet_pos = object1->getPosition() + Point(cos((object1->getAngle() - 90) / 180 * PI), sin((object1->getAngle() - 90) / 180 * PI)) * 95;
							SpriteType bullet_type = bullet_sprite;
							switch (object1->getObjectType()) {
							case rocket_launcher:
								bullet_type = rocket_sprite;
								break;
							}
							Object * object = new Object
							(
								bullet_pos,
								Point(),
								ObjectType::bullet,
								CollisionType::bullet_col,
								VisualInfo
								(
									bullet_type,
									AnimationType::hold_anim,
									1000000000
								)
							);
							object->getUnitInfo()->setFaction(object1->getUnitInfo()->getFaction());
							object->setParent(object1);     // to remember who is shooting
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
		int try_count = 3;
        auto deleted = [](Object * obj) {
            return obj != nullptr && (obj->isDeleted() || obj->getUnitInfo() == nullptr || obj->getUnitInfo()->isDead());
        };
		//bool dirty = true;
		while (try_count > 0) {
			bool smth_cleaned = false;
			for (int layer = 0; layer < objects.size(); layer++) {
				for (int i = 0; i < objects[layer].size(); i++) {
					if (objects[layer][i]->garbageCollector()) {
						smth_cleaned = true;
					}
					Object * enemy = (Object *)objects[layer][i]->getUnitInfo()->getEnemy();
					if (deleted(enemy)) {
						objects[layer][i]->getUnitInfo()->setEnemy(nullptr);
						smth_cleaned = true;
					}
				}
			}
            for (auto iter = enemy_wave_list.begin(); iter != enemy_wave_list.end(); iter++) {
                for (int i = 0; i < iter->second.size(); i++) {
                    if (deleted(iter->second[i])) {
                        iter->second.erase(iter->second.begin() + i);
                    }
                }
            }
            for (int i = 0; i < npc_list.size(); i++) {
                if (deleted(npc_list[i]) || (npc_list[i] && npc_list[i]->getAttached()->size() == 0)) {
                    delete static_cast<NPCInfo *>(npc_list[i]->getNPCInfo());
                    npc_list[i]->setNPCInfo(nullptr);
                    npc_list.erase(npc_list.begin() + i);
                }
            }
			if (!smth_cleaned) {
				try_count--;
			}
		}
		try_count = 3;
		//dirty = true;
		while (try_count > 0) {
			bool smth_cleaned = false;
			for (int layer = 0; layer < objects.size(); layer++) {
				for (int i = 0; i < objects[layer].size(); i++) {
					if (deleted(objects[layer][i])) {
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
						smth_cleaned = true;
						delete objects[layer][i];
						objects[layer].erase(objects[layer].begin() + i);
					}
				}
			}
			if (!smth_cleaned) {
				try_count--;
			}
		}
	}

	void processEventBuffer() {
		while (true) {
			Event buffer_elem = event_buffer.getEvent();

			if (buffer_elem.getData(0) == nullptr) {
				break;
			}
			double thresh = consts.getSpeedDamageThreshold();
			double coef = consts.getSpeedDamageCoef();

            Object * obj1, *obj2;
            UnitInfo * unit1, * unit2;
            std::wstring * side_effect_id, *side_effect_value;
			switch (buffer_elem.getEventType()) {
			case null_event:
				break;
			case default_collision:
            {
                obj1 = static_cast<Object *>(buffer_elem.getData(0));
                obj2 = static_cast<Object *>(buffer_elem.getData(1));
                unit1 = obj1 ? obj1->getUnitInfo() : nullptr;
                unit2 = obj2 ? obj2->getUnitInfo() : nullptr;
                if (obj1->getObjectType() == ObjectType::bullet && obj2->getObjectType() == ObjectType::bullet) {
                    break;
                }
                if (obj1->getObjectType() == ObjectType::bullet && obj2->getObjectType() != ObjectType::asteroid) {
                    if (unit1 && unit2) {
                        Object * parent1 = (Object *)obj1->getParent();
                        if ((parent1->getUnitInfo()->getFaction() != obj2->getUnitInfo()->getFaction())) {
                            event_buffer.addEvent(EventType::attack, parent1, obj2);
                            obj1->deleteObject();
                        }
                    }
                    else {
                        std::cout << "Bullet bug triggered" << std::endl;
                        if (obj1->isDeleted()) {
                            std::cout << "Supposed to be deleted" << std::endl;
                        }
                    }

                }
                if (obj2->getObjectType() == ObjectType::bullet && obj1->getObjectType() != ObjectType::asteroid) {
                    obj2->deleteObject();
                    if (unit1 && unit2) {
                        if ((obj1->getUnitInfo()->getFaction() != obj2->getUnitInfo()->getFaction())) {
                            event_buffer.addEvent(EventType::attack, (Object *)obj2->getParent(), obj1);
                        }
                    }
                    else {
                        std::cout << "Bullet bug triggered" << std::endl;
                        if (obj2->isDeleted()) {
                            std::cout << "Supposed to be deleted" << std::endl;
                        }
                    }

                }
                break;
            }
            case attack: 
            {
                obj1 = static_cast<Object *>(buffer_elem.getData(0));
                obj2 = static_cast<Object *>(buffer_elem.getData(1));
                unit1 = obj1 ? obj1->getUnitInfo() : nullptr;
                unit2 = obj2 ? obj2->getUnitInfo() : nullptr;
                if (unit1 != nullptr && unit2 != nullptr) {
                    float damage = unit1->getAttackDamage(1) * (unit1->isAffected(damage_buff) ? 2 : 1);
                    unit2->dealDamage(damage);
                    if ((obj1->getObjectType() == turret && obj1->getUnitInfo()->getFaction() == hero_faction)) {
                        unit2->dealDamage(damage * research_manager.getDomeLocalDamageBonusCoef() * unit1->getDomeCount());
                        unit1->grantHeal((1 + research_manager.getDomeLocalDamageBonusCoef() * unit1->getDomeCount()) * unit1->getAttackDamage(1) * research_manager.getTurretLifestealCoef() * (unit1->isAffected(damage_buff) ? 2 : 1));
                    }
                }
                break;
            }
            case dialog_side_effect:
            {
                side_effect_id = static_cast<std::wstring *>(buffer_elem.getData(0));
                side_effect_value = static_cast<std::wstring *>(buffer_elem.getData(1));
                
                // main part of side-effect handling

                if (side_effect_id->substr(0, 4).compare(L"set_") == 0) {
                    dialog_additional_info.setData(side_effect_id->substr(4), *side_effect_value);
                }

                break;
            }
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
		
		std::vector<std::pair<Point, std::reference_wrapper<AsteroidGenerator::NPC_Metadata>>> new_asteroid_list = asteroid_gen.processFrame(gen_basis, gen_radius);
		for (auto iter = new_asteroid_list.begin(); iter != new_asteroid_list.end(); iter++) {
			Point new_pos = iter->first;
			FactionList faction = iter->second.get().faction;

			static std::vector<std::vector<SpriteType>> tier_list{    // new tier system(old tiers mixed because of increased chance of special asteroids)
				{   // tier 0
					asteroid_gold_interspersed_sprite,
					asteroid_iron_interspersed_sprite,
					asteroid_suspiciously_flat_sprite
				},
				{   // tier 1

					asteroid_gold_interspersed_sprite,
					asteroid_iron_interspersed_sprite,
					asteroid_suspiciously_flat_sprite,

					asteroid_gold_interspersed_sprite,
					asteroid_iron_interspersed_sprite,
					asteroid_suspiciously_flat_sprite,

					asteroid_strange_cracked_sprite,
					asteroid_ordinary_wealthy_sprite,
					asteroid_poor_mountainous_sprite,
					asteroid_wealthy_cracked_sprite,
					asteroid_ordinary_mountainous_sprite,
					asteroid_strange_poor_sprite
				},
				{   // tier 2
					asteroid_gold_interspersed_sprite,
					asteroid_iron_interspersed_sprite,
					asteroid_suspiciously_flat_sprite,

					asteroid_gold_interspersed_sprite,
					asteroid_iron_interspersed_sprite,
					asteroid_suspiciously_flat_sprite,

					asteroid_strange_cracked_sprite,
					asteroid_ordinary_wealthy_sprite,
					asteroid_poor_mountainous_sprite,
					asteroid_wealthy_cracked_sprite,
					asteroid_ordinary_mountainous_sprite,
					asteroid_strange_poor_sprite,

					asteroid_strange_cracked_sprite,
					asteroid_ordinary_wealthy_sprite,
					asteroid_poor_mountainous_sprite,
					asteroid_wealthy_cracked_sprite,
					asteroid_ordinary_mountainous_sprite,
					asteroid_strange_poor_sprite,

					asteroid_swampy_with_gold_mines_sprite,
					asteroid_unstable_explosive_ore_sprite,
					asteroid_old_laboratory_sprite,
					asteroid_lava_surface_sprite
				},
				{   // tier 3
					asteroid_gold_interspersed_sprite,
					asteroid_iron_interspersed_sprite,
					asteroid_suspiciously_flat_sprite,

					asteroid_gold_interspersed_sprite,
					asteroid_iron_interspersed_sprite,
					asteroid_suspiciously_flat_sprite,

					asteroid_gold_interspersed_sprite,
					asteroid_iron_interspersed_sprite,
					asteroid_suspiciously_flat_sprite,

					asteroid_strange_cracked_sprite,
					asteroid_ordinary_wealthy_sprite,
					asteroid_poor_mountainous_sprite,
					asteroid_wealthy_cracked_sprite,
					asteroid_ordinary_mountainous_sprite,
					asteroid_strange_poor_sprite,

					asteroid_strange_cracked_sprite,
					asteroid_ordinary_wealthy_sprite,
					asteroid_poor_mountainous_sprite,
					asteroid_wealthy_cracked_sprite,
					asteroid_ordinary_mountainous_sprite,
					asteroid_strange_poor_sprite,

					asteroid_swampy_with_gold_mines_sprite,
					asteroid_unstable_explosive_ore_sprite,
					asteroid_old_laboratory_sprite,
					asteroid_lava_surface_sprite,

					asteroid_drone_factory_sprite,
					asteroid_rocket_launcher_sprite,
					asteroid_ancient_laboratory_sprite,
					asteroid_ancient_giant_gold_mine_sprite
				}
			};

			SpriteType asteroid_type = asteroid_sprite;

			int special_asteroid_chance = rand() % 1000;
			bool npc_asteroid_chance = (rand() % 1000) < 200;
            if (game_mode != GameMode::adventure_mode) {
                npc_asteroid_chance = 0;
                faction = null_faction;
            }
			if (npc_asteroid_chance) {
			}
			else {
				if (special_asteroid_chance < 100) {
					asteroid_type = tier_list[0][rand() % tier_list[0].size()];
				}
				int num = std::min((int)tier_list.size() - 1, std::max(0, (int)(new_pos.getLength() / consts.getTierRange())));

				if (special_asteroid_chance < 250) {
					asteroid_type = tier_list[std::max(0, num - 2)][rand() % tier_list[std::max(0, num - 2)].size()];
				}
				if (special_asteroid_chance < 500) {
					asteroid_type = tier_list[std::max(0, num - 1)][rand() % tier_list[std::max(0, num - 1)].size()];
				}
				if (special_asteroid_chance < 750) {
					asteroid_type = tier_list[num][rand() % tier_list[num].size()];
				}
			}

			Object * object = new Object
			(
				new_pos,
				Point(),
				ObjectType::asteroid,
				CollisionType::asteroid_col,
				VisualInfo
				(
					asteroid_type,
					AnimationType::hold_anim,
					1000000000
				)
			);
			object->setAutoOrigin();
			object->getUnitInfo()->setFaction(faction);
			addObject(object, landscape_layer);

			if (npc_asteroid_chance) {
				WorldFactionList w_fact = WorldFactionList(faction - FactionList::alliance_of_ancient_knowledge);
				object->initNPCInfo(new NPCInfo(w_fact));
				int base_npc_lvl =
					(float)rand() / (float)(RAND_MAX + 1) *
					(float)rand() / (float)(RAND_MAX + 1) *
					(float)rand() / (float)(RAND_MAX + 1) *
					(float)rand() / (float)(RAND_MAX + 1);

				int special_mission_lvl_dif = rand() / (RAND_MAX + 1);
				Mission base_mission = createMission(w_fact, base_npc_lvl);
				Mission special_mission = createMission(w_fact, 1 - (1 - base_npc_lvl) * special_mission_lvl_dif);
				if (!base_mission.isFailed()) {
					static_cast<NPCInfo *>(object->getNPCInfo())->changeBaseMission(base_mission);
				}
				if (!special_mission.isFailed()) {
					static_cast<NPCInfo *>(object->getNPCInfo())->changeSpecialMission(special_mission);
				}
				npc_list.push_back(object);
			}

			std::vector<Object *> struct_arr = getRandomStructureSet(new_pos, object->getCollisionModel()->getModelElem(0)->collision_radius, struct_set, faction);
			for (int i = 0; i < struct_arr.size(); i++) {
				struct_arr[i]->setSpeed(object->getSpeed());
				addObject(struct_arr[i], landscape_layer);
				object->attachObject(struct_arr[i]);
			}
		}
	}

	void spawnEnemyGroup(int fighter_amount, int gunship_amount, Point pos, std::vector<Object *> * enemy_list = nullptr) {

		for (int i = 0; i < fighter_amount; i++) {
			float angle = ((float)i / fighter_amount) * 2 * PI;
			Point new_pos = pos + Point(cos(angle), sin(angle)) * (350 + fighter_amount * 10);

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
            if (enemy_list) {
                enemy_list->push_back(object);
            }
			setTurretArray(object);
		}

		for (int i = 0; i < gunship_amount; i++) {
			float angle = ((float)i / gunship_amount) * 2 * PI;
			Point new_pos = pos + Point(cos(angle), sin(angle)) * (500 + gunship_amount * 10);

			Object * object = new Object
			(
				new_pos,
				Point(),
				ObjectType::alien_gunship,
				CollisionType::alien_gunship_col,
				VisualInfo
				(
					SpriteType::alien_gunship_sprite,
					AnimationType::hold_anim,
					1000000000
				)
			);
			object->setAutoOrigin();
			object->getUnitInfo()->setFaction(aggressive_faction);
			addObject(object, landscape_layer);
            if (enemy_list) {
                enemy_list->push_back(object);
            }
			setTurretArray(object);
		}
	}

	void setTurretArray(Object * base) {
		Object * object;
		switch (base->getObjectType()) {
		case alien_fighter:
			object = new Object
			(
				base->getPosition(),
				Point(),
				ObjectType::alien_turret2,
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
			break;
		case alien_gunship:
			object = new Object
			(
				base->getPosition() + Point(-77, 0),
				Point(),
				ObjectType::alien_turret2,
				CollisionType::alien_turret2_col,
				VisualInfo
				(
					SpriteType::alien_turret2_sprite,
					AnimationType::hold_anim,
					1000000000
				)
			);
			object->setAutoOrigin();
			object->getUnitInfo()->setFaction(aggressive_faction);
			addObject(object, main_layer);
			base->attachObject(object);

			object = new Object
			(
				base->getPosition() + Point(74, 0),
				Point(),
				ObjectType::alien_turret2,
				CollisionType::alien_turret2_col,
				VisualInfo
				(
					SpriteType::alien_turret2_sprite,
					AnimationType::hold_anim,
					1000000000
				)
			);
			object->setAutoOrigin();
			object->getUnitInfo()->setFaction(aggressive_faction);
			addObject(object, main_layer);
			base->attachObject(object);

			object = new Object
			(
				base->getPosition() + Point(69, -45),
				Point(),
				ObjectType::alien_turret2,
				CollisionType::alien_turret2_col,
				VisualInfo
				(
					SpriteType::alien_turret2_sprite,
					AnimationType::hold_anim,
					1000000000
				)
			);
			object->setAutoOrigin();
			object->getUnitInfo()->setFaction(aggressive_faction);
			addObject(object, main_layer);
			base->attachObject(object);

			object = new Object
			(
				base->getPosition() + Point(-73, -45),
				Point(),
				ObjectType::alien_turret2,
				CollisionType::alien_turret2_col,
				VisualInfo
				(
					SpriteType::alien_turret2_sprite,
					AnimationType::hold_anim,
					1000000000
				)
			);
			object->setAutoOrigin();
			object->getUnitInfo()->setFaction(aggressive_faction);
			addObject(object, main_layer);
			base->attachObject(object);
			break;
        case alien_bombard:
            object = new Object
            (
                base->getPosition() + Point(-77, 0),
                Point(),
                ObjectType::alien_turret3,
                CollisionType::alien_turret3_col,
                VisualInfo
                (
                    SpriteType::alien_turret3_sprite,
                    AnimationType::hold_anim,
                    1000000000
                )
            );
            object->setAutoOrigin();
            object->getUnitInfo()->setFaction(aggressive_faction);
            addObject(object, main_layer);
            base->attachObject(object);
            break;
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
			if (objects[landscape_layer][i]->getUnitInfo() && objects[landscape_layer][i]->getUnitInfo()->getFaction() == hero_faction) {
				if (objects[landscape_layer][i]->getAttached()->size() >= 7) {
					switch (objects[landscape_layer][i]->getObjectSpriteType()) {
					case asteroid_old_laboratory_sprite:
						resource_manager.addResearch(consts.getBaseResearchIncome() * 30 * consts.getFPSLock() / fps.getFPS());
						break;
					case asteroid_ancient_laboratory_sprite:
						resource_manager.addResearch(consts.getBaseResearchIncome() * 50 * consts.getFPSLock() / fps.getFPS());
						break;
					case asteroid_ancient_giant_gold_mine_sprite:
						resource_manager.addGold(consts.getBaseGoldIncome() * 50 * consts.getFPSLock() / fps.getFPS());
						break;
					}
				}
				for (int j = 0; j < attach->size(); j++) {
					(*attach)[j]->getUnitInfo()->grantHeal(cnt * research_manager.getDomeLocalRegenCoef() * consts.getDomeHeal());
					float gold_coef = 1, research_coef = 1;
					switch (objects[landscape_layer][i]->getObjectSpriteType()) {
					case asteroid:
						break;
					case asteroid_gold_interspersed_sprite:
						gold_coef = 1.1;
						break;
					case asteroid_suspiciously_flat_sprite:
						research_coef = 1.1;
						break;
					case asteroid_strange_cracked_sprite:
						research_coef = 1.2;
						break;
					case asteroid_ordinary_wealthy_sprite:
						gold_coef = 1.25;
						research_coef = 0.8;
						break;
					case asteroid_poor_mountainous_sprite:
						gold_coef = 0.75;
						break;
					case asteroid_wealthy_cracked_sprite:
						gold_coef = 1.25;
						break;
					case asteroid_ordinary_mountainous_sprite:
						research_coef = 0.8;
						break;
					case asteroid_strange_poor_sprite:
						research_coef = 1.2;
						gold_coef = 0.75;
						break;
					case asteroid_swampy_with_gold_mines_sprite:
						gold_coef = 1.5;
						break;
					}
					switch ((*attach)[j]->getObjectType()) {
					case gold:
						(*attach)[j]->getUnitInfo()->grantHeal(consts.getDomeHeal() * research_manager.getGoldRegenCoef());
						resource_manager.addGold(
							consts.getBaseGoldIncome() *
							(research_manager.getGoldIncomeCoef() + cnt * research_manager.getDomeLocalGoldIncomeCoef()) *
							gold_coef *
							consts.getFPSLock() / fps.getFPS());
						break;
					case science:
						(*attach)[j]->getUnitInfo()->grantHeal(consts.getDomeHeal() * research_manager.getScienceRegenCoef() * consts.getFPSLock() / fps.getFPS());
						resource_manager.addResearch(
							consts.getBaseResearchIncome() *
							(research_manager.getScienceIncomeCoef() + cnt * research_manager.getDomeLocalResearchIncomeCoef()) *
							research_coef *
							consts.getFPSLock() / fps.getFPS());
						break;
					case turret:
						(*attach)[j]->getUnitInfo()->grantHeal(consts.getDomeHeal() * research_manager.getTurretRegenCoef() * consts.getFPSLock() / fps.getFPS());
						break;
					}
				}
			}
		}
	}

	void processDiscovery(Point center) {
		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				if ((center - objects[layer][i]->getPosition()).getLength() < consts.getDiscoveryRange()) {
					objects[layer][i]->setDiscovered();
				}
			}
		}
		discovered_asteroid_list.clear();
		for (int i = 0; i < objects[landscape_layer].size(); i++) {
			if (!objects[landscape_layer][i]->isDiscovered()) {
				discovered_asteroid_list.push_back(objects[landscape_layer][i]);
			}
		}
	}

	Object * randomDiscoveredAsteroid() {
		if (discovered_asteroid_list.size()) {
			return discovered_asteroid_list[rand() * discovered_asteroid_list.size() / (RAND_MAX + 1)];
		}
		return objects[landscape_layer][1];
	}

	Object * randomNPCAsteroid() {
		if (npc_list.size()) {
			return npc_list[npc_list.size() * rand() / (RAND_MAX + 1)];
		}
		return nullptr;
	}

    Mission createMission(WorldFactionList faction, float mission_lvl_coef) {

        auto mission_stage_list = mission_stage_info.getRandomMissionDescription();
        float reward_coef = mission_stage_list.first;
        float reward = (int)consts.getMinimalMissionPrice() + (int)(consts.getMaxRandomMissionPriceAddition() * reward_coef * (1 - pow(consts.getMissionPriceChangeCoef(), 100 * mission_lvl_coef))) / 5 * 5;
        Mission output(reward);

        for (auto i = mission_stage_list.second.begin(); i != mission_stage_list.second.end(); i++) {
            BaseInfo * info = *i;
            void * cour_obj_ptr, * def_obj_ptr;
            switch (info->getType()) {
            case BaseInfo::courier:
                cour_obj_ptr = randomNPCAsteroid();
                if (cour_obj_ptr == nullptr) {
                    output.setFailState();
                }
                output.addMissionStage(new CourierStage(cour_obj_ptr, info->getStringID()));
                break;
            case BaseInfo::defence:
                def_obj_ptr = randomNPCAsteroid();
                if (def_obj_ptr == nullptr) {
                    output.setFailState();
                }
                output.addMissionStage(new DefenceStage(def_obj_ptr, *static_cast<DefenceInfo *>(info), info->getMinLvl() + (info->getMaxLvl() - info->getMinLvl()) * mission_lvl_coef));
                break;
            }
        }
		return output;
	}

	void checkObjective() {
        Mission * cur_miss = rpg_profile.getCurrentMission();
        if (cur_miss == nullptr) {
            return;
        }
		if (!cur_miss->completed()) {
			Objective objective = cur_miss->getObjective();
            float delay;
            switch (cur_miss->getMissionStageType()) {
            case MissionStage::courier:
                switch (objective.type) {
                case Objective::point:
                    if (hero_object && (hero_object->getPosition() - static_cast<Object *>(objective.data)->getPosition()).getLength() < consts.getInteractionDistance()) {
                        cur_miss->setObjectiveCompleted();
                        if (rpg_profile.getCurrentMission()->completed()) {
                            std::vector<std::wstring> buffer = phrase_container.getPhraseBuffer(L"courier_mission_completed_npc", 0);
                            std::wstring message_string = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
                            global_event_buffer.push_back(Event(new float(rpg_profile.getCurrentMission()->getReward()), reward));
                            global_event_buffer.push_back(Event(new std::wstring(message_string), message));
                        }
                    }
                    break;
                }
                break;
            case MissionStage::defence:
                switch (objective.type) {
                case Objective::point:
                    if (hero_object && (hero_object->getPosition() - static_cast<Object *>(objective.data)->getPosition()).getLength() < consts.getInteractionDistance()) {
                        cur_miss->setObjectiveCompleted();
                    }
                    break;
                case Objective::wave_delay:
					delay = *static_cast<float *>(objective.data);
                    if (mission_info.count(cur_miss->getID())) {
						auto iter = mission_info.find(cur_miss->getID());
						auto time_dif = std::chrono::steady_clock::now() - *(std::chrono::time_point<std::chrono::steady_clock> *)(iter->second);
						if (std::chrono::duration_cast<std::chrono::seconds>(time_dif).count() > delay) {
							mission_info.erase(iter);
							cur_miss->setObjectiveCompleted();
						}
                    }
                    else {
                        mission_info.insert(std::pair<unsigned long long, void *>(cur_miss->getID(), (void *)new std::chrono::time_point<std::chrono::steady_clock>(std::chrono::steady_clock::now())));
                    }
                    break;
                case Objective::wave_level:
					if (enemy_wave_list.count(cur_miss->getID())) {
                        auto iter = enemy_wave_list.find(cur_miss->getID());
                        if (iter->second.empty()) {
                            cur_miss->setObjectiveCompleted();
                            enemy_wave_list.erase(iter);
                            if (rpg_profile.getCurrentMission()->completed()) {
                                std::vector<std::wstring> buffer = phrase_container.getPhraseBuffer(L"defence_mission_completed_npc", 0);
                                std::wstring message_string = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
                                global_event_buffer.push_back(Event(new float(rpg_profile.getCurrentMission()->getReward()), reward));
                                global_event_buffer.push_back(Event(new std::wstring(message_string), message));
                            }
                        }
					}
					else if(hero_object){
                        std::vector<Object *> enemy_list;
                        float angle = (float)(rand() % 1024) / 512 * PI;
                        Point new_spawn_point = hero_object->getPosition() + Point(cos(angle), sin(angle)) * (consts.getDefenceMissionEnemySpawnRange() + 0.1);
                        int enemy_lvl = *static_cast<float *>(objective.data);
                        int gunship_amount = enemy_lvl / 10;
                        int fighter_amount = std::max(1, enemy_lvl / 2 - 2 * gunship_amount);
                        spawnEnemyGroup(fighter_amount, gunship_amount, new_spawn_point, &enemy_list);
						enemy_wave_list.insert(std::pair<unsigned long long, std::vector<Object *>>(cur_miss->getID(), enemy_list));
					}
                    break;
                }
                break;
            }
			
		}
	}

public:

	Map(std::string mission_desc_path) : 
		asteroid_gen(2500.0f, 500.0f, 0.0f, rand()),
		mission_stage_info(mission_desc_path) {}

	Map(std::string path, std::string mission_desc_path) : 
        objects(parseMap(path)), 
        mission_stage_info(mission_desc_path),
		asteroid_gen(2500.0f, 500.0f, 0.0f, rand()) {

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
        if (object->getObjectSpriteType() == null_sprite) {
            std::cout << "wtf" << std::endl;
        }
		objects[layer].push_back(object);
		last_clicked_object = object;
	}

	void processFrame(Point click, Point view_pos) {
		processClick(click);
		processObjectSpeed();
		processCollisionFrame();
		updateClosestAsteroid();
		processDomeBonuses();
		processUnitAI();
		processEventBuffer();
		garbageCollector();
		processDiscovery(view_pos);
		checkObjective();
		rpg_profile.processCompletedMissions();

		if (hero_object == nullptr) {
			return;
		}

		rebuildMap(view_pos, gen_radius, cam_radius, save_out_range, 1, asteroid_amount, min_range, max_range);
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
			if (((*base->getAttached())[i]->getPosition() - base->getPosition()).getLength() <= 150) {
				inner_ring.push_back((*base->getAttached())[i]);
			}
			else {
				outer_ring.push_back((*base->getAttached())[i]);
			}
		}
		Object * object = nullptr;
		if (inner_ring.size() >= 7 && 
			(
				outer_ring.size() >= 10 || 
				base->getObjectSpriteType() == asteroid_old_laboratory_sprite || 
				base->getObjectSpriteType() == asteroid_ancient_laboratory_sprite || 
				base->getObjectSpriteType() == asteroid_ancient_giant_gold_mine_sprite || 
				base->getObjectSpriteType() == asteroid_drone_factory_sprite || 
				base->getObjectSpriteType() == asteroid_rocket_launcher_sprite)) {

			return false;
		}

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
			object->setSpeed(base->getSpeed());
			object->getUnitInfo()->setFaction(base->getUnitInfo()->getFaction());
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
			object->setSpeed(base->getSpeed());
			object->getUnitInfo()->setFaction(base->getUnitInfo()->getFaction());
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
			object->setSpeed(base->getSpeed());
			object->getUnitInfo()->setFaction(base->getUnitInfo()->getFaction());
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
			object->setSpeed(base->getSpeed());
			object->getUnitInfo()->setFaction(base->getUnitInfo()->getFaction());
			break;
		}
		if (object == nullptr) {
			return false;
		}
		object->researchApply(dome_amount);
		switch (base->getObjectSpriteType()) {
		case asteroid_iron_interspersed_sprite:
			object->getUnitInfo()->maxHealthRescale(1.1);
			break;
		case asteroid_strange_cracked_sprite:
			object->getUnitInfo()->maxHealthRescale(0.75);
			break;
		case asteroid_poor_mountainous_sprite:
			object->getUnitInfo()->maxHealthRescale(1.25);
			break;
		case asteroid_wealthy_cracked_sprite:
			object->getUnitInfo()->maxHealthRescale(0.75);
			break;
		case asteroid_ordinary_mountainous_sprite:
			object->getUnitInfo()->maxHealthRescale(1.25);
			break;
		case asteroid_unstable_explosive_ore_sprite:
			object->getUnitInfo()->damageRescale(1.5);
			break;
		}

		base->attachObject(object);
		// rebuild structures list

		float base_angle = 0;
		if (type == turret) {
			if (outer_ring.size() >= 10) {
				inner_ring.push_back(object);
				base_angle = (float)(rand() % 1024) / 512 * PI;
				for (int i = 0; i < inner_ring.size(); i++) {
					float cur_angle = (float)i / std::min(6, (int)inner_ring.size()) * 2 * PI + base_angle;
					inner_ring[i]->setPosition(base->getPosition() + Point(sin(cur_angle), cos(cur_angle)) * (base->getCollisionModel()->getModelElem(0)->collision_radius - 180));
				}
				if (inner_ring.size() % 6 == 1) {
					inner_ring[inner_ring.size() - 1]->setPosition(base->getPosition());
				}
			}
			else {
				outer_ring.push_back(object);
				base_angle = (float)(rand() % 1024) / 512 * PI;
				for (int i = 0; i < outer_ring.size(); i++) {
					float cur_angle = (float)i / outer_ring.size() * 2 * PI + base_angle;
					outer_ring[i]->setPosition(base->getPosition() + Point(sin(cur_angle), cos(cur_angle)) * (base->getCollisionModel()->getModelElem(0)->collision_radius - 60));
				}
			}
		}
		else {
			if (inner_ring.size() >= 7) {
				outer_ring.push_back(object);
				base_angle = (float)(rand() % 1024) / 512 * PI;
				for (int i = 0; i < outer_ring.size(); i++) {
					float cur_angle = (float)i / outer_ring.size() * 2 * PI + base_angle;
					outer_ring[i]->setPosition(base->getPosition() + Point(sin(cur_angle), cos(cur_angle)) * (base->getCollisionModel()->getModelElem(0)->collision_radius - 60));
				}
			}
			else {
				inner_ring.push_back(object);
				base_angle = (float)(rand() % 1024) / 512 * PI;
				for (int i = 0; i < inner_ring.size(); i++) {
					float cur_angle = (float)i / std::min(6, (int)inner_ring.size()) * 2 * PI + base_angle;
					inner_ring[i]->setPosition(base->getPosition() + Point(sin(cur_angle), cos(cur_angle)) * (base->getCollisionModel()->getModelElem(0)->collision_radius - 180));
				}
				if (inner_ring.size() % 6 == 1) {
					inner_ring[inner_ring.size() - 1]->setPosition(base->getPosition());
				}
			}
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

		int group_amount = std::min(5, enemy_lvl / 2) + sqrt(enemy_lvl);
		int gunship_amount = enemy_lvl / 12 + (enemy_lvl > 24 ? (enemy_lvl - 24) / 6 : 0);
		int fighter_amount = std::max(1, enemy_lvl / 2 - 3 * gunship_amount - (enemy_lvl > 10 ? (enemy_lvl - 10) / 4 : 0));

		while (group_amount > 0) {
			int nearest_point = rand() % convex.size();
			float angle = (float)(rand() % 1024) / 512 * PI;
			Point new_spawn_point = convex[nearest_point] + Point(cos(angle), sin(angle)) * (consts.getInfiniteModeEnemySpawnRange() + 0.1);
			bool outofrange = true;
			for (int i = 0; i < convex.size(); i++) {
				if ((convex[i] - new_spawn_point).getLength() < consts.getInfiniteModeEnemySpawnRange()) {
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


			spawnEnemyGroup(fighter_amount, gunship_amount, new_spawn_point);
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

	void saveToFile(std::string path) {
		std::ifstream save(path);

	}
};