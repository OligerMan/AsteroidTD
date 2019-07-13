#pragma once

#include "Object.h"
#include "EventBuffer.h"
#include "MapParser.h"
#include "GameConstants.h"
#include "AStarAlgorithm.h"
#include "StructureSetGeneration.h"

void fixCollision(Object * obj1, Object * obj2) { 
	bool force_move = obj1->getCollisionModel()->isStatic() && obj2->getCollisionModel()->isStatic();
	const double min_speed = 0.001;
	float eps_speed = 0.1;

	CollisionModel * col1 = obj1->getCollisionModel();
	CollisionModel * col2 = obj2->getCollisionModel();

	Point tmp_speed1 = col1->getSpeed();
	Point tmp_speed2 = col2->getSpeed();

	Point speed1 = tmp_speed1, speed2 = tmp_speed2;
	Point origin1 = col1->getOrigin(), origin2 = col2->getOrigin();


	// set previous position and add to it half of speed

	tmp_speed1 /= 2;
	tmp_speed2 /= 2;
	col1->changePosition(Point() - tmp_speed1);
	col2->changePosition(Point() - tmp_speed2);
	if (force_move) {
		col1->forceChangePosition(Point() - tmp_speed1);
		col2->forceChangePosition(Point() - tmp_speed2);
	}
	
	while (tmp_speed1.getLength() > min_speed || tmp_speed2.getLength() > min_speed) {
		tmp_speed1 /= 2;
		tmp_speed2 /= 2;
		if (checkModelCollision(col1, col2)) {
			col1->changePosition(Point() - tmp_speed1);
			col2->changePosition(Point() - tmp_speed2);
			if (force_move) {
				col1->forceChangePosition(Point() - tmp_speed1);
				col2->forceChangePosition(Point() - tmp_speed2);
			}
		}
		else {
			col1->changePosition(Point() + tmp_speed1);
			col2->changePosition(Point() + tmp_speed2);
			if (force_move) {
				col1->forceChangePosition(Point() + tmp_speed1);
				col2->forceChangePosition(Point() + tmp_speed2);
			}
		}
	}

	Point pos1 = col1->getPosition();
	Point pos2 = col2->getPosition();

	Point normal_vect = (pos1 - pos2).getNormal();
	normal_vect = Point(normal_vect.y, -normal_vect.x);

	Point normal_speed1 = (normal_vect * (normal_vect.x * (speed1 - tmp_speed1).x + normal_vect.y * (speed1 - tmp_speed1).y));
	col1->changePosition(normal_speed1);
	Point normal_speed2 = (normal_vect * (normal_vect.x * (speed2 - tmp_speed2).x + normal_vect.y * (speed2 - tmp_speed2).y));
	col2->changePosition(normal_speed2);
	if (force_move) {
		col1->forceChangePosition(normal_speed1);
		col2->forceChangePosition(normal_speed2);
	}

	if (checkModelCollision(col1, col2)) {
		col1->changePosition((col1->getPosition() - col2->getPosition()).getNormal() * eps_speed);
		col2->changePosition((col2->getPosition() - col1->getPosition()).getNormal() * eps_speed);
		if (force_move) {
			col1->forceChangePosition((col1->getPosition() - col2->getPosition()).getNormal() * eps_speed);
			col2->forceChangePosition((col2->getPosition() - col1->getPosition()).getNormal() * eps_speed);
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

	Object * hero = nullptr;
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
				if (objects[cnt][i] != hero) {
					if (checkObjectCollision(hero, objects[cnt][i])) {
						event_buffer.addEvent(EventType::default_collision, objects[cnt][i], hero);
					}
				}
				for (int j = 0; j < objects[cnt].size(); j++) {
					if (i != j) {
						if (checkObjectCollision(objects[cnt][i], objects[cnt][j])) {
							event_buffer.addEvent(EventType::default_collision, objects[cnt][i], objects[cnt][j]);
							fixCollision(objects[cnt][i], objects[cnt][j]);
						}
					}
				}
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

								if (areEnemies((FactionList)faction1, (FactionList)faction2)) {
									if (!(faction1 == hero_faction) && object1->canObjectAttack()) {
										Point vect = (object2->getPosition() - object1->getPosition()).getNormal();
										Object * prev_enemy = (Object *)object1->getUnitInfo()->getEnemy();
										if (prev_enemy != nullptr) {
											if ((prev_enemy->getPosition() - object1->getPosition()).getLength() > (object2->getPosition() - object1->getPosition()).getLength()) {
												object1->getUnitInfo()->setEnemy((void *)object2);
											}
											else {
												vect = (prev_enemy->getPosition() - object1->getPosition()).getNormal();
											}
											if ((object1->getPosition() - prev_enemy->getPosition()).getLength() > object1->getUnitInfo()->getAngerRange()) {
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
													object1->changeAngle(angle_diff / 10.0);
												}
											}
										}
										else if((object1->getPosition() - object2->getPosition()).getLength() <= object1->getUnitInfo()->getAngerRange()){
											object1->getUnitInfo()->setEnemy((void *)object2);
										}
									}
								}
							}
						}
					}
				}

				// shoot if enemy is on your way and in enough range
				// TODO
				if (object1->getUnitInfo()->getEnemy() != nullptr) {
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
						if (object1->getUnitInfo()->attack1Ready()) {
							Point bullet_pos = object1->getPosition() + Point(cos((object1->getAngle() - 90) / 180 * PI), sin((object1->getAngle() - 90) / 180 * PI)) * 65;
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
							object->getUnitInfo()->setFaction(FactionList::null_faction);
							object->getUnitInfo()->setEnemy(object1);     // to remember who is shooting
							object->setAutoOrigin();
							object->setAngle(object1->getAngle());
							object->setSpeed(Point(cos((object1->getAngle() - 90) / 180 * PI), sin((object1->getAngle() - 90) / 180 * PI)) * 5);

							addObject(object, main_layer);
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
			}
		}
		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				if (objects[layer][i]->isDeleted() || !(objects[layer][i]->getUnitInfo() != nullptr && !objects[layer][i]->getUnitInfo()->isDead())) {
					if (objects[layer][i] == hero) {
						hero = nullptr;
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
				if (obj1->getObjectType() == ObjectType::bullet) {
					obj1->deleteObject();
					event_buffer.addEvent(EventType::attack, (Object *)obj1->getUnitInfo()->getEnemy(), obj2);
					
				}
				if (obj2->getObjectType() == ObjectType::bullet) {
					obj2->deleteObject();
					event_buffer.addEvent(EventType::attack, (Object *)obj2->getUnitInfo()->getEnemy(), obj1);
				}
				break;
			case attack:

				if (unit1 != nullptr && unit2 != nullptr) {
					obj2->getUnitInfo()->dealDamage(obj1->getUnitInfo()->getAttackDamage1());
					obj2->changeSpeed((obj2->getPosition() - obj1->getPosition()).getNormal() * consts.getKnockbackSpeed());
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
				objects[layer][i]->forceChangePosition(objects[layer][i]->getSpeed()/* * timer.getTimeDelay() / 1000*/);
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
				addObject(struct_arr[i], main_layer);
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
				faction = friendly_faction;
			}
			if (faction_score > 933) {
				faction = neutral_faction;
			}
			if (faction_score > 966) {
				faction = aggressive_faction;
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
				addObject(struct_arr[i], main_layer);
				object->attachObject(struct_arr[i]);
			}
		}
	}

public:

	Map() {}

	Map(std::string path) : objects(parseMap(path)) {
		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				if (objects[layer][i]->getObjectType() == ObjectType::hero) {
					hero = objects[layer][i];
				}
			}
		}
		createNavigationGrid();
		rebuildMap(hero->getPosition(), gen_radius, 0, save_out_range, 1, asteroid_amount, min_range, max_range);
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
		processUnitAI();
		processEventBuffer();
		garbageCollector();

		if (hero == nullptr) {
			return;
		}

		rebuildMap(hero->getPosition(), gen_radius, cam_radius, save_out_range, 1, asteroid_amount, min_range, max_range);
	}

	Object * getHero() {
		return hero;
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
		if ((closest_asteroid && ((hero->getPosition() - closest_asteroid->getPosition()).getLength() > consts.getInteractionDistance())) || !closest_asteroid) {
			for (int i = 0; i < objects[landscape_layer].size(); i++) {
				if (objects[0][i]->getObjectType() == ObjectType::asteroid) {
					if (((hero->getPosition() - objects[landscape_layer][i]->getPosition()).getLength() < consts.getInteractionDistance())) {
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
};