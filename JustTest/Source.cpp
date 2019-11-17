#include "VisualController.h"
#include "GUIVisualController.h"
#include "GUIManager.h"
#include "ResourceManager.h"
#include "FPS.h"
#include "GameStatus.h"
#include "Research.h"
#include "ResearchVisualController.h"
#include "Tutorial.h"
#include "OnlineRank.h"
#include "KeyboardBuffer.h"

#include <chrono>
#include <Windows.h>
#include <Xinput.h>
#include <SFML\Audio.hpp>

enum XBOXGamepadButtons {
	A,B,X,Y,LB,RB,BACK,START,LSTICK,RSTICK
};


void gameCycle(std::string map_name, sf::RenderWindow & window, VisualController & visual_ctrl, GUIVisualController & gui_visual_ctrl, ResearchVisualController & res_visual_ctrl) {


	sf::Texture main_background_texture, research_background_texture, game_over_background_texture;
	sf::Sprite main_background_sprite, research_background_sprite, game_over_background_sprite;


	main_background_texture.loadFromFile("background/main_background.png");
	research_background_texture.loadFromFile("background/research_background.png");

	main_background_sprite.setTexture(main_background_texture);
	main_background_sprite.setOrigin(sf::Vector2f((int)main_background_texture.getSize().x / 2, (int)main_background_texture.getSize().y / 2));
	research_background_sprite.setTexture(research_background_texture);
	research_background_sprite.setOrigin(sf::Vector2f((int)research_background_texture.getSize().x / 2, (int)research_background_texture.getSize().y / 2));

	game_over_background_texture.loadFromFile("background/game_over_background.png");
	game_over_background_sprite.setTexture(game_over_background_texture);
	game_over_background_sprite.setPosition(0, 0);
	game_over_background_sprite.setScale(sf::Vector2f(window.getSize().x / game_over_background_texture.getSize().x, window.getSize().y / game_over_background_texture.getSize().y));

	sf::Text title;
	sf::Font base_font;

	sf::View view1(sf::Vector2f(0.0, 0.0), sf::Vector2f(window.getSize().x * 1.2, window.getSize().y * 1.2));  // main game mode
	sf::View view2(sf::Vector2f(0.0, 0.0), sf::Vector2f(window.getSize().x * 5, window.getSize().y * 5));      // strategic view
	sf::View view3(sf::Vector2f(0.0, 0.0), sf::Vector2f(window.getSize().x, window.getSize().y));      // research view
	sf::View view4(sf::Vector2f((int)window.getSize().x / 2, (int)window.getSize().y / 2), sf::Vector2f(window.getSize().x, window.getSize().y));      // main menu view
	
	int last_view_change = 0;    // number of frame when you changed view last time
	int last_mode_change = 0;    // number of frame when you changed mode last time
	int last_build = 0;    // number of frame when you built smth last time
	int last_pause = 0;    // number of frame when you paused game last time
    int last_research_open = 0; // number of frame when you opened research list last time
	int last_research_choice = 0; // number of frame when you chose another research last time
	int last_menu_choice = 0;
	int last_tutorial = 0;

	GameStatus prev_game_status = pause;

	Map game_map1("maps/" + map_name + ".map");

	resource_manager.clear(settings.getStartGold(), 5);
    research_manager.initResearch("research.cfg");
	research_manager.setGraphSize();
	std::vector<Research *> research_list = research_manager.getResearchArray();
	std::vector<ResearchNode *> research_graph = research_manager.getResearchGraph();
	int cur_research_index = 0;

	GUIManager gui_manager(window.getSize().y);

	sf::Vector2f strategic_back_pos, main_back_pos;

	game_status = game_hero_mode;
	skills_mode = set2;

	window.setView(view1);

	bool is_game_cycle = true;
	int frame_num = 0;
	int game_frame_num = 0;

	Point previous_speed;

	int wave_delay = 40;
	int wave_count = 0;
	std::chrono::time_point<std::chrono::steady_clock> last_wave = std::chrono::steady_clock::now();

	rank.resetUserInfo();
	std::chrono::time_point<std::chrono::steady_clock> round_start = std::chrono::steady_clock::now();


	auto time_check = [&round_start]() {
		while (true) {
			if (game_status == game_hero_mode || game_status == game_strategic_mode) {
				rank.addGameplayTime(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - round_start).count());
				round_start = std::chrono::steady_clock::now();
				Sleep(1000);
			}
		}
	};

	std::thread thread(time_check);
	thread.detach();


	// for defining vibration on damage
	float hero_hp = 0;
	int vibration_time = 0;
	int vibr_on_dmg_time = 30;

	// movement tutorial counter
	float move_points_left = 1500;
	int switching_view_counter = 2;
	int switching_modes_counter = 3;
	int research_number = 0;

	Point viewport_pos;

	title.setPosition(sf::Vector2f(window.getSize().x / 2, 150));
	title.setFillColor(sf::Color::White);
	title.setOutlineColor(sf::Color::Black);
	title.setOutlineThickness(1);
	title.setCharacterSize(50);
	title.setFont(base_font);

	struct Button {
		Point pos;
		sf::Texture texture_default, texture_selected;
		sf::Sprite sprite;
		float radius = 75;
		std::string advice_string;
	};
	enum buttons_name {
		retry,
		menu,
		pause_continue,
		pause_to_menu,
		pause_to_desktop,

		BUTTONS_NAME_LIST_SIZE
	};
	std::vector<Button> buttons;
	buttons.resize(BUTTONS_NAME_LIST_SIZE);

	buttons[retry].pos = Point(window.getSize().x / 2, 300);
	buttons[retry].texture_default.loadFromFile("menu_buttons\\retry.png");
	buttons[retry].texture_selected.loadFromFile("menu_buttons\\retry_selected.png");
	buttons[retry].sprite.setTexture(buttons[retry].texture_default);
	buttons[retry].advice_string = "try again";

	buttons[menu].pos = Point(window.getSize().x / 2, 500);
	buttons[menu].texture_default.loadFromFile("menu_buttons\\menu.png");
	buttons[menu].texture_selected.loadFromFile("menu_buttons\\menu_selected.png");
	buttons[menu].sprite.setTexture(buttons[menu].texture_default);
	buttons[menu].advice_string = "go back to menu";

	buttons[pause_continue].pos = Point(0, -1000);
	buttons[pause_continue].texture_default.loadFromFile("menu_buttons\\pause_continue.png");
	buttons[pause_continue].texture_selected.loadFromFile("menu_buttons\\pause_continue_selected.png");
	buttons[pause_continue].sprite.setTexture(buttons[pause_continue].texture_default);
	buttons[pause_continue].advice_string = "continue game";

	buttons[pause_to_menu].pos = Point();
	buttons[pause_to_menu].texture_default.loadFromFile("menu_buttons\\pause_to_menu.png");
	buttons[pause_to_menu].texture_selected.loadFromFile("menu_buttons\\pause_to_menu_selected.png");
	buttons[pause_to_menu].sprite.setTexture(buttons[pause_to_menu].texture_default);
	buttons[pause_to_menu].advice_string = "go to menu";

	buttons[pause_to_desktop].pos = Point(0, 1000);
	buttons[pause_to_desktop].texture_default.loadFromFile("menu_buttons\\pause_to_desktop.png");
	buttons[pause_to_desktop].texture_selected.loadFromFile("menu_buttons\\pause_to_desktop_selected.png");
	buttons[pause_to_desktop].sprite.setTexture(buttons[pause_to_desktop].texture_default);
	buttons[pause_to_desktop].advice_string = "go to desktop";

	int chosen_button = retry;

	auto pause_game = [&]() {
		prev_game_status = game_status;
		game_status = pause;
		last_pause = frame_num;
		window.setView(view2);
		rank.addGameplayTime(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - round_start).count());
		if (tutorial.isWorkingOnStep(tutorial.pause_tutorial)) {
			tutorial.nextStep();
		}
		chosen_button = pause_continue;
	};

	auto unpause_game = [&]() {
		game_status = prev_game_status;
		if (game_status == research) {
			game_status = game_hero_mode;
		}
		prev_game_status = pause;
		round_start = std::chrono::steady_clock::now();
		if (game_status == game_hero_mode) {
			window.setView(view1);
		}
		else {
			window.setView(view2);
		}
		if (tutorial.isWorkingOnStep(tutorial.unpause_tutorial)) {
			tutorial.nextStep();
		}
	};

	while (window.isOpen())
	{

		frame_num++;
		// FPS 
		fps.processFrame(frame_num);

		if (game_status == game_hero_mode || game_status == game_strategic_mode || game_status == pause) {
			
			if (tutorial.isWorkingOnStep(tutorial.no_tutorial)) {
				game_frame_num++;
			}
			
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

			gui_manager.setText("Gold: " + std::to_string((int)resource_manager.getGold()), 0.01, gold_sign, Point(-(int)window.getSize().x / 2, -(int)window.getSize().y / 2), 35);
			gui_manager.setText("Research: " + std::to_string((int)resource_manager.getResearch()), 0.01, research_sign, Point(-(int)window.getSize().x / 2, -(int)window.getSize().y / 2 + 55), 35);

			if (skills_mode == set1) {
				gui_manager.setText("Ability Mode", 0.01, skill_status_sign, Point(window.getSize().x / 2 - 150, -(int)window.getSize().y / 2), 50);
			}
			else {
				gui_manager.setText("Build Mode", 0.01, skill_status_sign, Point(window.getSize().x / 2 - 150, -(int)window.getSize().y / 2), 50);
			}

			if (game_status != pause && tutorial.isWorkingOnStep(tutorial.no_tutorial)) {
				if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_wave).count() > wave_delay) {
					wave_count++;
					if (!(game_status == game_strategic_mode)) {
						game_map1.spawnEnemy(wave_count, Point(view1.getCenter().x, view1.getCenter().y));
					}
					else {
						game_map1.spawnEnemy(wave_count, Point(view2.getCenter().x, view2.getCenter().y));
					}
					gui_manager.forceSetTopSign("New Wave", 5);
					last_wave = std::chrono::steady_clock::now();
					wave_delay += 1;
				}
			}

			// tutorial code
			if (tutorial.getCurrentStep() != tutorial.no_tutorial) {
				if (tutorial.isWorkingOnStep(tutorial.base_description)) {
					gui_manager.forceSetTopSign("This tutorial will show you\nsome base mechanics of\nthis game\n(press " + (sf::Joystick::isConnected(0) ? std::string("A") : std::string("Space")) + " for next step)", 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.movement_tutorial)) {
					gui_manager.forceSetTopSign("Try to move with WASD keys\nor left gamepad stick\n(" + std::to_string((int)move_points_left) + " pts left)", 0.01);
					move_points_left -= hero_object->getSpeed().getLength();
					if (move_points_left < 0) {
						tutorial.nextStep();
					}
				}
				if (tutorial.isWorkingOnStep(tutorial.switching_view_tutorial)) {
					gui_manager.forceSetTopSign("OK! Now let's try to change\ncamera mode with Q keyboard key\nor LB gamepad key\n(" + std::to_string(switching_view_counter) + " times left)", 0.01);
					if (switching_view_counter == 0) {
						tutorial.nextStep();
					}
				}

				if (tutorial.isWorkingOnStep(tutorial.build_mode_dome_tutorial)) {
					gui_manager.forceSetTopSign("Fly to the nearest asteroid\nand build dome on it\nwith key 1 on keyboard\nor Y on gamepad", 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.build_mode_dome_description_tutorial)) {
					gui_manager.forceSetTopSign("Dome grants some regeneration\nfor all structures on same asteroid\n(Space or A to proceed)", 0.01);
				}

				if (tutorial.isWorkingOnStep(tutorial.build_mode_turret_tutorial)) {
					gui_manager.forceSetTopSign("Fly to the nearest asteroid\nand build turret on it\nwith key 2 on keyboard\nor X on gamepad", 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.build_mode_turret_description_tutorial)) {
					gui_manager.forceSetTopSign("Turret is full-autonomous static\ndefence system for asteroid\nwith one minus - range so\nyou need more to defend your structures\n(Space or A to proceed)", 0.01);
				}

				if (tutorial.isWorkingOnStep(tutorial.build_mode_gold_tutorial)) {
					gui_manager.forceSetTopSign("Fly to the nearest asteroid\nand build gold mine on it\nwith key 3 on keyboard\nor B on gamepad", 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.build_mode_gold_description_tutorial)) {
					gui_manager.forceSetTopSign("You need gold mines to extract\nfrom asteroid all you can sell for gold\n(Space or A to proceed)", 0.01);
				}

				if (tutorial.isWorkingOnStep(tutorial.build_mode_science_tutorial)) {
					gui_manager.forceSetTopSign("Fly to the nearest asteroid\nand build science station on it\nwith key 4 on keyboard\nor A on gamepad", 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.build_mode_science_description_tutorial)) {
					gui_manager.forceSetTopSign("You need science stations to\nget some research points\n(about it slightly further,\nSpace or A to proceed)", 0.01);
				}

				if (tutorial.isWorkingOnStep(tutorial.switching_modes_tutorial)) {
					gui_manager.forceSetTopSign("OK! Now let's try to change\nbuild mode to skills mode with E keyboard key\nor RB gamepad key\n(" + std::to_string(switching_modes_counter) + " times left)", 0.01);
					if (switching_modes_counter == 0) {
						tutorial.nextStep();
					}
				}

				if (tutorial.isWorkingOnStep(tutorial.using_skills_rocket_tutorial)) {
					gui_manager.forceSetTopSign("This was assumed as a damage skill with low cost,\nbut I still don't have enough\ntime to create it\n(1 key on keyboard or Y on gamepad to try)", 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.using_skills_speed_tutorial)) {
					gui_manager.forceSetTopSign("This just your speed buff\n(2 key on keyboard or X on gamepad to try)", 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.using_skills_damage_tutorial)) {
					gui_manager.forceSetTopSign("Doubles damage and attack speed\non all turrets on asteroid\nnext to your ship\n(3 key on keyboard or B on gamepad to try)", 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.using_skills_heal_tutorial)) {
					gui_manager.forceSetTopSign("Heals you and all structures\non asteroids next to your ship\n(4 key on keyboard or A on gamepad to try)", 0.01);
				}

				if (tutorial.isWorkingOnStep(tutorial.research_tutorial)) {
					gui_manager.forceSetTopSign("Press F on keyboard or BACK on gamepad\nto open research list", 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.pause_tutorial)) {
					gui_manager.forceSetTopSign("Press R to pause the game", 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.unpause_tutorial)) {
					gui_manager.forceSetTopSign("Game paused! Press R to unpause the game", 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.tutorial_end)) {
					gui_manager.forceSetTopSign("Now you know all game basics.\nPress Space or A to start new game. GL HF!", 0.01);
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
				rank.selfRankUpdate(false);
				chosen_button = retry;
				continue;
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

			double hero_speed = hero_object->getUnitInfo()->getDefaultSpeed();

			Point new_speed;

			if (!(game_status == game_strategic_mode)) {
				Object * closest_asteroid = game_map1.getClosestAsteroid();
				if (closest_asteroid != nullptr && game_frame_num > 1500) {
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
					if (((sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Joystick::isButtonPressed(0, LB)) && (frame_num - last_view_change) > fps.getFPS() / 3 /* 0.33 sec delay for changing view again */) && tutorial.isWorkingOnStep(tutorial.switching_view_tutorial)) {
						if (!(game_status == game_strategic_mode)) {
							game_status = game_strategic_mode;
							last_view_change = frame_num;
							window.setView(view2);
							if (tutorial.isWorkingOnStep(tutorial.switching_view_tutorial)) {
								switching_view_counter--;
							}
						}
						else {
							game_status = game_hero_mode;
							last_view_change = frame_num;
							window.setView(view1);
							if (tutorial.isWorkingOnStep(tutorial.switching_view_tutorial)) {
								switching_view_counter--;
							}
						}
					}
					if (((sf::Keyboard::isKeyPressed(sf::Keyboard::E) || sf::Joystick::isButtonPressed(0, RB)) && (frame_num - last_mode_change) > fps.getFPS() / 3 /* 0.33 sec delay for changing view again */) && tutorial.isWorkingOnStep(tutorial.switching_modes_tutorial)) {
						if (skills_mode == set1) {
							skills_mode = set2;
							switching_modes_counter--;
						}
						else {
							skills_mode = set1;
							switching_modes_counter--;
						}
						last_mode_change = frame_num;
						last_build = frame_num - consts.getFPSLock();
					}
					if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) || sf::Joystick::isButtonPressed(0, Y)) && (frame_num - last_build) > fps.getFPS() / 4 /* 0.25 sec delay for changing view again */ && (tutorial.isWorkingOnStep(tutorial.build_mode_dome_tutorial) || tutorial.isWorkingOnStep(tutorial.using_skills_rocket_tutorial))) {
						if (skills_mode == set1) {
							// hero attack 1 (mb rocket launch)
							if (tutorial.isWorkingOnStep(tutorial.using_skills_rocket_tutorial)) {
								tutorial.nextStep();
							}
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
										if (tutorial.isWorkingOnStep(tutorial.build_mode_dome_tutorial)) {
											tutorial.nextStep();
										}
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
					if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) || sf::Joystick::isButtonPressed(0, X)) && (frame_num - last_build) > fps.getFPS() / 4 /* 0.25 sec delay for changing view again */ && (tutorial.isWorkingOnStep(tutorial.build_mode_turret_tutorial) || tutorial.isWorkingOnStep(tutorial.using_skills_speed_tutorial))) {
						if (skills_mode == set1) {
							// hero speed increase
							if (resource_manager.spendGold(consts.getSpeedBuffPrice())) {
								hero_object->setEffect(Effect(1200, move_speed_buff));
								last_build = frame_num + consts.getFPSLock();
								if (tutorial.isWorkingOnStep(tutorial.using_skills_speed_tutorial)) {
									tutorial.nextStep();
								}
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
										if (tutorial.isWorkingOnStep(tutorial.build_mode_turret_tutorial)) {
											tutorial.nextStep();
										}
									}
									break;
								}
								last_build = frame_num;
							}
						}
					}
					if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) || sf::Joystick::isButtonPressed(0, B)) && (frame_num - last_build) > fps.getFPS() / 4 /* 0.25 sec delay for changing view again */ && (tutorial.isWorkingOnStep(tutorial.build_mode_gold_tutorial) || tutorial.isWorkingOnStep(tutorial.using_skills_damage_tutorial))) {
						if (skills_mode == set1) {
							// attack buff for asteroid
							if (game_map1.getClosestAsteroid() != nullptr) {
								if (resource_manager.spendGold(consts.getDamageBuffPrice())) {
									game_map1.setAsteroidBuff(Effect(1200, EffectList::attack_speed_buff), game_map1.getClosestAsteroid());
									game_map1.setAsteroidBuff(Effect(1200, EffectList::damage_buff), game_map1.getClosestAsteroid());
									last_build = frame_num + consts.getFPSLock();
									if (tutorial.isWorkingOnStep(tutorial.using_skills_damage_tutorial)) {
										tutorial.nextStep();
									}
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
										game_map1.getClosestAsteroid()->setFaction(hero_faction);
										if (tutorial.isWorkingOnStep(tutorial.build_mode_gold_tutorial)) {
											tutorial.nextStep();
										}
									}
									break;
								}
								last_build = frame_num;
							}
						}
					}
					if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Num4) || sf::Joystick::isButtonPressed(0, A)) && (frame_num - last_build) > fps.getFPS() / 4 /* 0.25 sec delay for changing view again */ && (tutorial.isWorkingOnStep(tutorial.build_mode_science_tutorial) || tutorial.isWorkingOnStep(tutorial.using_skills_heal_tutorial))) {
						if (skills_mode == set1) {
							// heal for asteroid and self
							if (game_map1.getClosestAsteroid() != nullptr) {
								if (resource_manager.spendGold(consts.getHealBuffPrice())) {
									game_map1.setAsteroidBuff(Effect(1, const_heal), game_map1.getClosestAsteroid());
									game_map1.setAsteroidBuff(Effect(1200, regen_buff), game_map1.getClosestAsteroid());
									if (tutorial.isWorkingOnStep(tutorial.using_skills_heal_tutorial)) {
										tutorial.nextStep();
									}
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
										game_map1.getClosestAsteroid()->setFaction(hero_faction);
										if (tutorial.isWorkingOnStep(tutorial.build_mode_science_tutorial)) {
											tutorial.nextStep();
										}
									}
									break;
								}
								last_build = frame_num;
							}
						}
					}
					if ((sf::Keyboard::isKeyPressed(sf::Keyboard::F) || sf::Joystick::isButtonPressed(0, BACK)) && (frame_num - last_research_open) > fps.getFPS() / 4 /* 0.25 sec delay for changing view again */ && tutorial.isWorkingOnStep(tutorial.research_tutorial)) {
						last_research_open = frame_num;
						prev_game_status = game_status;
						game_status = research; rank.addGameplayTime(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - round_start).count());
						if (tutorial.isWorkingOnStep(tutorial.research_tutorial)) {
							tutorial.nextStep();
						}
					}
				}
				if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || sf::Joystick::isButtonPressed(0, START)) && (frame_num - last_pause) > fps.getFPS() / 4 && (tutorial.isWorkingOnStep(tutorial.pause_tutorial) || tutorial.isWorkingOnStep(tutorial.unpause_tutorial))) {
					if (game_status != pause) {
						pause_game();
					}
				}
				

				if (tutorial.getCurrentStep() != tutorial.no_tutorial) {
					if (tutorial.isWorkingOnStep(tutorial.base_description) || tutorial.isWorkingOnStep(tutorial.build_mode_dome_description_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_turret_description_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_gold_description_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_science_description_tutorial) || tutorial.isWorkingOnStep(tutorial.tutorial_end)) {
						if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Joystick::isButtonPressed(0, A)) && (frame_num - last_tutorial) > fps.getFPS() / 4 /* 0.25 sec delay for changing view again */) {
							last_tutorial = frame_num;
							if (tutorial.getCurrentStep() == tutorial.tutorial_end) {
								game_status = game_hero_mode;
								tutorial.nextStep();
								return;
							}
							tutorial.nextStep();
						}
					}
				}

				if ((abs(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Z)) > 80) || sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
					std::vector<std::string> rank_list;
					if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Z) > 0 || sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
						rank_list = rank.getTopKillList();
					}
					else {
						rank_list = rank.getTopSurviveTimeList();
					}
					for (int i = 0; i < rank_list.size(); i++) {
						gui_manager.setText(rank_list[i], 0.01, rank_start + i, Point(-window.getView().getSize().x / 2 + 100, -300 + 50 * i), 40);
					}
				}
			}

			if (!(game_status == game_strategic_mode || game_status == pause)) {
				if (hero_object->getUnitInfo()->isAffected(move_speed_buff)) {
					hero_speed *= consts.getMoveSpeedBuffMultiplier();
				}
				new_speed = new_speed.getNormal() * hero_speed;
				if (!(tutorial.isWorkingOnStep(tutorial.movement_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_dome_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_turret_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_gold_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_science_tutorial))) {
					new_speed = Point();
				}
				if (hero_object != nullptr) {
					Point old_speed = hero_object->getSpeed();
					old_speed -= previous_speed;
					hero_object->setSpeed(old_speed * (1 - consts.getFrictionCoef()));

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
			else if (game_status != pause) {
					hero_object->setSpeed(Point());
					hero_object->setAnimationType(hold_anim);
					new_speed = new_speed.getNormal() * consts.getStrategicCameraSpeed() * consts.getFPSLock() / fps.getFPS();
					view2.setCenter(view2.getCenter() + sf::Vector2f(new_speed.x, new_speed.y));
					strategic_back_pos = strategic_back_pos + sf::Vector2f(new_speed.x * 0.95, new_speed.y * 0.95);
				
			}


			if (hero_object != nullptr) {
				const double view_speed_coef = 0.05;    // must be from 0 to 1, where 0 for static camera and 1 for camera istantly over hero
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

			if (game_status == pause) {
				bool is_input_state = false;
				for (int i = pause_continue; i <= pause_to_desktop; i++) {
					if (i == chosen_button) {
						buttons[i].sprite.setTexture(buttons[i].texture_selected);
					}
					else {
						buttons[i].sprite.setTexture(buttons[i].texture_default);
					}
					buttons[i].sprite.setScale(sf::Vector2f(buttons[i].radius * 2 / buttons[i].sprite.getTexture()->getSize().x / window.getSize().x * window.getView().getSize().x, buttons[i].radius * 2 / buttons[i].sprite.getTexture()->getSize().y / window.getSize().y * window.getView().getSize().y));
					buttons[i].sprite.setOrigin(sf::Vector2f(buttons[i].sprite.getTexture()->getSize().x / 2, buttons[i].sprite.getTexture()->getSize().y / 2));
					buttons[i].sprite.setPosition(sf::Vector2f(buttons[i].pos.x + viewport_pos.x, buttons[i].pos.y + viewport_pos.y));
				}

				gui_manager.forceSetTopSign("Press Space to " + buttons[chosen_button].advice_string, 0.01);
				if (sf::Joystick::isConnected(0)) {
					gui_manager.forceSetTopSign("Press A to " + buttons[chosen_button].advice_string, 0.01);
				}
				title.setOrigin(title.getGlobalBounds().width / 2, title.getGlobalBounds().height / 2);

				if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus()) {

					Point cursor_pos;
					sf::Vector2i mouse_pos = sf::Mouse::getPosition();
					sf::Vector2i window_pos = window.getPosition();
					cursor_pos = Point(mouse_pos.x/* - window.getSize().x / 2*/ - window.getPosition().x, mouse_pos.y/* - window.getSize().y / 2*/ - window.getPosition().y);
					for (int i = pause_continue; i <= pause_to_desktop; i++) {
						if ((cursor_pos - buttons[i].pos).getLength() <= buttons[i].radius) {
							if (i == pause_continue) {
								unpause_game();
								is_input_state = true;
							}
							if (i == pause_to_menu) {
								game_status = main_menu;
								return;
							}
							if (i == pause_to_desktop) {
								game_status = exit_to_desktop;
								return;
							}
						}
					}
				}

				if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || (sf::Joystick::isConnected(0) && sf::Joystick::isButtonPressed(0, A))) && window.hasFocus()) {
					
					if (chosen_button == pause_continue) {
						is_input_state = true;
						unpause_game();
					}
					if (chosen_button == pause_to_menu) {
						game_status = main_menu;
						return;
					}
					if (chosen_button == pause_to_desktop) {
						game_status = exit_to_desktop;
						return;
					}
				}
				////
				Point move_vector;
				if (window.hasFocus()) {
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
						sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
						sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
						sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {

						if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
							move_vector += Point(0, -1);
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
							move_vector += Point(-1, 0);
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
							move_vector += Point(0, 1);
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
							move_vector += Point(1, 0);
						}
					}
					else if (sf::Joystick::isConnected(0)) {         // gamepad input

						move_vector = Point(
							sf::Joystick::getAxisPosition(0, sf::Joystick::X),
							sf::Joystick::getAxisPosition(0, sf::Joystick::Y));

						if (abs(move_vector.x) < 5 && abs(move_vector.y) < 5) {
							move_vector = Point();
						}
					}
				}
				
				move_vector.normalize();

				if (move_vector.x != 0 || move_vector.y != 0) {

					int next_menu_button = -1;
					double min_distance = 1e9;

					for (int i = pause_continue; i <= pause_to_desktop; i++) {
						double angle_diff =
							(std::atan2(move_vector.x, move_vector.y) -
								std::atan2(
									buttons[i].pos.x - buttons[chosen_button].pos.x,
									buttons[i].pos.y - buttons[chosen_button].pos.y));

						if (abs(angle_diff) > 0.000001) {     // angle_diff is not close to zero
							if (abs(angle_diff) > abs(angle_diff + 2 * PI)) {
								angle_diff += 2 * PI;
							}
							if (abs(angle_diff) > abs(angle_diff - 2 * PI)) {
								angle_diff -= 2 * PI;
							}
						}
						if (abs(angle_diff) <= PI / 8.0) {
							if (min_distance > (buttons[i].pos - buttons[chosen_button].pos).getLength() && (buttons[i].pos - buttons[chosen_button].pos).getLength() > 0.01 /*not close to zero*/) {
								next_menu_button = i;
								min_distance = (buttons[i].pos - buttons[chosen_button].pos).getLength();
							}
						}
					}


					if ((next_menu_button != -1) && ((frame_num - last_menu_choice) > fps.getFPS() / 2)) {
						chosen_button = next_menu_button;
						last_menu_choice = frame_num;
					}
					is_input_state = true;
				}
				if (!is_input_state) {
					last_menu_choice = frame_num - 200;
				}
				for (int i = pause_continue; i <= pause_to_desktop; i++) {
					window.draw(buttons[i].sprite);
				}
				window.draw(title);
			}
			

		}
		if (game_status == game_over) {
			window.setView(view4);
			for (int i = 0; i <= menu; i++) {
				if (i == chosen_button) {
					buttons[i].sprite.setTexture(buttons[i].texture_selected);
				}
				else {
					buttons[i].sprite.setTexture(buttons[i].texture_default);
				}
				buttons[i].sprite.setScale(sf::Vector2f(buttons[i].radius * 2 / buttons[i].sprite.getTexture()->getSize().x, buttons[i].radius * 2 / buttons[i].sprite.getTexture()->getSize().y));
				buttons[i].sprite.setOrigin(sf::Vector2f(buttons[i].sprite.getTexture()->getSize().x / 2, buttons[i].sprite.getTexture()->getSize().y / 2));
				buttons[i].sprite.setPosition(sf::Vector2f(buttons[i].pos.x, buttons[i].pos.y));
			}

			title.setString("Press Space to " + buttons[chosen_button].advice_string);
			if (sf::Joystick::isConnected(0)) {
				title.setString("Press A to " + buttons[chosen_button].advice_string);
			}
			title.setOrigin(title.getGlobalBounds().width / 2, title.getGlobalBounds().height / 2);

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

				Point cursor_pos;
				sf::Vector2i mouse_pos = sf::Mouse::getPosition();
				sf::Vector2i window_pos = window.getPosition();
				cursor_pos = Point(mouse_pos.x/* - window.getSize().x / 2*/ - window.getPosition().x, mouse_pos.y/* - window.getSize().y / 2*/ - window.getPosition().y);
				for (int i = 0; i < buttons.size(); i++) {
					if ((cursor_pos - buttons[i].pos).getLength() <= buttons[i].radius) {
						if (i == retry) {
							game_status = game_hero_mode;
							return;
						}
						if (i == menu) {
							game_status = main_menu;
							return;
						}
					}
				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || (sf::Joystick::isConnected(0) && sf::Joystick::isButtonPressed(0, A))) {
				if (chosen_button == retry) {
					game_status = game_hero_mode;
					return;
				}
				if (chosen_button == menu) {
					game_status = main_menu;
					return;
				}
			}

			Point move_vector;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
				sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
				sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
				sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
					move_vector += Point(0, -1);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
					move_vector += Point(-1, 0);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
					move_vector += Point(0, 1);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
					move_vector += Point(1, 0);
				}
			}
			else if (sf::Joystick::isConnected(0)) {         // gamepad input

				move_vector = Point(
					sf::Joystick::getAxisPosition(0, sf::Joystick::X),
					sf::Joystick::getAxisPosition(0, sf::Joystick::Y));

				if (abs(move_vector.x) < 5 && abs(move_vector.y) < 5) {
					move_vector = Point();
				}
			}
			move_vector.normalize();

			if (move_vector.x != 0 || move_vector.y != 0) {

				int next_menu_button = -1;
				double min_distance = 1e9;

				for (int i = 0; i < buttons.size(); i++) {
					double angle_diff =
						(std::atan2(move_vector.x, move_vector.y) -
							std::atan2(
								buttons[i].pos.x - buttons[chosen_button].pos.x,
								buttons[i].pos.y - buttons[chosen_button].pos.y));

					if (abs(angle_diff) > 0.000001) {     // angle_diff is not close to zero
						if (abs(angle_diff) > abs(angle_diff + 2 * PI)) {
							angle_diff += 2 * PI;
						}
						if (abs(angle_diff) > abs(angle_diff - 2 * PI)) {
							angle_diff -= 2 * PI;
						}
					}
					if (abs(angle_diff) <= PI / 8.0) {
						if (min_distance > (buttons[i].pos - buttons[chosen_button].pos).getLength() && (buttons[i].pos - buttons[chosen_button].pos).getLength() > 0.01 /*not close to zero*/) {
							next_menu_button = i;
							min_distance = (buttons[i].pos - buttons[chosen_button].pos).getLength();
						}
					}
				}


				if ((next_menu_button != -1) && ((frame_num - last_menu_choice) > fps.getFPS() / 2)) {
					chosen_button = next_menu_button;
					last_menu_choice = frame_num;
				}
			}

			window.draw(game_over_background_sprite);
			for (int i = 0; i < buttons.size(); i++) {
				window.draw(buttons[i].sprite);
			}
			window.draw(title);
		}
        if (game_status == research) {

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::F) || sf::Joystick::isButtonPressed(0, BACK)) && (frame_num - last_research_open) > fps.getFPS() / 4 /* 0.25 sec delay for changing view again */) {
				if (tutorial.getCurrentStep() == tutorial.no_tutorial || (tutorial.getCurrentStep() == tutorial.research_tutorial_close && research_number > 0)) {
					tutorial.nextStep();
					last_research_open = frame_num;
					game_status = prev_game_status;
					round_start = std::chrono::steady_clock::now();
				}
            }

            research_background_sprite.setScale(window.getSize().x / research_background_texture.getSize().x, window.getSize().y / research_background_texture.getSize().y);
            research_background_sprite.setPosition(0, 0);

			window.setView(view3);
            window.clear(sf::Color::Black);

			res_visual_ctrl.processFrame(&window, view3.getCenter(), cur_research_index);

			Point move_vector;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || 
				sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
				sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
				sf::Keyboard::isKeyPressed(sf::Keyboard::D)){

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
					move_vector += Point(0, -1);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
					move_vector += Point(-1, 0);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
					move_vector += Point(0, 1);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
					move_vector += Point(1, 0);
				}
			}
			else if (sf::Joystick::isConnected(0)) {         // gamepad input

				move_vector = Point(
					sf::Joystick::getAxisPosition(0, sf::Joystick::X),
					sf::Joystick::getAxisPosition(0, sf::Joystick::Y));

				if (abs(move_vector.x) < 5 && abs(move_vector.y) < 5) {
					move_vector = Point();
				}
			}
			move_vector.normalize();

			if (move_vector.x != 0 || move_vector.y != 0) {

				int next_research_index = -1;
				double min_distance = 1e9;

				for (int i = 0; i < research_graph.size(); i++) {
					double angle_diff =
						(std::atan2(move_vector.x, move_vector.y) -
							std::atan2(
								research_graph[i]->pos.x - research_graph[cur_research_index]->pos.x,
								research_graph[i]->pos.y - research_graph[cur_research_index]->pos.y));

					if (abs(angle_diff) > 0.000001) {     // angle_diff is not close to zero
						if (abs(angle_diff) > abs(angle_diff + 2 * PI)) {
							angle_diff += 2 * PI;
						}
						if (abs(angle_diff) > abs(angle_diff - 2 * PI)) {
							angle_diff -= 2 * PI;
						}
					}
					if (abs(angle_diff) <= PI / 8.0) {
						if (min_distance > (research_graph[i]->pos - research_graph[cur_research_index]->pos).getLength() && (research_graph[i]->pos - research_graph[cur_research_index]->pos).getLength() > 0.01 /*not close to zero*/) {
							next_research_index = i;
							min_distance = (research_graph[i]->pos - research_graph[cur_research_index]->pos).getLength();
						}
					}
				}


				if (next_research_index != -1 && ((frame_num - last_research_choice) > fps.getFPS() / 2)) {
					cur_research_index = next_research_index;
					last_research_choice = frame_num;
				}
			}
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Joystick::isButtonPressed(0, A)) {
				if (!research_list[cur_research_index]->unlocked && research_manager.isResearchActive(cur_research_index)) {
					resource_manager.spendResearch(research_list[cur_research_index]->cost);
					research_list[cur_research_index]->unlocked = true;
					research_number++;
				}
			}

			const double view_speed_coef = 0.035;    // must be from 0 to 1, where 0 for static camera and 1 for camera istantly over hero
			Point research_pos = research_graph[cur_research_index]->pos;
			Point diff = (research_pos - Point(view3.getCenter())) * view_speed_coef * consts.getFPSLock() / fps.getFPS();
			view3.setCenter(view3.getCenter() + sf::Vector2f(diff.x, diff.y));
        }

		vibration_time--;
		window.display();

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
				game_status = exit_to_desktop;
			}
			if (event.type == sf::Event::LostFocus) {
				pause_game();
			}
		}
	}
}

