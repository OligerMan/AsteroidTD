#include "VisualController.h"
#include "GUIVisualController.h"
#include "GUIManager.h"
#include "ResourceManager.h"

#include <chrono>
#include <Windows.h>

enum XBOXGamepadButtons {
	A,B,X,Y,LB,RB,BACK,START,LSTICK,RSTICK
};

void gameCycle(std::string map_name) {
	collision_type_init();
	object_type_init();
	event_type_init();
	sprite_type_init();
	animation_type_init();
	faction_type_init();

	srand(time(NULL));   // setting a magic number to random generation

	if (!settings.isLoaded()) {
		std::cout << "Settings file error, enabling default settings" << std::endl;
		settings.setDefaults();
	}

	sf::RenderWindow window(sf::VideoMode(settings.getWindowWidth(), settings.getWindowHeight()), "JustTest", sf::Style::None);
	sf::View view1(sf::Vector2f(0.0, 0.0), sf::Vector2f(settings.getWindowWidth() * 1.2, settings.getWindowHeight() * 1.2));
	sf::View view2(sf::Vector2f(0.0, 0.0), sf::Vector2f(settings.getWindowWidth() * 5, settings.getWindowHeight() * 5));
	bool strategic_view = false;    // choice from strategic camera(big vision, hero don't move) and hero camera(default camera)
	bool fight_mode = false;       // choice from build/peaceful mode(default) and fight mode
	int last_view_change = 0;    // number of frame where you changed view last time
	int last_mode_change = 0;    // number of frame where you changed mode last time
	int last_build = 0;    // number of frame where you changed mode last time

	window.setFramerateLimit(consts.getFPSLock());

	VisualController visual_ctrl;
	GUIVisualController gui_visual_ctrl;
	Map game_map1("maps/" + map_name + ".map");
	ResourceManager res_manager(1000, 5);

	Map object_presets("redactor_resources/object_templates.map");
	GUIManager gui_manager(object_presets.getObjectsBuffer());


	window.setView(view1);

	bool is_game_cycle = true;
	int frame_num = 0;

	Point previous_speed;

	while (window.isOpen())
	{
		frame_num++;

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		Point viewport_pos = Point(view1.getCenter().x, view1.getCenter().y);

		// game cycle

		// resource manager control
		res_manager.processFrame();
		if (!strategic_view) {
			gui_manager.setText(std::to_string((int)res_manager.getGold()), 0.01, gold_sign, Point(-settings.getWindowWidth() / 2, -settings.getWindowHeight() / 2), 30);
			gui_manager.setText(std::to_string((int)res_manager.getResearch()), 0.01, research_sign, Point(-settings.getWindowWidth() / 2, -settings.getWindowHeight() / 2 + 50), 30);
		}

		if (frame_num % 10000 == 0) {
			if (!strategic_view) {
				game_map1.spawnEnemy(frame_num / 10000, Point(view1.getCenter().x, view1.getCenter().y));
			}
			else {
				game_map1.spawnEnemy(frame_num / 10000, Point(view2.getCenter().x, view2.getCenter().y));
			}
		}

		if (is_game_cycle) {
			window.clear(sf::Color::Black);
			is_game_cycle = visual_ctrl.processFrame(&window, game_map1.getObjectsBuffer());
			is_game_cycle = gui_visual_ctrl.processFrame(&window, gui_manager.getObjectsBuffer(), gui_manager.getGUIText(), viewport_pos) && is_game_cycle;

			if (settings.isCollisionDebugMode()) {

				for (int x = 0; x < window.getSize().x; x+=3) {
					for (int y = 0; y < window.getSize().y; y+=3) {
						if (game_map1.isClickable(Point(x, y) + viewport_pos - Point(settings.getWindowWidth() / 2, settings.getWindowHeight() / 2))) {
							sf::Vertex line[] =
							{
								sf::Vertex(sf::Vector2f(x + viewport_pos.x - settings.getWindowWidth() / 2, y + viewport_pos.y - settings.getWindowHeight() / 2)),
								sf::Vertex(sf::Vector2f(x + viewport_pos.x - settings.getWindowWidth() / 2, y + viewport_pos.y + 1 - settings.getWindowHeight() / 2))
							};
							line->color = sf::Color::Red;

							window.draw(line, 2, sf::Lines);
						}
					}
				}
			}

			if (settings.isNavigationGridMode()) {
				// render navigation grid
				Point grid_offset = game_map1.getNavigationGridOffset();
				double grid_size = game_map1.getNavigationGridSize();

				for (int x = 0; x < window.getSize().x; x += 3) {
					for (int y = 0; y < window.getSize().y; y += 3) {
						Point grid_point = (Point(x, y) + viewport_pos - Point(settings.getWindowWidth() / 2, settings.getWindowHeight() / 2));
						if (game_map1.getNavigationGridElem((grid_point.x - grid_offset.x) / grid_size, (grid_point.y - grid_offset.y) / grid_size)) {
							sf::Vertex line[] =
							{
								sf::Vertex(sf::Vector2f(grid_point.x, grid_point.y)),
								sf::Vertex(sf::Vector2f(grid_point.x, grid_point.y + 1))
							};
							line->color = sf::Color::Green;

							window.draw(line, 2, sf::Lines);
						}
					}
				}

				// render navigation paths
				std::vector<std::vector<Point>> navigation_paths = game_map1.getNavigationPaths();
				for (int path_num = 0; path_num < navigation_paths.size(); path_num++) {
					for (int i = 1; i < navigation_paths[path_num].size(); i++) {
						sf::Vertex line[] =
						{
							sf::Vertex(sf::Vector2f(navigation_paths[path_num][i - 1].x, navigation_paths[path_num][i - 1].y)),
							sf::Vertex(sf::Vector2f(navigation_paths[path_num][i].x, navigation_paths[path_num][i].y))
						};
						line->color = sf::Color::Blue;

						window.draw(line, 2, sf::Lines);
					}
				}
			}
			
		}
		// input handling

		Point cursor_pos;
		sf::Vector2i mouse_pos = sf::Mouse::getPosition();
		sf::Vector2i window_pos = window.getPosition();
		cursor_pos = Point(mouse_pos.x - settings.getWindowWidth() / 2 - window.getPosition().x, mouse_pos.y - settings.getWindowHeight() / 2 - window.getPosition().y);

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			if (!gui_manager.processFrame(cursor_pos, viewport_pos)) {
				game_map1.processFrame(cursor_pos + viewport_pos);
			}
		}
		else {
			if (!gui_manager.processFrame(Point(10000000, 100000000), viewport_pos)) {
				game_map1.processFrame(Point(10000000, 100000000));
			}
		}

		if (game_map1.getHero() == nullptr) {
			// end game code
			break;
		}

		Event gui_event = gui_manager.getEvent();
		if (gui_event.getFirstObject() != nullptr) {
			switch (gui_event.getEventType()) {
			case create_new:
				game_map1.addObject(gui_event.getFirstObject(), 0);
				break;
			default:
				break;
			}
		}

		double hero_speed = game_map1.getHero()->getUnitInfo()->getDefaultSpeed();
		Object * hero_object = game_map1.getHero();
		
		Point new_speed;

		if (!strategic_view) {
			Object * closest_asteroid = game_map1.getClosestAsteroid();
			if (closest_asteroid != nullptr && frame_num > 1500) {
				switch (closest_asteroid->getUnitInfo()->getFaction()) {
				case null_faction:
					gui_manager.setTopSign("Empty", 0.01);
					break;
				case hero_faction:
					gui_manager.setTopSign("Your", 0.01);
					break;
				case friendly_faction:
					gui_manager.setTopSign("Friendly", 0.01);
					break;
				case neutral_faction:
					gui_manager.setTopSign("Neutral", 0.01);
					break;
				case aggressive_faction:
					gui_manager.setTopSign("Aggressive", 0.01);
					break;
				}
			}
		}
		
		if (hero_object != nullptr) {           // input
			if (sf::Joystick::isConnected(0)) {         // gamepad input

				new_speed = Point(
					sf::Joystick::getAxisPosition(0, sf::Joystick::X),
					sf::Joystick::getAxisPosition(0, sf::Joystick::Y));

				if (abs(new_speed.x) < 1 && abs(new_speed.y) < 1) {
					new_speed = Point();
				}
				else {
					hero_object->setAnimationType(move_anim);
				}

				int buttons_count = sf::Joystick::getButtonCount(0);
				for (int i = 0; i < buttons_count; i++) {
					if (sf::Joystick::isButtonPressed(0, i)) {
						if (settings.isGamepadDebugEnabled()) {
							std::cout << "Gamepad button number " << i << " is pressed" << std::endl;
						}
					}
				}
			}
			// keyboard input
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				new_speed += Point(0, -1);
				if (!strategic_view) {
					hero_object->setAnimationType(move_anim);
				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				new_speed += Point(-1, 0);
				if (!strategic_view) {
					hero_object->setAnimationType(move_anim);
				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				new_speed += Point(0, 1);
				if (!strategic_view) {
					hero_object->setAnimationType(move_anim);
				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				new_speed += Point(1, 0);
				if (!strategic_view) {
					hero_object->setAnimationType(move_anim);
				}
			}
			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Joystick::isButtonPressed(0, LB)) && (frame_num - last_view_change) > consts.getFPSLock() / 3 /* 0.33 sec delay for changing view again */) {
				if (!strategic_view) {
					strategic_view = true;
					last_view_change = frame_num;
					window.setView(view2);
				}
				else{
					strategic_view = false;
					last_view_change = frame_num;
					window.setView(view1);
				}
			}
			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::E) || sf::Joystick::isButtonPressed(0, RB)) && (frame_num - last_mode_change) > consts.getFPSLock() / 3 /* 0.33 sec delay for changing view again */) {
				if (!fight_mode) {
					fight_mode = true;
				}
				else {
					fight_mode = false;
				}
				last_mode_change = frame_num;
			}
			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) || sf::Joystick::isButtonPressed(0, Y)) && (frame_num - last_build) > consts.getFPSLock() / 4 /* 0.25 sec delay for changing view again */) {
				if (fight_mode) {
					// hero attack 1 (mb rocket launch)
				}
				else {
					// build dome(on your or empty asteroid), or start conversation with others
					if (game_map1.getClosestAsteroid()) {
						int faction = game_map1.getClosestAsteroid()->getUnitInfo()->getFaction();
						switch (faction) {
						case null_faction:
						case hero_faction:
							if (res_manager.spendGold(consts.getBaseDomePrice())) {
								if (!game_map1.addStructure(game_map1.getClosestAsteroid(), dome)) {
									res_manager.addGold(consts.getBaseDomePrice());
								}
								game_map1.getClosestAsteroid()->setFaction(hero_faction);
							}

							break;
						case friendly_faction:
						case neutral_faction:
						case aggressive_faction:
							// start conversation
							break;
						}
						last_build = frame_num;
					}
					
				}
				
			}
			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) || sf::Joystick::isButtonPressed(0, X)) && (frame_num - last_build) > consts.getFPSLock() / 4 /* 0.25 sec delay for changing view again */) {
				if (fight_mode) {
					// hero attack 2 
				}
				else {
					if (game_map1.getClosestAsteroid()) {
						int faction = game_map1.getClosestAsteroid()->getUnitInfo()->getFaction();
						switch (faction) {
						case hero_faction:
							if (res_manager.spendGold(consts.getBaseTurretPrice())) {
								if (!game_map1.addStructure(game_map1.getClosestAsteroid(), turret)) {
									res_manager.addGold(consts.getBaseTurretPrice());
								}
								game_map1.getClosestAsteroid()->setFaction(hero_faction);
							}
							break;
						}
						last_build = frame_num;
					}
				}
			}
			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) || sf::Joystick::isButtonPressed(0, B)) && (frame_num - last_build) > consts.getFPSLock() / 4 /* 0.25 sec delay for changing view again */) {
				if (fight_mode) {
					// hero attack 2 
				}
				else {
					if (game_map1.getClosestAsteroid()) {
						int faction = game_map1.getClosestAsteroid()->getUnitInfo()->getFaction();
						switch (faction) {
						case hero_faction:
							if (res_manager.spendGold(consts.getBaseGoldPrice())) {
								if (!game_map1.addStructure(game_map1.getClosestAsteroid(), gold)) {
									res_manager.addGold(consts.getBaseGoldPrice());
								}
								else {
									res_manager.changeGoldIncome(consts.getBaseGoldIncome());
								}
								game_map1.getClosestAsteroid()->setFaction(hero_faction);
							}
							break;
						}
						last_build = frame_num;
					}
				}
			}
			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Num4) || sf::Joystick::isButtonPressed(0, A)) && (frame_num - last_build) > consts.getFPSLock() / 4 /* 0.25 sec delay for changing view again */) {
				if (fight_mode) {
					// hero attack 2 
				}
				else {
					if (game_map1.getClosestAsteroid()) {
						int faction = game_map1.getClosestAsteroid()->getUnitInfo()->getFaction();
						switch (faction) {
						case hero_faction:
							if (res_manager.spendGold(consts.getBaseSciencePrice())) {
								if (!game_map1.addStructure(game_map1.getClosestAsteroid(), science)) {
									res_manager.addGold(consts.getBaseSciencePrice());
								}
								else {
									res_manager.changeResearchIncome(consts.getBaseResearchIncome());
								}
								game_map1.getClosestAsteroid()->setFaction(hero_faction);
							}
							break;
						}
						last_build = frame_num;
					}
				}
			}
		}

		if (!strategic_view) {
			new_speed = new_speed.getNormal() * hero_speed;
			if (hero_object != nullptr) {
				Point old_pos = hero_object->getSpeed();
				old_pos -= previous_speed;
				hero_object->setSpeed(old_pos * (1 - consts.getFrictionCoef()));

				hero_object->changeSpeed(new_speed);
				previous_speed = new_speed;

				if (hero_object->getSpeed().getLength() != 0) {
					hero_object->setAngle(atan2(hero_object->getSpeed().y, hero_object->getSpeed().x) / PI * 180);
				}
				else {
					previous_speed = Point();
				}
				if (hero_object->getSpeed() == Point()) {
					hero_object->setAnimationType(hold_anim);
				}
			}
		}
		else {
			hero_object->setSpeed(Point());
			hero_object->setAnimationType(hold_anim);
			new_speed = new_speed.getNormal() * consts.getStrategicCameraSpeed();
			view2.setCenter(view2.getCenter() + sf::Vector2f(new_speed.x, new_speed.y));
		}
		

		if (hero_object != nullptr) {
			const double view_speed_coef = 0.012;    // must be from 0 to 1, where 0 for static camera and 1 for camera istantly over hero
			Point hero_position = hero_object->getPosition();
			Point diff = (hero_position - Point(view1.getCenter())) * view_speed_coef;
			view1.setCenter(view1.getCenter() + sf::Vector2f(diff.x, diff.y));
		}
		
		if (!strategic_view) {
			window.setView(view1);
		}
		else {
			window.setView(view2);
		}
		
		window.display();
	}
}

int main() {

	std::string input;

	//std::cout << "Enter map name" << std::endl;
	//std::cin >> input; // path to game map
	input = "map2";    // for tests
	gameCycle(input);

	return 0;
}