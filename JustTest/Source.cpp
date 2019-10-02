#include "VisualController.h"
#include "GUIVisualController.h"
#include "GUIManager.h"
#include "ResourceManager.h"
#include "FPS.h"
#include "GameStatus.h"
#include "Research.h"
#include "ResearchVisualController.h"

#include <chrono>
#include <Windows.h>
#include <Xinput.h>

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

	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(sf::VideoMode(settings.getWindowWidth(), settings.getWindowHeight()), "AsteroidTD", sf::Style::None);
	int screenW = GetSystemMetrics(SM_CXSCREEN);
	int screenH = GetSystemMetrics(SM_CYSCREEN);
	window.setSize(sf::Vector2u(screenW, screenH));
	window.setPosition(sf::Vector2i(0, 0));

	sf::View view1(sf::Vector2f(0.0, 0.0), sf::Vector2f(window.getSize().x * 1.2, window.getSize().y * 1.2));  // main game mode
	sf::View view2(sf::Vector2f(0.0, 0.0), sf::Vector2f(window.getSize().x * 5, window.getSize().y * 5));      // strategic view
	sf::View view3(sf::Vector2f(0.0, 0.0), sf::Vector2f(window.getSize().x, window.getSize().y));      // research view
	int last_view_change = 0;    // number of frame where you changed view last time
	int last_mode_change = 0;    // number of frame where you changed mode last time
	int last_build = 0;    // number of frame where you built smth last time
	int last_pause = 0;    // number of frame where you paused game last time
    int last_research_open = 0; // number of frame where you opened research list last time

	GameStatus prev_game_status = pause;

	window.setFramerateLimit(consts.getFPSLock());

	VisualController visual_ctrl;
	GUIVisualController gui_visual_ctrl(&window);
	ResearchVisualController res_visual_ctrl;
	Map game_map1("maps/" + map_name + ".map");

	resource_manager.clear(settings.getStartGold(), 5);
    research_manager.initResearch("research.cfg");
	research_manager.setGraphSize();
	std::vector<Research *> research_list = research_manager.getResearchArray();
	std::vector<ResearchNode *> research_graph = research_manager.getResearchGraph();

	Map object_presets("redactor_resources/object_templates.map");
	GUIManager gui_manager(object_presets.getObjectsBuffer(), window.getSize().y);

	sf::Texture main_background_texture, research_background_texture;
    main_background_texture.loadFromFile("background/main_background.png");
    research_background_texture.loadFromFile("background/research_background.png");

	sf::Sprite main_background_sprite, research_background_sprite;
    main_background_sprite.setTexture(main_background_texture);
    main_background_sprite.setOrigin(sf::Vector2f((int)main_background_texture.getSize().x / 2, (int)main_background_texture.getSize().y / 2));
    research_background_sprite.setTexture(research_background_texture);
    research_background_sprite.setOrigin(sf::Vector2f((int)research_background_texture.getSize().x / 2, (int)research_background_texture.getSize().y / 2));

	sf::Vector2f strategic_back_pos, main_back_pos;

	game_status = game_hero_mode;

	window.setView(view1);

	bool is_game_cycle = true;
	int frame_num = 0;

	Point previous_speed;

	int wave_delay = 10000;
	int next_wave = 10000;

	// for defining vibration on damage
	float hero_hp = 0;
	int vibration_time = 0;
	int vibr_on_dmg_time = 30;

	Point viewport_pos;

	while (window.isOpen())
	{

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		frame_num++;
		// FPS 
		fps.processFrame(frame_num);

		if (game_status == game_hero_mode || game_status == game_strategic_mode || game_status == pause) {

			
			if (!(game_status == game_strategic_mode || game_status == pause)) {
				main_background_sprite.setPosition(main_back_pos);
                main_background_sprite.setScale(sf::Vector2f(3, 3));
			}
			else {
                main_background_sprite.setPosition(strategic_back_pos);
                main_background_sprite.setScale(sf::Vector2f(8, 8));
			}

			if (game_status == game_hero_mode) {
				viewport_pos = Point(view1.getCenter().x, view1.getCenter().y);
			}
			else {
				viewport_pos = Point(view2.getCenter().x, view2.getCenter().y);
			}
			Object * hero_object = game_map1.getHero();

			if (hero_object->getUnitInfo()->getHealth() - hero_hp < -0.01) {
				vibration_time = vibr_on_dmg_time;
			}
			if (vibration_time > 0) {
				XINPUT_VIBRATION vibration;
				ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
				vibration.wLeftMotorSpeed = 0; // use any value between 0-65535 here
				vibration.wRightMotorSpeed = 65535; // use any value between 0-65535 here
				XInputSetState(0, &vibration);
			}
			else {
				XINPUT_VIBRATION vibration;
				ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
				vibration.wLeftMotorSpeed = 0; // use any value between 0-65535 here
				vibration.wRightMotorSpeed = 0; // use any value between 0-65535 here
				XInputSetState(0, &vibration);
			}
			hero_hp = hero_object->getUnitInfo()->getHealth();
			// game cycle

			// resource manager control
			if (game_status != pause) {
				resource_manager.processFrame();
            }
            else {
                gui_manager.forceSetTopSign("Pause", 0.01);
            }

			gui_manager.setText(std::to_string((int)resource_manager.getGold()), 0.01, gold_sign, Point(-(int)window.getSize().x / 2, -(int)window.getSize().y / 2), 30);
			gui_manager.setText(std::to_string((int)resource_manager.getResearch()), 0.01, research_sign, Point(-(int)window.getSize().x / 2, -(int)window.getSize().y / 2 + 50), 30);

			if (skills_mode == set1) {
				gui_manager.setText("Ability Mode", 0.01, skill_status_sign, Point(window.getSize().x / 2 - 150, -(int)window.getSize().y / 2), 50);
			}
			else {
				gui_manager.setText("Build Mode", 0.01, skill_status_sign, Point(window.getSize().x / 2 - 150, -(int)window.getSize().y / 2), 50);
			}

			if (game_status != pause) {
				if (frame_num == next_wave) {
					if (!(game_status == game_strategic_mode)) {
						game_map1.spawnEnemy(frame_num / 8000, Point(view1.getCenter().x, view1.getCenter().y));
					}
					else {
						game_map1.spawnEnemy(frame_num / 8000, Point(view2.getCenter().x, view2.getCenter().y));
					}
					gui_manager.forceSetTopSign("New Wave", 5);
					next_wave += wave_delay;
					wave_delay += 200;
				}
			}
			
			if (is_game_cycle) {
				// set background
				window.draw(main_background_sprite);


				is_game_cycle = visual_ctrl.processFrame(&window, game_map1.getObjectsBuffer());
				float hero_hp = hero_object->getUnitInfo()->getHealth() / hero_object->getUnitInfo()->getMaxHealth();

				is_game_cycle = gui_visual_ctrl.processFrame(&window, gui_manager.getObjectsBuffer(), gui_manager.getGUIText(), viewport_pos, hero_hp) && is_game_cycle;

				if (settings.isCollisionDebugMode()) {

					for (int x = 0; x < window.getSize().x; x += 3) {
						for (int y = 0; y < window.getSize().y; y += 3) {
							if (game_map1.isClickable(Point(x, y) + viewport_pos - Point(window.getSize().x / 2, window.getSize().y / 2))) {
								sf::Vertex line[] =
								{
									sf::Vertex(sf::Vector2f(x + viewport_pos.x - window.getSize().x / 2, y + viewport_pos.y - window.getSize().y / 2)),
									sf::Vertex(sf::Vector2f(x + viewport_pos.x - window.getSize().x / 2, y + viewport_pos.y + 1 - window.getSize().y / 2))
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
							Point grid_point = (Point(x, y) + viewport_pos - Point(window.getSize().x / 2, window.getSize().y / 2));
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
			cursor_pos = Point(mouse_pos.x - window.getSize().x / 2 - window.getPosition().x, mouse_pos.y - window.getSize().y / 2 - window.getPosition().y);

			if (game_status != pause) {
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
			}
			

			if (game_map1.getHero() == nullptr) {
				// end game code
				game_status = game_over;
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

			double hero_speed = hero_object->getUnitInfo()->getDefaultSpeed() * consts.getFPSLock() / fps.getFPS();

			Point new_speed;

			if (!(game_status == game_strategic_mode)) {
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

					if (abs(new_speed.x) < 5 && abs(new_speed.y) < 5) {
						new_speed = Point();
					}
					else {
						if (game_status == game_hero_mode) {
							hero_object->setAnimationType(move_anim);
						}
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
					if (!(game_status == game_strategic_mode)) {
						hero_object->setAnimationType(move_anim);
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
					new_speed += Point(-1, 0);
					if (!(game_status == game_strategic_mode)) {
						hero_object->setAnimationType(move_anim);
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
					new_speed += Point(0, 1);
					if (!(game_status == game_strategic_mode)) {
						hero_object->setAnimationType(move_anim);
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
					new_speed += Point(1, 0);
					if (!(game_status == game_strategic_mode)) {
						hero_object->setAnimationType(move_anim);
					}
				}

				if (game_status != pause) {
					if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Joystick::isButtonPressed(0, LB)) && (frame_num - last_view_change) > consts.getFPSLock() / 3 /* 0.33 sec delay for changing view again */) {
						if (!(game_status == game_strategic_mode)) {
							game_status = game_strategic_mode;
							last_view_change = frame_num;
							window.setView(view2);
						}
						else {
							game_status = game_hero_mode;
							last_view_change = frame_num;
							window.setView(view1);
						}
					}
					if ((sf::Keyboard::isKeyPressed(sf::Keyboard::E) || sf::Joystick::isButtonPressed(0, RB)) && (frame_num - last_mode_change) > consts.getFPSLock() / 3 /* 0.33 sec delay for changing view again */) {
						if (skills_mode == set1) {
							skills_mode = set2;
						}
						else {
							skills_mode = set1;
						}
						last_mode_change = frame_num;
						last_build = frame_num - consts.getFPSLock();
					}
					if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) || sf::Joystick::isButtonPressed(0, Y)) && (frame_num - last_build) > consts.getFPSLock() / 4 /* 0.25 sec delay for changing view again */) {
						if (skills_mode == set1) {
							// hero attack 1 (mb rocket launch)
						}
						else {
							// build dome(on your or empty asteroid), or start conversation with others
							if (game_map1.getClosestAsteroid()) {
								int faction = game_map1.getClosestAsteroid()->getUnitInfo()->getFaction();
								switch (faction) {
								case null_faction:
								case hero_faction:
									if (resource_manager.spendGold(consts.getBaseDomePrice())) {
										if (!game_map1.addStructure(game_map1.getClosestAsteroid(), dome)) {
											resource_manager.addGold(consts.getBaseDomePrice());
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
						if (skills_mode == set1) {
							// hero speed increase
							if (resource_manager.spendGold(consts.getSpeedBuffPrice())) {
								hero_object->setEffect(Effect(1200, move_speed_buff));
								last_build = frame_num + consts.getFPSLock();
							}
						}
						else {
							if (game_map1.getClosestAsteroid()) {
								int faction = game_map1.getClosestAsteroid()->getUnitInfo()->getFaction();
								switch (faction) {
								case hero_faction:
									if (resource_manager.spendGold(consts.getBaseTurretPrice())) {
										if (!game_map1.addStructure(game_map1.getClosestAsteroid(), turret)) {
											resource_manager.addGold(consts.getBaseTurretPrice());
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
						if (skills_mode == set1) {
							// attack buff for asteroid
							if (game_map1.getClosestAsteroid() != nullptr) {
								if (resource_manager.spendGold(consts.getDamageBuffPrice())) {
									game_map1.setAsteroidBuff(Effect(1200, EffectList::attack_speed_buff), game_map1.getClosestAsteroid());
									game_map1.setAsteroidBuff(Effect(1200, EffectList::damage_buff), game_map1.getClosestAsteroid());
									last_build = frame_num + consts.getFPSLock();
								}
							}
						}
						else {
							if (game_map1.getClosestAsteroid()) {
								int faction = game_map1.getClosestAsteroid()->getUnitInfo()->getFaction();
								switch (faction) {
								case hero_faction:
									if (resource_manager.spendGold(consts.getBaseGoldPrice())) {
										if (!game_map1.addStructure(game_map1.getClosestAsteroid(), gold)) {
											resource_manager.addGold(consts.getBaseGoldPrice());
										}
										else {
											resource_manager.changeGoldIncome(consts.getBaseGoldIncome());
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
						if (skills_mode == set1) {
							// heal for asteroid and self
							if (game_map1.getClosestAsteroid() != nullptr) {
								if (resource_manager.spendGold(consts.getHealBuffPrice())) {
									game_map1.setAsteroidBuff(Effect(1, const_heal), game_map1.getClosestAsteroid());
									game_map1.setAsteroidBuff(Effect(1200, regen_buff), game_map1.getClosestAsteroid());
								}
							}
							hero_object->setEffect(Effect(1, const_heal));
							hero_object->setEffect(Effect(1200, regen_buff));
							last_build = frame_num + consts.getFPSLock();
						}
						else {
							if (game_map1.getClosestAsteroid()) {
								int faction = game_map1.getClosestAsteroid()->getUnitInfo()->getFaction();
								switch (faction) {
								case hero_faction:
									if (resource_manager.spendGold(consts.getBaseSciencePrice())) {
										if (!game_map1.addStructure(game_map1.getClosestAsteroid(), science)) {
											resource_manager.addGold(consts.getBaseSciencePrice());
										}
										else {
											resource_manager.changeResearchIncome(consts.getBaseResearchIncome());
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
				if ((sf::Keyboard::isKeyPressed(sf::Keyboard::R) || sf::Joystick::isButtonPressed(0, START)) && (frame_num - last_pause) > consts.getFPSLock() / 4) {
					if (game_status != pause) {
						prev_game_status = game_status;
						game_status = pause;
						last_pause = frame_num;
						window.setView(view2);
					}
					else {
						game_status = prev_game_status;
						prev_game_status = pause;
						last_pause = frame_num;
						if (game_status == game_hero_mode) {
							window.setView(view1);
						}
						else {
							window.setView(view2);
						}
					}
				}
				if ((sf::Keyboard::isKeyPressed(sf::Keyboard::F) || sf::Joystick::isButtonPressed(0, BACK)) && (frame_num - last_research_open) > consts.getFPSLock() / 4 /* 0.25 sec delay for changing view again */) {
					last_research_open = frame_num;
					prev_game_status = game_status;
					game_status = research;
				}
			}

			if (!(game_status == game_strategic_mode || game_status == pause)) {
				if (hero_object->getUnitInfo()->isAffected(move_speed_buff)) {
					hero_speed *= consts.getMoveSpeedBuffMultiplier();
				}
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
					new_speed = new_speed.getNormal() * consts.getStrategicCameraSpeed() * consts.getFPSLock() / fps.getFPS();
					view2.setCenter(view2.getCenter() + sf::Vector2f(new_speed.x, new_speed.y));
					strategic_back_pos = strategic_back_pos + sf::Vector2f(new_speed.x * 0.95, new_speed.y * 0.95);
				
			}


			if (hero_object != nullptr) {
				const double view_speed_coef = 0.012;    // must be from 0 to 1, where 0 for static camera and 1 for camera istantly over hero
				Point hero_position = hero_object->getPosition();
				Point diff = (hero_position - Point(view1.getCenter())) * view_speed_coef * consts.getFPSLock() / fps.getFPS();
				view1.setCenter(view1.getCenter() + sf::Vector2f(diff.x, diff.y));
				main_back_pos = main_back_pos + sf::Vector2f(diff.x * 0.95, diff.y * 0.95);
			}

			if (!(game_status == game_strategic_mode || game_status == pause)) {
				window.setView(view1);
			}
			else {
				window.setView(view2);
			}
		}
		if (game_status == game_over) {
            return;
		}
        if (game_status == research) {

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::F) || sf::Joystick::isButtonPressed(0, BACK)) && (frame_num - last_research_open) > consts.getFPSLock() / 4 /* 0.25 sec delay for changing view again */) {
                last_research_open = frame_num;
                game_status = prev_game_status;
				prev_game_status = research;
            }

            research_background_sprite.setScale(window.getSize().x / research_background_texture.getSize().x, window.getSize().y / research_background_texture.getSize().y);
            research_background_sprite.setPosition(0, 0);

			view3.setCenter(0, 0);
			window.setView(view3);
            window.clear(sf::Color::Black);

			res_visual_ctrl.processFrame(&window);
        }


		vibration_time--;

		window.display();
		
	}
}

int main() {

	HWND hWnd = GetConsoleWindow();
	//ShowWindow(hWnd, SW_HIDE);

	std::string input;

	//std::cout << "Enter map name" << std::endl;
	//std::cin >> input; // path to game map
	input = "map2";    // for tests
	gameCycle(input);

	return 0;
}