int main() {

	HWND console_hWnd = GetConsoleWindow();
	//ShowWindow(console_hWnd, SW_HIDE);

	sf::ContextSettings context_settings;
	context_settings.antialiasingLevel = 8;
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(sf::VideoMode(settings.getWindowWidth(), settings.getWindowHeight()), "AsteroidTD", sf::Style::None, context_settings);

	window.setFramerateLimit(consts.getFPSLock());

	collision_type_init();
	object_type_init();
	event_type_init();
	sprite_type_init();
	animation_type_init();
	faction_type_init();

	unit_info_init();
	collision_info_init();

	srand(time(NULL));   // setting a magic number to random generation

	if (!settings.isLoaded()) {
		std::cout << "Settings file error, enabling default settings" << std::endl;
		settings.setDefaults();
	}

	int screenW = GetSystemMetrics(SM_CXSCREEN);
	int screenH = GetSystemMetrics(SM_CYSCREEN);
	window.setSize(sf::Vector2u(screenW, screenH));
	window.setPosition(sf::Vector2i(0, 0));

	VisualController visual_ctrl;
	GUIVisualController gui_visual_ctrl(&window);
	ResearchVisualController res_visual_ctrl(&window);

	sf::Texture menu_background_texture;
	menu_background_texture.loadFromFile("background/menu_background.png");
	sf::Sprite menu_background_sprite;
	menu_background_sprite.setTexture(menu_background_texture);
	menu_background_sprite.setPosition(0, 0);
	menu_background_sprite.setScale(sf::Vector2f(window.getSize().x / menu_background_texture.getSize().x, window.getSize().y / menu_background_texture.getSize().y));


	sf::Text title;
	sf::Text nickname_title;
	sf::Font base_font;
	base_font.loadFromFile("a_Alterna.ttf");

	title.setPosition(sf::Vector2f(window.getSize().x / 2, 150));
	title.setFillColor(sf::Color::White);
	title.setOutlineColor(sf::Color::Black);
	title.setOutlineThickness(1);
	title.setCharacterSize(50);
	title.setFont(base_font);

	nickname_title.setPosition(sf::Vector2f(window.getSize().x / 2 - 75, window.getSize().y / 2 - 200));
	nickname_title.setFillColor(sf::Color::White);
	nickname_title.setOutlineColor(sf::Color::Black);
	nickname_title.setOutlineThickness(1);
	nickname_title.setCharacterSize(50);
	nickname_title.setFont(base_font);

	sf::View main_view(sf::Vector2f((int)window.getSize().x / 2, (int)window.getSize().y / 2), sf::Vector2f(window.getSize().x, window.getSize().y));      // main menu view
	window.setView(main_view);

	game_status = main_menu;

	// main menu
	struct Button {
		Point pos;
		sf::Texture texture_default, texture_selected;
		sf::Sprite sprite;
		float radius = 75;
		std::string advice_string;
	};
	enum buttons_name {
		infinity_mode_button,
		settings_button,
		shutdown_button,

		BUTTONS_NAME_LIST_SIZE
	};
	std::vector<Button> buttons;
	buttons.resize(BUTTONS_NAME_LIST_SIZE);

	buttons[infinity_mode_button].pos = Point(window.getSize().x / 2 - 150, 350);
	buttons[infinity_mode_button].texture_default.loadFromFile("menu_buttons\\inf_mode.png");
	buttons[infinity_mode_button].texture_selected.loadFromFile("menu_buttons\\inf_mode_selected.png");
	buttons[infinity_mode_button].sprite.setTexture(buttons[infinity_mode_button].texture_default);
	buttons[infinity_mode_button].advice_string = "start infinity mode";

	buttons[settings_button].pos = Point(window.getSize().x / 2 + 150, 350);
	buttons[settings_button].texture_default.loadFromFile("menu_buttons\\settings.png");
	buttons[settings_button].texture_selected.loadFromFile("menu_buttons\\settings_selected.png");
	buttons[settings_button].sprite.setTexture(buttons[settings_button].texture_default);
	buttons[settings_button].advice_string = "open settings";

	buttons[shutdown_button].pos = Point(window.getSize().x / 2, 550);
	buttons[shutdown_button].texture_default.loadFromFile("menu_buttons\\shutdown.png");
	buttons[shutdown_button].texture_selected.loadFromFile("menu_buttons\\shutdown_selected.png");
	buttons[shutdown_button].sprite.setTexture(buttons[shutdown_button].texture_default);
	buttons[shutdown_button].advice_string = "close the game";

	int chosen_button = infinity_mode_button;

	std::vector<std::string> settings_list = settings.getChangeableSettingsList();
	float shift = 0;
	float max_shift = 3.5;
	int chosen_setting = 0;
	int max_font_size = 50;
	int sign_distance = 100;
	sf::Text settings_sign;
	settings_sign.setFillColor(sf::Color::White);
	settings_sign.setOutlineColor(sf::Color::Black);
	settings_sign.setOutlineThickness(1);
	settings_sign.setFont(base_font);
	bool settings_changing = false;

	int frame_num = 0;
	int last_menu_choice = -1000;


	if (!settings.getNickname().size()) {
		game_status = nickname_enter;
		keyboard_buffer.activate();
		keyboard_buffer.setMaxBufferSize(24);
	}

	rank.launchUpdateWorker();
	rank.launchSelfRankUpdateWorker();

	sf::Music music;
	if (!music.openFromFile("space_back1.1.flac")) {
		std::cout << "Music load failed" << std::endl;
	}
	else {
		music.setLoop(true);
		music.play();
	}


	while (true) {
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
				return 0;
			}
		}

		frame_num++;

		window.setView(main_view);
		if (window.hasFocus()) {
			if (game_status == main_menu) {
				for (int i = 0; i < buttons.size(); i++) {
					if (i == chosen_button) {
						buttons[i].sprite.setTexture(buttons[i].texture_selected);
					}
					else {
						buttons[i].sprite.setTexture(buttons[i].texture_default);
					}
					buttons[i].sprite.setScale(sf::Vector2f(buttons[i].radius * 2 / buttons[i].sprite.getTexture()->getSize().x, buttons[i].radius * 2 / buttons[i].sprite.getTexture()->getSize().y));
					buttons[i].sprite.setOrigin(sf::Vector2f(buttons[i].sprite.getTexture()->getSize().x / 2, buttons[i].sprite.getTexture()->getSize().y / 2));
					buttons[i].sprite.setPosition(sf::Vector2f(buttons[i].pos.x, buttons[i].pos.y));
				}

				title.setString("Press Space to " + buttons[chosen_button].advice_string);
				if (sf::Joystick::isConnected(0)) {
					title.setString("Press A to " + buttons[chosen_button].advice_string);
				}
				title.setOrigin(title.getGlobalBounds().width / 2, title.getGlobalBounds().height / 2);

				bool is_input_state = false;
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

					Point cursor_pos;
					sf::Vector2i mouse_pos = sf::Mouse::getPosition();
					sf::Vector2i window_pos = window.getPosition();
					cursor_pos = Point(mouse_pos.x/* - window.getSize().x / 2*/ - window.getPosition().x, mouse_pos.y/* - window.getSize().y / 2*/ - window.getPosition().y);
					for (int i = 0; i < buttons.size(); i++) {
						if ((cursor_pos - buttons[i].pos).getLength() <= buttons[i].radius) {
							if (i == infinity_mode_button) {
								game_status = game_hero_mode;
								is_input_state = true;
							}
							if (i == settings_button) {
								game_status = settings_menu;
								is_input_state = true;
							}
							if (i == shutdown_button) {
								game_status = exit_to_desktop;
								is_input_state = true;
							}
							last_menu_choice = frame_num;
						}
					}
				}
				if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || (sf::Joystick::isConnected(0) && sf::Joystick::isButtonPressed(0, A))) && (frame_num - last_menu_choice) > consts.getFPSLock() / 4) {
					if (chosen_button == infinity_mode_button) {
						game_status = game_hero_mode;
						is_input_state = true;
					}
					if (chosen_button == settings_button) {
						game_status = settings_menu;
						is_input_state = true;
					}
					if (chosen_button == shutdown_button) {
						game_status = exit_to_desktop;
						is_input_state = true;
					}
					last_menu_choice = frame_num;
				}

				Point move_vector;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {

					if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
						move_vector += Point(0, -1);
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
						move_vector += Point(-1, 0);
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
						move_vector += Point(0, 1);
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
						move_vector += Point(1, 0);
					}
				}
				else if (sf::Joystick::isConnected(0)) {         // gamepad input

					move_vector = Point(
						sf::Joystick::getAxisPosition(0, sf::Joystick::X),
						sf::Joystick::getAxisPosition(0, sf::Joystick::Y));

					if (abs(move_vector.x) < 5 && abs(move_vector.y) < 5) {
						move_vector = Point();
					}
				}
				move_vector.normalize();

				if (move_vector.x != 0 || move_vector.y != 0) {

					int next_menu_button = -1;
					double min_distance = 1e9;

					for (int i = 0; i < buttons.size(); i++) {
						double angle_diff =
							(std::atan2(move_vector.x, move_vector.y) -
								std::atan2(
									buttons[i].pos.x - buttons[chosen_button].pos.x,
									buttons[i].pos.y - buttons[chosen_button].pos.y));

						if (abs(angle_diff) > 0.000001) {     // angle_diff is not close to zero
							if (abs(angle_diff) > abs(angle_diff + 2 * PI)) {
								angle_diff += 2 * PI;
							}
							if (abs(angle_diff) > abs(angle_diff - 2 * PI)) {
								angle_diff -= 2 * PI;
							}
						}
						if (abs(angle_diff) <= PI / 8.0) {
							if (min_distance > (buttons[i].pos - buttons[chosen_button].pos).getLength() && (buttons[i].pos - buttons[chosen_button].pos).getLength() > 0.01 /*not close to zero*/) {
								next_menu_button = i;
								min_distance = (buttons[i].pos - buttons[chosen_button].pos).getLength();
							}
						}
					}


					if ((next_menu_button != -1) && ((frame_num - last_menu_choice) > consts.getFPSLock() / 2)) {
						chosen_button = next_menu_button;
						last_menu_choice = frame_num;
						is_input_state = true;
					}
				}

				if (!is_input_state) {
					last_menu_choice = frame_num - 200;
				}

				window.draw(menu_background_sprite);
				for (int i = 0; i < buttons.size(); i++) {
					window.draw(buttons[i].sprite);
				}
				window.draw(title);
				window.display();
			}
			else if (game_status == settings_menu) {
				window.draw(menu_background_sprite);
				for (int i = 0; i < settings_list.size(); i++) {
					if (settings_list[i] == "nickname") {
						settings_sign.setString("Nickname: ");
					}
					if (settings_list[i] == "ranking_server") {
						settings_sign.setString("Ranking server: ");
					}
					if (settings_list[i] == "window_width") {
						settings_sign.setString("Window width: ");
					}
					if (settings_list[i] == "window_height") {
						settings_sign.setString("Window height: ");
					}
					if (!settings_changing || i != chosen_setting) {
						settings_sign.setString(settings_sign.getString() + settings.getSettingString(settings_list[i]));
					}
					else {
						settings_sign.setString(settings_sign.getString() + keyboard_buffer.getBuffer());
					}
					settings_sign.setPosition(window.getView().getSize().x / 2, window.getView().getSize().y / 2 + sign_distance * (i - shift) - 100);
					int char_size_raw = (max_shift - std::min(max_shift, abs(i - shift))) / max_shift * max_font_size;
					settings_sign.setCharacterSize(char_size_raw < 15 ? 0 : std::max(30, char_size_raw));
					settings_sign.setOrigin(settings_sign.getGlobalBounds().width / 2, settings_sign.getGlobalBounds().height / 2);
					if (settings_sign.getCharacterSize() > 5) {
						window.draw(settings_sign);
					}
				}
				bool key_pressed = false;
				if (!settings_changing) {
					if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) || (sf::Joystick::isConnected(0) && (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) < -10))) {
						if ((frame_num - last_menu_choice) > consts.getFPSLock() / 4) {
							last_menu_choice = frame_num;
							chosen_setting = std::max(0, chosen_setting - 1);
						}
						key_pressed = true;
					}
					if (((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) || (sf::Joystick::isConnected(0) && (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) > 10)))) {
						if ((frame_num - last_menu_choice) > consts.getFPSLock() / 4) {
							last_menu_choice = frame_num;
							chosen_setting = std::min((int)settings_list.size() - 1, chosen_setting + 1);
						}
						key_pressed = true;
					}
					if (((sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) || (sf::Joystick::isConnected(0) && (sf::Joystick::isButtonPressed(0, A))))) {
						if ((frame_num - last_menu_choice) > consts.getFPSLock() / 4) {
							last_menu_choice = frame_num;
							settings_changing = true;
							keyboard_buffer.activate();
							keyboard_buffer.clearBuffer();
							keyboard_buffer.setMaxBufferSize(24);
						}
						key_pressed = true;
					}
					if (((sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) || (sf::Joystick::isConnected(0) && (sf::Joystick::isButtonPressed(0, BACK))))) {
						if ((frame_num - last_menu_choice) > consts.getFPSLock() / 4) {
							game_status = main_menu;
						}
						key_pressed = true;
					}

					title.setString("Press Escape on keyboard or Back on gamepad to exit settings");
				}
				else {
					if (((sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) || (sf::Joystick::isConnected(0) && (sf::Joystick::isButtonPressed(0, A))))) {
						if ((frame_num - last_menu_choice) > consts.getFPSLock() / 4) {
							last_menu_choice = frame_num;
							settings_changing = false;
							keyboard_buffer.deactivate();
							settings.setSetting(settings_list[chosen_setting], keyboard_buffer.getBuffer());
							if (settings_list[chosen_setting] == "nickname" || settings_list[chosen_setting] == "ranking_server") {
								rank.reboot();
							}
						}
						key_pressed = true;
					}
					if (((sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) || (sf::Joystick::isConnected(0) && (sf::Joystick::isButtonPressed(0, BACK))))) {
						if ((frame_num - last_menu_choice) > consts.getFPSLock() / 4) {
							last_menu_choice = frame_num;
							settings_changing = false;
							keyboard_buffer.deactivate();
						}
						key_pressed = true;
					}
					title.setString("Press Escape on keyboard or Back on gamepad to discard changes");
				}
				if (!key_pressed) {
					last_menu_choice = frame_num - 200;
				}
				shift = shift + (chosen_setting - shift) * 0.05;
				title.setOrigin(title.getGlobalBounds().width / 2, title.getGlobalBounds().height / 2);
				window.draw(title);
				window.display();
			}
			else if (game_status == exit_to_desktop) {
				return 0;
			}
			else if (game_status == nickname_enter) {
				title.setString("Please enter your nickname and press Enter");

				if (sf::Joystick::isConnected(0)) {
					title.setString("Please enter your nickname and press A");
				}

				if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || (sf::Joystick::isConnected(0) && sf::Joystick::isButtonPressed(0, A))) && (frame_num - last_menu_choice) > consts.getFPSLock()) {
					settings.setNickname(keyboard_buffer.getBuffer());
					keyboard_buffer.activate();
					keyboard_buffer.clearBuffer();
					game_status = main_menu;
					continue;
				}

				nickname_title.setString("Your nickname: " + keyboard_buffer.getBuffer());
				title.setOrigin(title.getGlobalBounds().width / 2, title.getGlobalBounds().height / 2);
				nickname_title.setOrigin(title.getGlobalBounds().width / 2, title.getGlobalBounds().height / 2);

				window.draw(menu_background_sprite);
				window.draw(title);
				window.draw(nickname_title);
				window.display();
			}
			else {
				fps.reset();
				try {
					gameCycle("map2", window, visual_ctrl, gui_visual_ctrl, res_visual_ctrl);
				}
				catch (std::exception exc) {
					std::cout << "Exception handled" << std::endl;
				}
			}
		}
		
	}

	return 0;
}