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
#include "MusicManager.h"
#include "BackgroundGeneration.h"
#include "PhraseContainer.h"
#include "NPCInfo.h"
#include "DialogVisualController.h"
#include "MissionVisualController.h"
#include "ButtonSelector.h"

#include <chrono>
#include <SFML/Audio.hpp>

#ifdef __linux__ || __APPLE__ 
#include <unistd.h>
#include <X11/Xlib.h>
#elif _WIN32
#include <Windows.h>
#include <Xinput.h>
#else
#endif

enum XBOXGamepadButtons {
	A,B,X,Y,LB,RB,BACK,START,LSTICK,RSTICK
};

void gameCycle(std::string map_name, sf::RenderWindow & window, VisualController & visual_ctrl, GUIVisualController & gui_visual_ctrl, ResearchVisualController & res_visual_ctrl, MusicManager & music_manager) {

	sf::Texture main_background_texture, research_background_texture, game_over_background_texture;
	sf::Sprite main_background_sprite, research_background_sprite, game_over_background_sprite;


	game_over_background_texture.loadFromFile("background" + path_separator + "game_over_background.png");
	game_over_background_sprite.setTexture(game_over_background_texture);
	game_over_background_sprite.setPosition(0, 0);
	game_over_background_sprite.setScale(sf::Vector2f(window.getSize().x / game_over_background_texture.getSize().x, window.getSize().y / game_over_background_texture.getSize().y));

	sf::Text title;
	sf::Font base_font;
	
	sf::View view1(sf::Vector2f(0.0, 0.0), sf::Vector2f(window.getSize().x * consts.getHeroViewScale(), window.getSize().y * consts.getHeroViewScale()));  // main game mode
	sf::View view2(sf::Vector2f(0.0, 0.0), sf::Vector2f(window.getSize().x * consts.getStrategicViewScale(), window.getSize().y * consts.getStrategicViewScale()));      // strategic view
	sf::View view3(sf::Vector2f(0.0, 0.0), sf::Vector2f(window.getSize().x, window.getSize().y));      // research view
	sf::View view4(sf::Vector2f((int)window.getSize().x / 2, (int)window.getSize().y / 2), sf::Vector2f(window.getSize().x, window.getSize().y));      // main menu view
	
	int last_view_change = 0;    // number of frame when you changed view last time
	int last_mode_change = 0;    // number of frame when you changed mode last time
	int last_build = 0;    // number of frame when you built smth last time
	int last_pause = 0;    // number of frame when you paused game last time
    int last_research_open = 0; // number of frame when you opened research list last time
	int last_research_choice = 0; // number of frame when you chose another research last time
	int last_menu_choice = 0;
	int last_dialog_choice = 0;
    int last_tutorial = 0;
    int last_volume_change = 0;
	int current_mission = 0;

	int last_shot = 0;

	GameStatus prev_game_status = game_pause;

	Map game_map1("maps/" + map_name + ".map", "mission.cfg");

	resource_manager.clear(settings.getStartGold(), 5);
    research_manager.initResearch("research.cfg");
	research_manager.setGraphSize();

	std::vector<Point> research_pos;
	for (int i = 0; i < research_manager.getResearchAmount(); i++) {
		research_pos.push_back(research_manager.getResearchNode(i)->pos);
	}
	button_selector.initButtonList(ButtonSelector::research, 300, PI / 3, research_pos);
	std::vector<Research *> research_list = research_manager.getResearchArray();
	std::vector<ResearchNode *> research_graph = research_manager.getResearchGraph();
	int cur_research_index = 0;

	GUIManager gui_manager;
    DialogVisualController dialog_visual_ctrl(window);
	MissionVisualController mission_visual_ctrl(window);

	sf::Vector2f strategic_back_pos, main_back_pos;

	game_status = game_hero_mode;
	skills_mode = set1;

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
#ifdef __linux__ || __APPLE__
				sleep(1);
#elif _WIN32
				Sleep(1000);
#endif
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
	title.setCharacterSize(50 * window.getView().getSize().y / 1080);
	title.setFont(base_font);

	SkillsMode prev_skills_mode = set2;

    std::vector<std::wstring> buffer;
    std::wstring
        skills_build_sign,
        skills_ability_sign,
        skills_interact_sign,
		enemy_power_sign,
        new_wave_sign,
        message_sign,
        keyboard_press_title,
        gamepad_press_title,

        try_again_title,
        go_back_to_menu_title,
        continue_game_title,
        go_to_menu_title,
        go_to_desktop_title,
        volume_down_title,
        volume_up_title,
        gold_sign_wstring,
        research_sign_wstring,

        aster_basic_sign,
        aster_gold_inter_sign,
        aster_iron_inter_sign,
        aster_susp_flat_sign,

        aster_str_crack_sign,
        aster_ord_wealthy_sign,
        aster_poor_mount_sign,
        aster_wealthy_crack_sign,
        aster_ord_mount_sign,
        aster_str_poor_sign,

        aster_swamp_gold_sign,
        aster_unstab_expl_sign,
        aster_old_lab_sign,
        aster_lava_surf_sign,

        aster_drone_sign,
        aster_rocket_sign,
        aster_anc_lab_sign,
        aster_anc_mine_sign,

        tutorial_base_desc,
        tutorial_move,
        tutorial_view,
        tutorial_dome,
        tutorial_dome_desc,
        tutorial_turret,
        tutorial_turret_desc,
        tutorial_gold,
        tutorial_gold_desc,
        tutorial_science,
        tutorial_science_desc,
        tutorial_skill_mode,
        tutorial_rocket,
        tutorial_speed,
        tutorial_damage,
        tutorial_heal,
        tutorial_research,
        tutorial_research_close,
        tutorial_pause,
        tutorial_unpause,
        tutorial_end,

        tutorial_for_next_step,
        tutorial_move_pts_left,
        tutorial_times_left;


    auto resetStrings = [&]() {
        buffer = phrase_container.getPhraseBuffer(L"skills_build_sign_GUI", 0);
        skills_build_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"skills_ability_sign_GUI", 0);
        skills_ability_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
		buffer = phrase_container.getPhraseBuffer(L"skills_interact_sign_GUI", 0);
		skills_interact_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
		buffer = phrase_container.getPhraseBuffer(L"enemy_power_sign_GUI", 0);
		enemy_power_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"new_wave_sign_GUI", 0);
        new_wave_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"message_sign_GUI", 0);
        message_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"keyboard_press_title_GUI", 0);
        keyboard_press_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"gamepad_press_title_GUI", 0);
        gamepad_press_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"try_again_title_GUI", 0);
        try_again_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"go_back_to_menu_title_GUI", 0);
        go_back_to_menu_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"continue_game_title_GUI", 0);
        continue_game_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"go_to_menu_title_GUI", 0);
        go_to_menu_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"go_to_desktop_title_GUI", 0);
        go_to_desktop_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"volume_down_title_GUI", 0);
        volume_down_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"volume_up_title_GUI", 0);
        volume_up_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"gold_sign_GUI", 0);
        gold_sign_wstring = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"research_sign_GUI", 0);
        research_sign_wstring = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"aster_basic_GUI", 0);
        aster_basic_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_gold_inter_GUI", 0);
        aster_gold_inter_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_iron_inter_GUI", 0);
        aster_iron_inter_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_susp_flat_GUI", 0);
        aster_susp_flat_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"aster_str_crack_GUI", 0);
        aster_str_crack_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_ord_wealthy_GUI", 0);
        aster_ord_wealthy_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_poor_mount_GUI", 0);
        aster_poor_mount_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_wealthy_crack_GUI", 0);
        aster_wealthy_crack_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_ord_mount_GUI", 0);
        aster_ord_mount_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_str_poor_GUI", 0);
        aster_str_poor_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"aster_swamp_gold_GUI", 0);
        aster_swamp_gold_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_unstab_expl_GUI", 0);
        aster_unstab_expl_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_old_lab_GUI", 0);
        aster_old_lab_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_lava_surf_GUI", 0);
        aster_lava_surf_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"aster_drone_GUI", 0);
        aster_drone_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_rocket_GUI", 0);
        aster_rocket_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_anc_lab_GUI", 0);
        aster_anc_lab_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"aster_anc_mine_GUI", 0);
        aster_anc_mine_sign = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"tutorial_base_desc", 0);
        tutorial_base_desc = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_move", 0);
        tutorial_move = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_view", 0);
        tutorial_view = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_dome", 0);
        tutorial_dome = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_dome_desc", 0);
        tutorial_dome_desc = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_turret", 0);
        tutorial_turret = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_turret_desc", 0);
        tutorial_turret_desc = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_gold", 0);
        tutorial_gold = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_gold_desc", 0);
        tutorial_gold_desc = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_science", 0);
        tutorial_science = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_science_desc", 0);
        tutorial_science_desc = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_skill_mode", 0);
        tutorial_skill_mode = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_rocket", 0);
        tutorial_rocket = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_speed", 0);
        tutorial_speed = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_damage", 0);
        tutorial_damage = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_heal", 0);
        tutorial_heal = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_research", 0);
        tutorial_research = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_research_close", 0);
        tutorial_research_close = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_pause", 0);
        tutorial_pause = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_unpause", 0);
        tutorial_unpause = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_end", 0);
        tutorial_end = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"tutorial_for_next_step", 0);
        tutorial_for_next_step = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_move_pts_left", 0);
        tutorial_move_pts_left = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        buffer = phrase_container.getPhraseBuffer(L"tutorial_times_left", 0);
        tutorial_times_left = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
    };

    resetStrings();

	struct Button {
		Point pos;
		sf::Texture texture_default, texture_selected;
		sf::Sprite sprite;
		float radius = 75;
		std::wstring advice_string;
	};

	enum buttons_name {
		retry,
		menu,

		BUTTONS_NAME_LIST_SIZE
	};
	std::vector<Button> buttons_game_over;
	buttons_game_over.resize(BUTTONS_NAME_LIST_SIZE);

	buttons_game_over[retry].pos = Point(-200, -150);
	buttons_game_over[retry].texture_default.loadFromFile("menu_buttons" + path_separator + "retry.png");
	buttons_game_over[retry].texture_selected.loadFromFile("menu_buttons" + path_separator + "retry_selected.png");
	buttons_game_over[retry].sprite.setTexture(buttons_game_over[retry].texture_default);
	buttons_game_over[retry].advice_string = try_again_title;

	buttons_game_over[menu].pos = Point(200, -150);
	buttons_game_over[menu].texture_default.loadFromFile("menu_buttons" + path_separator + "menu.png");
	buttons_game_over[menu].texture_selected.loadFromFile("menu_buttons" + path_separator + "menu_selected.png");
	buttons_game_over[menu].sprite.setTexture(buttons_game_over[menu].texture_default);
	buttons_game_over[menu].advice_string = go_back_to_menu_title;

	std::vector<Point> game_over_buttons_pos = {
		buttons_game_over[retry].pos,
		buttons_game_over[menu].pos
	};
	button_selector.initButtonList(ButtonSelector::game_over, 500, PI / 3, game_over_buttons_pos);
	std::vector<Button> buttons;
    enum pause_buttons_name {
        pause_continue,
        pause_to_menu,
        pause_to_desktop,
        pause_volume_down,
        pause_volume_up,

        PAUSE_BUTTONS_NAME_LIST_SIZE
    };
    buttons.clear();
    buttons.resize(PAUSE_BUTTONS_NAME_LIST_SIZE);

	buttons[pause_continue].pos = Point(0, -250);
	buttons[pause_continue].texture_default.loadFromFile("menu_buttons" + path_separator + "pause_continue.png");
	buttons[pause_continue].texture_selected.loadFromFile("menu_buttons" + path_separator + "pause_continue_selected.png");
	buttons[pause_continue].sprite.setTexture(buttons[pause_continue].texture_default);
	buttons[pause_continue].advice_string = continue_game_title;

	buttons[pause_to_menu].pos = Point();
	buttons[pause_to_menu].texture_default.loadFromFile("menu_buttons" + path_separator + "pause_to_menu.png");
	buttons[pause_to_menu].texture_selected.loadFromFile("menu_buttons" + path_separator + "pause_to_menu_selected.png");
	buttons[pause_to_menu].sprite.setTexture(buttons[pause_to_menu].texture_default);
	buttons[pause_to_menu].advice_string = go_to_menu_title;

    buttons[pause_to_desktop].pos = Point(0, 250);
    buttons[pause_to_desktop].texture_default.loadFromFile("menu_buttons" + path_separator + "pause_to_desktop.png");
    buttons[pause_to_desktop].texture_selected.loadFromFile("menu_buttons" + path_separator + "pause_to_desktop_selected.png");
    buttons[pause_to_desktop].sprite.setTexture(buttons[pause_to_desktop].texture_default);
    buttons[pause_to_desktop].advice_string = go_to_desktop_title;

    buttons[pause_volume_up].pos = Point(200, -125);
    buttons[pause_volume_up].texture_default.loadFromFile("menu_buttons" + path_separator + "pause_volume_up.png");
    buttons[pause_volume_up].texture_selected.loadFromFile("menu_buttons" + path_separator + "pause_volume_up_selected.png");
    buttons[pause_volume_up].sprite.setTexture(buttons[pause_volume_up].texture_default);
    buttons[pause_volume_up].advice_string = volume_up_title;

    buttons[pause_volume_down].pos = Point(200, 125);
    buttons[pause_volume_down].texture_default.loadFromFile("menu_buttons" + path_separator + "pause_volume_down.png");
    buttons[pause_volume_down].texture_selected.loadFromFile("menu_buttons" + path_separator + "pause_volume_down_selected.png");
    buttons[pause_volume_down].sprite.setTexture(buttons[pause_volume_down].texture_default);
    buttons[pause_volume_down].advice_string = volume_down_title;

    std::vector<Point> buttons_pos = {
        buttons[pause_continue].pos,
        buttons[pause_to_menu].pos,
        buttons[pause_to_desktop].pos,
        buttons[pause_volume_down].pos,
        buttons[pause_volume_up].pos
    };
    button_selector.initButtonList(ButtonSelector::pause_buttons, 1000, PI / 4, buttons_pos);

	int chosen_button = 0;  // in game over menu
    int chosen_answer = 0;   // for dialogs


	auto pause_game = [&]() {
		if (game_status != game_pause && tutorial.isWorkingOnStep(tutorial.pause_tutorial)) {
			prev_game_status = game_status;
			game_status = game_pause;
			last_pause = frame_num;
			window.setView(view2);
			rank.addGameplayTime(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - round_start).count());
			if (tutorial.isWorkingOnStep(tutorial.pause_tutorial)) {
				tutorial.nextStep();
			}
			chosen_button = pause_continue;
		}
	};

	auto unpause_game = [&]() {
		if (game_status == game_pause) {
			game_status = prev_game_status;
			if (game_status == research) {
				game_status = game_hero_mode;
			}
			prev_game_status = game_pause;
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
		}
	};

	auto shoot_rocket = [&](Object * hero_object) {
		Point bullet_pos = hero_object->getPosition() + Point(cos((hero_object->getAngle()) / 180 * PI), sin((hero_object->getAngle()) / 180 * PI)) * hero_object->getCollisionModel()->getMaxRadius();
		Object * object = new Object
		(
			bullet_pos,
			Point(),
			ObjectType::bullet,
			CollisionType::bullet_col,
			VisualInfo
			(
				SpriteType::rocket_sprite,
				AnimationType::hold_anim,
				1000000000
			)
		);
		object->getUnitInfo()->setFaction(hero_object->getUnitInfo()->getFaction());
		object->setParent(hero_object);     // to remember who is shooting
		object->setAutoOrigin();
		object->setAngle(hero_object->getAngle() + 90);
		object->setSpeed(Point(cos((hero_object->getAngle()) / 180 * PI), sin((hero_object->getAngle()) / 180 * PI)) * 5 + hero_object->getSpeed());

		game_map1.addObject(object, game_map1.main_layer);

		hero_object->attachObject(object);
	};

    background_manager.generateAroundCenter(game_map1.getHero()->getPosition());

	while (window.isOpen())
	{
		frame_num++;
		// FPS 
		fps.processFrame(frame_num);

		for (int i = 0; i < global_event_buffer.size(); i++) {
			switch (global_event_buffer[i].getEventType()) {
			case EventType::reward:
				resource_manager.addGold(*static_cast<float *>(global_event_buffer[i].getData(0)));
				global_event_buffer.erase(global_event_buffer.begin() + i);
				break;
			case EventType::message:
				gui_manager.forceSetTopSign(message_sign + L": " + *static_cast<std::wstring *>(global_event_buffer[i].getData(0)), 5);
				global_event_buffer.erase(global_event_buffer.begin() + i);
				break;
			}
		}

        Point cursor_pos;
        sf::Vector2i mouse_pos = sf::Mouse::getPosition();
        sf::Vector2i window_pos = window.getPosition();
        cursor_pos = Point(mouse_pos.x - sf::Vector2f(window.getSize()).x / 2 - window_pos.x, mouse_pos.y - sf::Vector2f(window.getSize()).y / 2 - window_pos.y);

		if (game_status == game_hero_mode || game_status == game_strategic_mode || game_status == game_pause) {
			
			if (tutorial.isWorkingOnStep(tutorial.no_tutorial)) {
				game_frame_num++;
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
#ifdef __linux__ || __APPLE__ 
#elif _WIN32
                if (sf::Joystick::isConnected(0)) {
                    XINPUT_VIBRATION vibration;
                    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
                    vibration.wLeftMotorSpeed = 0; // use any value between 0-65535 here
                    vibration.wRightMotorSpeed = 65535; // use any value between 0-65535 here
                    XInputSetState(0, &vibration);
                }
#else
#endif
				
			}
			else {
#ifdef __linux__ || __APPLE__  
#elif _WIN32
                if (sf::Joystick::isConnected(0)) {
                    XINPUT_VIBRATION vibration;
                    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
                    vibration.wLeftMotorSpeed = 0; // use any value between 0-65535 here
                    vibration.wRightMotorSpeed = 0; // use any value between 0-65535 here
                    XInputSetState(0, &vibration);
                }
#else
#endif
			}
			hero_hp = hero_object->getUnitInfo()->getHealth();
			// game cycle
            Point gold_pos = Point(-(int)window.getView().getSize().x / 2 / 1.2, -(int)window.getView().getSize().y / 2 / 1.2);
			gui_manager.setText(gold_sign_wstring + std::to_wstring((int)resource_manager.getGold()), 0.01, gold_sign, gold_pos, 30);
            Point res_pos = Point(-(int)window.getView().getSize().x / 2 / 1.2, -(int)window.getView().getSize().y / 2 / 1.2 + 55 * window.getView().getSize().y / 1080);
			gui_manager.setText(research_sign_wstring + std::to_wstring((int)resource_manager.getResearch()), 0.01, research_sign, res_pos, 30);

			if (game_map1.getClosestAsteroid() && game_map1.getClosestAsteroid()->getNPCInfo()) {
				if (skills_mode != npc_dialog) {
					prev_skills_mode = skills_mode;
				}
				skills_mode = SkillsMode::npc_dialog;
			}
			else if(prev_skills_mode != npc_dialog){
				skills_mode = prev_skills_mode;
				prev_skills_mode = SkillsMode::npc_dialog;
			}
            Point skill_pos = Point(
                window.getView().getSize().x / 2 / 1.2 - 250 * window.getView().getSize().x / 1920, 
                (int)window.getView().getSize().y / 2 / 1.2 + 10 * window.getView().getSize().x / 1920);
			Point enemy_power_pos = Point(
				-window.getView().getSize().x / 2 / 1.2 + 10 * window.getView().getSize().x / 1920,
				(int)window.getView().getSize().y / 2 / 1.2 + 10 * window.getView().getSize().x / 1920);

			if (skills_mode == set1) {
                
				gui_manager.setText(skills_ability_sign, 0.01, skill_status_sign, skill_pos, 30);
			}
			else if(skills_mode == set2) {
				gui_manager.setText(skills_build_sign, 0.01, skill_status_sign, skill_pos, 30);
			}
			else {
				gui_manager.setText(skills_interact_sign, 0.01, skill_status_sign, skill_pos, 30);
			}
			gui_manager.setText(enemy_power_sign + std::to_wstring(game_map1.getEnemyPowerCoef()).substr(0,4), 0.01, power_sign, enemy_power_pos, 30);

			if (game_status != game_pause && tutorial.isWorkingOnStep(tutorial.no_tutorial) && game_mode == GameMode::infinity_mode) {
				if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_wave).count() > wave_delay) {
					wave_count++;
					if (!(game_status == game_strategic_mode)) {
						game_map1.spawnEnemy(wave_count, Point(view1.getCenter().x, view1.getCenter().y));
					}
					else {
						game_map1.spawnEnemy(wave_count, Point(view2.getCenter().x, view2.getCenter().y));
					}
					gui_manager.forceSetTopSign(new_wave_sign + L"(" + std::to_wstring(wave_count) + L")", 5);
					last_wave = std::chrono::steady_clock::now();
				}
			}

			// tutorial code
			if (tutorial.getCurrentStep() != tutorial.no_tutorial) {
				if (tutorial.isWorkingOnStep(tutorial.base_description)) {
					gui_manager.forceSetTopSign(tutorial_base_desc/*"This tutorial will show you\nsome base mechanics of\nthis game\n(press "*/ + (sf::Joystick::isConnected(0) ? std::wstring(L"A") : std::wstring(L"Space")) + tutorial_for_next_step/*" for next step)"*/, 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.movement_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_move/*"Try to move with WASD keys\nor left gamepad stick\n("*/ + std::to_wstring((int)move_points_left) + tutorial_move_pts_left/*" pts left)"*/, 0.01);
					move_points_left -= hero_object->getSpeed().getLength();
					if (move_points_left < 0) {
						tutorial.nextStep();
					}
				}
				if (tutorial.isWorkingOnStep(tutorial.switching_view_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_view/*"OK! Now let's try to change\ncamera mode with Q keyboard key\nor LB gamepad key\n("*/ + std::to_wstring(switching_view_counter) + tutorial_times_left/*" times left)"*/, 0.01);
					if (switching_view_counter == 0) {
						tutorial.nextStep();
					}
				}

				if (tutorial.isWorkingOnStep(tutorial.build_mode_dome_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_dome/*"Fly to the nearest asteroid\nand build dome on it\nwith key 1 on keyboard\nor Y on gamepad"*/, 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.build_mode_dome_description_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_dome_desc/*"Dome grants some regeneration\nfor all structures on same asteroid\n(Space or A to proceed)"*/, 0.01);
				}

				if (tutorial.isWorkingOnStep(tutorial.build_mode_turret_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_turret, 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.build_mode_turret_description_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_turret_desc, 0.01);
				}

				if (tutorial.isWorkingOnStep(tutorial.build_mode_gold_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_gold, 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.build_mode_gold_description_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_gold_desc, 0.01);
				}

				if (tutorial.isWorkingOnStep(tutorial.build_mode_science_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_science, 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.build_mode_science_description_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_science_desc, 0.01);
				}

				if (tutorial.isWorkingOnStep(tutorial.switching_modes_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_skill_mode + std::to_wstring(switching_modes_counter) + tutorial_times_left, 0.01);
					if (switching_modes_counter == 0) {
						tutorial.nextStep();
					}
				}

				if (tutorial.isWorkingOnStep(tutorial.using_skills_rocket_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_rocket, 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.using_skills_speed_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_speed, 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.using_skills_damage_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_damage, 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.using_skills_heal_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_heal, 0.01);
				}

				if (tutorial.isWorkingOnStep(tutorial.research_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_research, 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.pause_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_pause, 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.unpause_tutorial)) {
					gui_manager.forceSetTopSign(tutorial_unpause, 0.01);
				}
				if (tutorial.isWorkingOnStep(tutorial.tutorial_end)) {
					gui_manager.forceSetTopSign(tutorial_end, 0.01);
				}
			}
			
			if (is_game_cycle) {
				// set background
				window.clear(sf::Color::Black);
				background_manager.draw(window, window.getView().getCenter());

				is_game_cycle = visual_ctrl.processFrame(&window, game_map1.getObjectsBuffer());
				float hero_hp = hero_object->getUnitInfo()->getHealth() / hero_object->getUnitInfo()->getMaxHealth();

				float demolition_price = game_map1.getClosestAsteroid() == nullptr ? 0 : game_map1.getClosestAsteroid()->getAttachedPrice() / 2;
				is_game_cycle = gui_visual_ctrl.processFrame(&window, gui_manager.getObjectsBuffer(), gui_manager.getGUIText(), viewport_pos, hero_hp, demolition_price) && is_game_cycle;
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


			if (game_status != game_pause) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
					if (!gui_manager.processFrame(cursor_pos, viewport_pos)) {
						game_map1.processFrame(cursor_pos + viewport_pos, viewport_pos);
					}
				}
				else {
					if (!gui_manager.processFrame(Point(10000000, 100000000), viewport_pos)) {
						game_map1.processFrame(Point(10000000, 100000000), viewport_pos);
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
			if (gui_event.getData(0) != nullptr) {
				switch (gui_event.getEventType()) {
				case create_new:
					game_map1.addObject(static_cast<Object *>(gui_event.getData(0)), 0);
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
					switch (closest_asteroid->getObjectSpriteType()) {
					case asteroid:
						gui_manager.setTopSign(aster_basic_sign, 0.01);
						break;
					case asteroid_gold_interspersed_sprite:
						gui_manager.setTopSign(aster_gold_inter_sign, 0.01);
						break;
					case asteroid_iron_interspersed_sprite:
						gui_manager.setTopSign(aster_iron_inter_sign, 0.01);
						break;
					case asteroid_suspiciously_flat_sprite:
						gui_manager.setTopSign(aster_susp_flat_sign, 0.01);
						break;
					case asteroid_strange_cracked_sprite:
						gui_manager.setTopSign(aster_str_crack_sign, 0.01);
						break;
					case asteroid_ordinary_wealthy_sprite:
						gui_manager.setTopSign(aster_ord_wealthy_sign, 0.01);
						break;
					case asteroid_poor_mountainous_sprite:
						gui_manager.setTopSign(aster_poor_mount_sign, 0.01);
						break;
					case asteroid_wealthy_cracked_sprite:
						gui_manager.setTopSign(aster_wealthy_crack_sign, 0.01);
						break;
					case asteroid_ordinary_mountainous_sprite:
						gui_manager.setTopSign(aster_ord_mount_sign, 0.01);
						break;
					case asteroid_strange_poor_sprite:
						gui_manager.setTopSign(aster_str_poor_sign, 0.01);
						break;
					case asteroid_swampy_with_gold_mines_sprite:
						gui_manager.setTopSign(aster_swamp_gold_sign, 0.01);
						break;
					case asteroid_unstable_explosive_ore_sprite:
						gui_manager.setTopSign(aster_unstab_expl_sign, 0.01);
						break;
					case asteroid_old_laboratory_sprite:
						gui_manager.setTopSign(aster_old_lab_sign, 0.01);
						break;
					case asteroid_lava_surface_sprite:
						gui_manager.setTopSign(aster_lava_surf_sign, 0.01);
						break;
					case asteroid_drone_factory_sprite:
						gui_manager.setTopSign(aster_drone_sign, 0.01);
						break;
					case asteroid_rocket_launcher_sprite:
						gui_manager.setTopSign(aster_rocket_sign, 0.01);
						break;
					case asteroid_ancient_laboratory_sprite:
						gui_manager.setTopSign(aster_anc_lab_sign, 0.01);
						break;
					case asteroid_ancient_giant_gold_mine_sprite:
						gui_manager.setTopSign(aster_anc_mine_sign, 0.01);
						break;
					}
				}
			}
            bool move_activity = false;
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
                            move_activity = true;
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
                        move_activity = true;
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
					new_speed += Point(-1, 0);
					if (!(game_status == game_strategic_mode)) {
                        move_activity = true;
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
					new_speed += Point(0, 1);
					if (!(game_status == game_strategic_mode)) {
                        move_activity = true;
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
					new_speed += Point(1, 0);
					if (!(game_status == game_strategic_mode)) {
                        move_activity = true;
					}
				}

				if (game_status != game_pause) {
					if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
						new_speed = Point(static_cast<int>(sf::Mouse::getPosition().x) - static_cast<int>(window.getSize().x / 2), static_cast<int>(sf::Mouse::getPosition().y) - static_cast<int>(window.getSize().y / 2));
						hero_object->setAnimationType(move_anim); 
						hero_object->setAngle(atan2(new_speed.y, new_speed.x) / PI * 180);
					}
					if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
						Point mouse_vector = Point(static_cast<int>(sf::Mouse::getPosition().x) - static_cast<int>(window.getSize().x / 2), static_cast<int>(sf::Mouse::getPosition().y) - static_cast<int>(window.getSize().y / 2));
						//hero_object->setAngle(atan2(mouse_vector.y, mouse_vector.x) / PI * 180);
						if (skills_mode == set1) {
							if (hero_object->getUnitInfo()->attackReady(1)) {
								if (resource_manager.spendGold(consts.getAttackAbilityPrice())) {
									// hero attack 1 (mb rocket launch)
									shoot_rocket(hero_object);

									if (tutorial.isWorkingOnStep(tutorial.using_skills_rocket_tutorial)) {
										tutorial.nextStep();
									}
								}
							}
						}
					}
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
						else if(skills_mode == set2){
							skills_mode = set1;
							switching_modes_counter--;
						}
						last_mode_change = frame_num;
						last_build = frame_num - consts.getFPSLock();
					}
					if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) || sf::Joystick::isButtonPressed(0, Y)) && (frame_num - last_build) > fps.getFPS() / 4 /* 0.25 sec delay for changing view again */ && (tutorial.isWorkingOnStep(tutorial.build_mode_dome_tutorial) || tutorial.isWorkingOnStep(tutorial.using_skills_rocket_tutorial))) {
						if (skills_mode == set1) {
							if (hero_object->getUnitInfo()->attackReady(1)) {
								if (resource_manager.spendGold(consts.getAttackAbilityPrice())) {
									// hero attack 1 (mb rocket launch)
									shoot_rocket(hero_object);

									if (tutorial.isWorkingOnStep(tutorial.using_skills_rocket_tutorial)) {
										tutorial.nextStep();
									}
								}
							}
						}
						else if (skills_mode == set2) {
							// build dome(on your or empty asteroid), or start conversation with others
							if (game_map1.getClosestAsteroid()) {
								int type = game_map1.getClosestAsteroid()->getObjectSpriteType();
								if (type != asteroid_lava_surface_sprite) {
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
						else if (skills_mode == set2) {
							/*if (game_map1.getClosestAsteroid()) {
								int type = game_map1.getClosestAsteroid()->getObjectSpriteType();
								if (type != asteroid_drone_factory_sprite && 
									type != asteroid_swampy_with_gold_mines_sprite && 
									type != asteroid_rocket_launcher_sprite && 
									type != asteroid_old_laboratory_sprite && 
									type != asteroid_ancient_laboratory_sprite && 
									type != asteroid_ancient_giant_gold_mine_sprite) {

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
							}*/
                            if (game_map1.getClosestAsteroid()) {
                                int type = game_map1.getClosestAsteroid()->getObjectSpriteType();
                                if (type != asteroid_unstable_explosive_ore_sprite &&
                                    type != asteroid_old_laboratory_sprite &&
                                    type != asteroid_ancient_laboratory_sprite &&
                                    type != asteroid_ancient_giant_gold_mine_sprite) {

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
						else if (skills_mode == set2) {
							/*if (game_map1.getClosestAsteroid()) {
								int type = game_map1.getClosestAsteroid()->getObjectSpriteType();
								if (type != asteroid_unstable_explosive_ore_sprite &&
									type != asteroid_old_laboratory_sprite &&
									type != asteroid_ancient_laboratory_sprite &&
									type != asteroid_ancient_giant_gold_mine_sprite) {

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
							}*/
                            if (game_map1.getClosestAsteroid()) {
                                int type = game_map1.getClosestAsteroid()->getObjectSpriteType();
                                if (type != asteroid_drone_factory_sprite &&
                                    type != asteroid_swampy_with_gold_mines_sprite &&
                                    type != asteroid_rocket_launcher_sprite &&
                                    type != asteroid_old_laboratory_sprite &&
                                    type != asteroid_ancient_laboratory_sprite &&
                                    type != asteroid_ancient_giant_gold_mine_sprite) {

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
					}
					if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Num4) || sf::Joystick::isButtonPressed(0, A)) && (frame_num - last_build) > fps.getFPS() / 4 /* 0.25 sec delay for changing view again */ && (tutorial.isWorkingOnStep(tutorial.build_mode_science_tutorial) || tutorial.isWorkingOnStep(tutorial.using_skills_heal_tutorial))) {
						if (skills_mode == set1) {
							//// heal for asteroid and self
							//if (game_map1.getClosestAsteroid() != nullptr) {
							//	if (resource_manager.spendGold(consts.getHealBuffPrice())) {
							//		game_map1.setAsteroidBuff(Effect(1, const_heal), game_map1.getClosestAsteroid());
							//		game_map1.setAsteroidBuff(Effect(1200, regen_buff), game_map1.getClosestAsteroid());
							//		if (tutorial.isWorkingOnStep(tutorial.using_skills_heal_tutorial)) {
							//			tutorial.nextStep();
							//		}
							//	}
							//}
							//hero_object->setEffect(Effect(1, const_heal));
							//hero_object->setEffect(Effect(1200, regen_buff));
							//last_build = frame_num + consts.getFPSLock();
							
							// wipe all structures from asteroid
							if (game_map1.getClosestAsteroid() != nullptr) {
								resource_manager.addGold(game_map1.getClosestAsteroid()->getAttachedPrice() / 2);
								game_map1.getClosestAsteroid()->wipeAttached();
							}
						}
						else if (skills_mode == set2) {
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
                                            last_tutorial = frame_num;
										}
									}
									break;
								}
								last_build = frame_num;
							}
						}
						else if (game_map1.getClosestAsteroid()->getNPCInfo()) {
							game_status = dialog;
							last_dialog_choice = frame_num;
						}
					}
					if ((sf::Keyboard::isKeyPressed(sf::Keyboard::F) || sf::Joystick::isButtonPressed(0, BACK)) && (frame_num - last_research_open) > fps.getFPS() / 4 /* 0.25 sec delay for changing view again */ && tutorial.isWorkingOnStep(tutorial.research_tutorial)) {
						last_research_open = frame_num;
						prev_game_status = game_status;
						game_status = research; 
                        rank.addGameplayTime(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - round_start).count());
						if (tutorial.isWorkingOnStep(tutorial.research_tutorial)) {
							tutorial.nextStep();
						}
					}
				}
				if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || sf::Joystick::isButtonPressed(0, START)) && (frame_num - last_pause) > fps.getFPS() / 4 && (tutorial.isWorkingOnStep(tutorial.pause_tutorial) || tutorial.isWorkingOnStep(tutorial.unpause_tutorial))) {
					if (game_status != game_pause) {
						pause_game();
					}
				}
				

				if (tutorial.getCurrentStep() != tutorial.no_tutorial) {
					if (tutorial.isWorkingOnStep(tutorial.base_description) || tutorial.isWorkingOnStep(tutorial.build_mode_dome_description_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_turret_description_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_gold_description_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_science_description_tutorial) || tutorial.isWorkingOnStep(tutorial.tutorial_end)) {
						if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Joystick::isButtonPressed(0, A)) && (frame_num - last_tutorial) > fps.getFPS() / 4 /* 0.25 sec delay for changing view again */) {
							last_tutorial = frame_num;
                            last_build = frame_num;
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

			if (game_status == game_hero_mode) {
				if (hero_object->getUnitInfo()->isAffected(move_speed_buff)) {
					hero_speed *= consts.getMoveSpeedBuffMultiplier();
				}
				new_speed = new_speed.getNormal() * hero_speed;
				if (!(tutorial.isWorkingOnStep(tutorial.movement_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_dome_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_turret_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_gold_tutorial) || tutorial.isWorkingOnStep(tutorial.build_mode_science_tutorial))) {
					new_speed = Point();
				}
				if (hero_object != nullptr) {
					hero_object->changeSpeed(new_speed * consts.getFPSLock() / fps.getFPS());
					hero_object->setSpeed(hero_object->getSpeed() * std::pow(0.99, consts.getFPSLock() / fps.getFPS()));
					Point speed = hero_object->getSpeed();
					if (speed.getLength() > consts.getMaxHeroSpeed()) {
						hero_object->setSpeed(speed.getNormal() * consts.getMaxHeroSpeed());
					}
					hero_object->setAngle(atan2(speed.y, speed.x) / PI * 180);

                    if (hero_object->getSpeed().getLength() < 3 && !move_activity) {
                        hero_object->setAnimationType(hold_anim);
                    }
                    else {
                        hero_object->setAnimationType(move_anim);
                    }
				}
			}
			else if (game_status == game_strategic_mode) {
					
                if (sf::Joystick::isButtonPressed(0, LSTICK) || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    const double view2_speed_coef = 0.05;
                    Point hero_position = hero_object->getPosition();
                    Point diff = (hero_position - Point(view2.getCenter())) * view2_speed_coef * consts.getFPSLock() / fps.getFPS();
                    view2.setCenter(view2.getCenter() + sf::Vector2f(diff.x, diff.y));
                    strategic_back_pos = strategic_back_pos + sf::Vector2f(diff.x * 0.95, diff.y * 0.95);

                    background_manager.processFrame(diff, view2.getCenter());
                }
                else {
                    hero_object->setSpeed(Point());
                    //hero_object->setAnimationType(hold_anim);
                    new_speed = new_speed.getNormal() * consts.getStrategicCameraSpeed() * consts.getFPSLock() / fps.getFPS();
                    view2.setCenter(view2.getCenter() + sf::Vector2f(new_speed.x, new_speed.y));
                    strategic_back_pos = strategic_back_pos + sf::Vector2f(new_speed.x * 0.95, new_speed.y * 0.95);

                    background_manager.processFrame(new_speed, view2.getCenter());
                }
			}


			if (hero_object != nullptr) {
				const double view_speed_coef = 0.05;    // must be from 0 to 1, where 0 for static camera and 1 for camera istantly over hero
				Point hero_position = Point(hero_object->getPosition().x * window.getSize().x / 1920, hero_object->getPosition().y * window.getSize().y / 1080);
				Point diff = (hero_position - Point(view1.getCenter())) * view_speed_coef * consts.getFPSLock() / fps.getFPS();
				view1.setCenter(view1.getCenter() + sf::Vector2f(diff.x, diff.y));
				main_back_pos = main_back_pos + sf::Vector2f(diff.x * 0.95, diff.y * 0.95);

				background_manager.processFrame(diff, view1.getCenter());
			}

			if (!(game_status == game_strategic_mode || game_status == game_pause)) {
				window.setView(view1);
			}
			else {
				window.setView(view2);
			}

			if (game_status == game_pause) {
				bool is_input_state = false;
				for (int i = pause_continue; i <= pause_volume_up; i++) {
					if (i == chosen_button) {
						buttons[i].sprite.setTexture(buttons[i].texture_selected);
					}
					else {
						buttons[i].sprite.setTexture(buttons[i].texture_default);
					}
					buttons[i].sprite.setScale(sf::Vector2f(buttons[i].radius * 2 / buttons[i].sprite.getTexture()->getSize().x * window.getView().getSize().x / 1920, buttons[i].radius * 2 / buttons[i].sprite.getTexture()->getSize().y * window.getView().getSize().y / 1080));
					buttons[i].sprite.setOrigin(sf::Vector2f(buttons[i].sprite.getTexture()->getSize().x / 2, buttons[i].sprite.getTexture()->getSize().y / 2));
					buttons[i].sprite.setPosition(sf::Vector2f(buttons[i].pos.x * window.getView().getSize().x / 1920 + viewport_pos.x, buttons[i].pos.y * window.getView().getSize().y / 1080 + viewport_pos.y));
				}

				gui_manager.forceSetTopSign(keyboard_press_title + buttons[chosen_button].advice_string, 0.01);
				if (sf::Joystick::isConnected(0)) {
					gui_manager.forceSetTopSign(gamepad_press_title + buttons[chosen_button].advice_string, 0.01);
				}
				title.setOrigin(title.getGlobalBounds().width / 2, title.getGlobalBounds().height / 2);

                button_selector.processButtonBuffer(ButtonSelector::pause_buttons, cursor_pos);
                chosen_button = button_selector.getButtonList(ButtonSelector::pause_buttons).cur_index;

				if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || 
                    (sf::Joystick::isConnected(0) && sf::Joystick::isButtonPressed(0, A)) || 
                    (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus())) && 
                    window.hasFocus()) 
                {
					
					if (chosen_button == pause_continue) {
						is_input_state = true;
						unpause_game();
                        last_tutorial = frame_num;
					}
					if (chosen_button == pause_to_menu) {
						game_status = main_menu;
						return;
					}
					if (chosen_button == pause_to_desktop) {
						game_status = exit_to_desktop;
						return;
					}
                    if (chosen_button == pause_volume_down && (frame_num - last_volume_change) > fps.getFPS() / 4) {
                        music_manager.setVolume(std::max(0.0f, music_manager.getVolume() - 5.0f));
                        last_volume_change = frame_num;
                    }
                    if (chosen_button == pause_volume_up && (frame_num - last_volume_change) > fps.getFPS() / 4) {
                        music_manager.setVolume(std::min(100.0f, music_manager.getVolume() + 5.0f));
                        last_volume_change = frame_num;
                    }
				}
				for (int i = pause_continue; i <= pause_volume_up; i++) {
					window.draw(buttons[i].sprite);
				}
				window.draw(title);
			}
			
		}
		if (game_status == game_over) {
#ifdef __linux__ || __APPLE__  
#elif _WIN32
            if (sf::Joystick::isConnected(0)) {
                XINPUT_VIBRATION vibration;
                ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
                vibration.wLeftMotorSpeed = 0; // use any value between 0-65535 here
                vibration.wRightMotorSpeed = 0; // use any value between 0-65535 here
                XInputSetState(0, &vibration);
            }
#else
#endif

			window.setView(view4);
			ButtonList button_list = button_selector.getButtonList(ButtonSelector::game_over);
			for (int i = retry; i <= menu; i++) {
				if (i == chosen_button) {
					buttons_game_over[i].sprite.setTexture(buttons_game_over[i].texture_selected);
				}
				else {
					buttons_game_over[i].sprite.setTexture(buttons_game_over[i].texture_default);
				}
				buttons_game_over[i].sprite.setScale(sf::Vector2f(buttons_game_over[i].radius * 2 / buttons_game_over[i].sprite.getTexture()->getSize().x * window.getView().getSize().x / 1920, buttons_game_over[i].radius * 2 / buttons_game_over[i].sprite.getTexture()->getSize().y * window.getView().getSize().y / 1080));
				buttons_game_over[i].sprite.setOrigin(sf::Vector2f(buttons_game_over[i].sprite.getTexture()->getSize().x / 2, buttons_game_over[i].sprite.getTexture()->getSize().y / 2));
				buttons_game_over[i].sprite.setPosition(sf::Vector2f(window.getView().getSize().x / 2 + buttons_game_over[i].pos.x * window.getView().getSize().x / 1920, window.getView().getSize().y / 2 + buttons_game_over[i].pos.y * window.getView().getSize().y / 1080));
			}

			title.setString(keyboard_press_title + buttons_game_over[chosen_button].advice_string);
			if (sf::Joystick::isConnected(0)) {
				title.setString(gamepad_press_title + buttons_game_over[chosen_button].advice_string);
			}
			title.setOrigin(title.getGlobalBounds().width / 2, title.getGlobalBounds().height / 2);

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

				Point cursor_pos;
				sf::Vector2i mouse_pos = sf::Mouse::getPosition();
				sf::Vector2i window_pos = window.getPosition();
				cursor_pos = Point(mouse_pos.x - window.getPosition().x - window.getView().getSize().x / 2, mouse_pos.y - window.getPosition().y - window.getView().getSize().y / 2);
				for (int i = retry; i <= menu; i++) {
					if ((cursor_pos - buttons_game_over[i].pos).getLength() <= buttons_game_over[i].radius) {
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

			button_selector.processButtonBuffer(ButtonSelector::game_over, cursor_pos);
			chosen_button = button_selector.getButtonList(ButtonSelector::game_over).cur_index;

			window.clear(sf::Color::Black);
			background_manager.draw(window, Point());
			background_manager.processFrame(Point(1, 0), Point());
			window.draw(game_over_background_sprite);
			for (int i = retry; i <= menu; i++) {
				window.draw(buttons_game_over[i].sprite);
			}
			window.draw(title);
		}
        if (game_status == research) {

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::F) || sf::Joystick::isButtonPressed(0, BACK)) && (frame_num - last_research_open) > consts.getFPSLock() / 4 /* 0.25 sec delay for changing view again */) {
				if (tutorial.getCurrentStep() == tutorial.no_tutorial || (tutorial.getCurrentStep() == tutorial.research_tutorial_close && research_number > 0)) {
					tutorial.nextStep();
					last_research_open = frame_num;
					game_status = prev_game_status;
					round_start = std::chrono::steady_clock::now();
				}
            }
			if ((sf::Joystick::isButtonPressed(0, LB) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) && ((frame_num - last_menu_choice) > fps.getFPS() / 2) && game_mode == adventure_mode) {
				game_status = completed_mission_menu;
				last_menu_choice = frame_num;
			}

			if ((sf::Joystick::isButtonPressed(0, RB) || sf::Keyboard::isKeyPressed(sf::Keyboard::E)) && ((frame_num - last_menu_choice) > fps.getFPS() / 2) && game_mode == adventure_mode) {
				game_status = mission_menu;
				last_menu_choice = frame_num;
			}

			window.setView(view3);
            window.clear(sf::Color::Black); 
			background_manager.draw(window, Point());
			background_manager.processFrame(Point(1, 0), Point());

			res_visual_ctrl.processFrame(&window, view3.getCenter(), cur_research_index);

			button_selector.processButtonBuffer(ButtonSelector::research, Point());
			cur_research_index = button_selector.getButtonList(ButtonSelector::research).cur_index;
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Joystick::isButtonPressed(0, A) || (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus())) {
				if (!research_list[cur_research_index]->unlocked && research_manager.isResearchActive(cur_research_index)) {
					resource_manager.spendResearch(research_list[cur_research_index]->cost);
					research_list[cur_research_index]->unlocked = true;
					research_number++;
				}
			}

			const double view_speed_coef = 0.035;    // must be from 0 to 1, where 0 for static camera and 1 for camera istantly over hero
			Point research_pos = research_graph[cur_research_index]->pos * window.getView().getSize().y / 1080;
			Point diff = (research_pos - Point(view3.getCenter())) * view_speed_coef * consts.getFPSLock() / fps.getFPS();
			view3.setCenter(view3.getCenter() + sf::Vector2f(diff.x, diff.y));
        }
        if (game_status == dialog) {
            window.setView(view4);
            window.clear(sf::Color::Black);
            background_manager.draw(window, Point());
            background_manager.processFrame(Point(1, 0), Point());

            NPCInfo * npc = nullptr;
            if (game_map1.getClosestAsteroid()) {
                npc = static_cast<NPCInfo *>(game_map1.getClosestAsteroid()->getNPCInfo());
                dialog_visual_ctrl.processDialog(npc->getDialogInfo(), window, chosen_answer);

				bool is_input_state = false;
				bool is_keyboard_input = false;
				Point move_vector;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::D) || 
					sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {

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
					is_keyboard_input = true;
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

					is_input_state = true;

					int next_dialog_index = -1;
					double min_distance = 1500;

					for (int i = 0; i < npc->getDialogInfo().answers.size(); i++) {
						double angle_diff =
							(std::atan2(move_vector.x, move_vector.y) -
								std::atan2(
									0,
									i - chosen_answer));

						if (abs(angle_diff) > 0.000001) {     // angle_diff is not close to zero
							if (abs(angle_diff) > abs(angle_diff + 2 * PI)) {
								angle_diff += 2 * PI;
							}
							if (abs(angle_diff) > abs(angle_diff - 2 * PI)) {
								angle_diff -= 2 * PI;
							}
						}
						if (abs(angle_diff) <= PI / (is_keyboard_input ? 3.0 : 6.0)) {
							if ((next_dialog_index == -1 || ((Point(0, i) - Point(0, chosen_answer)).getLength() < (Point(0, next_dialog_index) - Point(0, chosen_answer)).getLength())) && (min_distance > (Point(0, i) - Point(0, chosen_answer)).getLength()) && (Point(0, i) - Point(0, chosen_answer)).getLength() > 0.01 /*not close to zero*/) {
								next_dialog_index = i;
								min_distance = (Point(0, i) - Point(0, chosen_answer)).getLength();
							}
						}
					}


					if (next_dialog_index != -1 && ((frame_num - last_dialog_choice) > fps.getFPS() / 2)) {
						chosen_answer = next_dialog_index;
						last_dialog_choice = frame_num;
					}
				}
				if (!is_input_state && !(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Joystick::isButtonPressed(0, A))) {
					last_dialog_choice = frame_num - fps.getFPS() - 1;
				}

				if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Joystick::isButtonPressed(0, A)) && ((frame_num - last_dialog_choice) > fps.getFPS())) {

					npc->nextTurn(npc->getDialogInfo().answers[chosen_answer].next_state_id);
					if (npc->getCurrentStage() == L"dialog_end") {
						game_status = game_hero_mode;
						last_build = frame_num;
					}
					last_dialog_choice = frame_num + 120;
					chosen_answer = 0;
				}
            }

            if (!game_map1.getClosestAsteroid() || !game_map1.getClosestAsteroid()->getNPCInfo()) {
                game_status = game_hero_mode;
            }
        }
		if (game_status == mission_menu) {
			window.setView(view4);
			window.clear(sf::Color::Black);
			background_manager.draw(window, Point());
			background_manager.processFrame(Point(1, 0), Point());

			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::F) || sf::Joystick::isButtonPressed(0, BACK)) && (frame_num - last_research_open) > consts.getFPSLock() / 4) {
				
				last_research_open = frame_num;
				game_status = prev_game_status;
				round_start = std::chrono::steady_clock::now();
			}

			if ((sf::Joystick::isButtonPressed(0, LB) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) && ((frame_num - last_menu_choice) > fps.getFPS() / 2)) {
				game_status = research;
				last_menu_choice = frame_num;
			}

			if ((sf::Joystick::isButtonPressed(0, RB) || sf::Keyboard::isKeyPressed(sf::Keyboard::E)) && ((frame_num - last_menu_choice) > fps.getFPS() / 2)) {
				game_status = completed_mission_menu;
				last_menu_choice = frame_num;
			}
			int chosen_menu = 0;

			button_selector.processButtonBuffer(ButtonSelector::mission_list, cursor_pos);
			chosen_menu = button_selector.getButtonList(ButtonSelector::mission_list).cur_index;
			mission_visual_ctrl.processMissionList(window, chosen_menu);

			if ((sf::Joystick::isButtonPressed(0, A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) && ((frame_num - last_menu_choice) > fps.getFPS() / 2)) {
				rpg_profile.setCurrentMission(chosen_menu);
			}
		}
		if (game_status == completed_mission_menu) {
			window.setView(view4);
			window.clear(sf::Color::Black);
			background_manager.draw(window, Point());
			background_manager.processFrame(Point(1, 0), Point());

			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::F) || sf::Joystick::isButtonPressed(0, BACK)) && (frame_num - last_research_open) > consts.getFPSLock() / 4) {

				last_research_open = frame_num;
				game_status = prev_game_status;
				round_start = std::chrono::steady_clock::now();
			}

			if ((sf::Joystick::isButtonPressed(0, LB) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) && ((frame_num - last_menu_choice) > fps.getFPS() / 2)) {
				game_status = mission_menu;
				last_menu_choice = frame_num;
			}

			if ((sf::Joystick::isButtonPressed(0, RB) || sf::Keyboard::isKeyPressed(sf::Keyboard::E)) && ((frame_num - last_menu_choice) > fps.getFPS() / 2)) {
				game_status = research;
				last_menu_choice = frame_num;
			}
			int chosen_menu = 0;
			button_selector.processButtonBuffer(ButtonSelector::completed_mission_list, cursor_pos);
			chosen_menu = button_selector.getButtonList(ButtonSelector::completed_mission_list).cur_index;
			mission_visual_ctrl.processCompletedMissionList(window, chosen_menu);
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
				//pause_game();
			}
			if (event.type == sf::Event::MouseWheelScrolled) {
				music_manager.setVolume(std::max(std::min(100.0f, music_manager.getVolume() + event.mouseWheelScroll.delta), 0.0f));
			}
		}
	}
    if (!window.isOpen()) {
        game_status = exit_to_desktop;
    }
}

int main() {
#ifdef __linux__ || __APPLE__  
#elif _WIN32
	HWND console_hWnd = GetConsoleWindow();
	//ShowWindow(console_hWnd, SW_HIDE);
#else
#endif

	phrase_container.parseFromFolder("text_config" + path_separator + settings.getLocalisationFile());

	sf::ContextSettings context_settings;
	context_settings.antialiasingLevel = 8;
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
#ifdef __linux__ || __APPLE__ 
    sf::RenderWindow window(sf::VideoMode(settings.getWindowWidth(), settings.getWindowHeight()), "AsteroidTD", sf::Style::Fullscreen, context_settings);
#elif _WIN32
    sf::RenderWindow window(sf::VideoMode(settings.getWindowWidth(), settings.getWindowHeight()), "AsteroidTD", sf::Style::None, context_settings);
#else
#endif

	sf::View main_view(sf::Vector2f((int)window.getSize().x / 2, (int)window.getSize().y / 2), sf::Vector2f(window.getSize().x, window.getSize().y));      // main menu view
	window.setView(main_view);

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
#ifdef __linux__ || __APPLE__  
	Display * xdisplay = XOpenDisplay(NULL);
	Screen * screen_info = DefaultScreenOfDisplay(xdisplay);
	int screenW = screen_info->width;
	int screenH = screen_info->height;
#elif _WIN32
	int screenW = GetSystemMetrics(SM_CXSCREEN);
	int screenH = GetSystemMetrics(SM_CYSCREEN);
#else
#endif
	window.setSize(sf::Vector2u(screenW, screenH));
	window.setPosition(sf::Vector2i(0, 0));

	VisualController visual_ctrl;
	GUIVisualController gui_visual_ctrl;
	ResearchVisualController res_visual_ctrl(&window);

	sf::Texture menu_background_texture;
	menu_background_texture.loadFromFile("background/menu_background.png");
	sf::Sprite menu_background_sprite;
	menu_background_sprite.setTexture(menu_background_texture);
	menu_background_sprite.setPosition(0, 0);
	menu_background_sprite.setScale(sf::Vector2f(window.getView().getSize().x / menu_background_texture.getSize().x, window.getView().getSize().y / menu_background_texture.getSize().y));

	sf::Text title;
	sf::Text nickname_title;
	sf::Font base_font;
	base_font.loadFromFile(consts.getDefaultFontPath());

	title.setPosition(sf::Vector2f(window.getView().getSize().x / 2, 150 * window.getView().getSize().y / 1080));
	title.setFillColor(sf::Color::White);
	title.setOutlineColor(sf::Color::Black);
	title.setOutlineThickness(1);
	title.setCharacterSize(40 * window.getView().getSize().y / 1080);
	title.setFont(base_font);

	nickname_title.setPosition(sf::Vector2f(window.getSize().x / 2 - 25, window.getSize().y / 2 - 200));
	nickname_title.setFillColor(sf::Color::White);
	nickname_title.setOutlineColor(sf::Color::Black);
	nickname_title.setOutlineThickness(1);
	nickname_title.setCharacterSize(40 * window.getView().getSize().y / 1080);
	nickname_title.setFont(base_font);

	game_status = main_menu;

	// main menu
	struct MenuButton {
		Point pos;
		sf::Texture texture_default, texture_selected;
		sf::Sprite sprite;
		float radius;
		std::wstring advice_string;
	};
	enum buttons_name {
		infinity_mode_button,
        /*adventure_mode_button,*/
		settings_button,
		shutdown_button,

		BUTTONS_NAME_LIST_SIZE
	};
	std::vector<MenuButton> buttons;
	buttons.resize(BUTTONS_NAME_LIST_SIZE);

	buttons[infinity_mode_button].pos = Point(window.getView().getSize().x / 2 - 250 * window.getView().getSize().x / 1920, 400 * window.getView().getSize().y / 1080);
	buttons[infinity_mode_button].texture_default.loadFromFile("menu_buttons" + path_separator + "inf_mode.png");
	buttons[infinity_mode_button].texture_selected.loadFromFile("menu_buttons" + path_separator + "inf_mode_selected.png");
	buttons[infinity_mode_button].sprite.setTexture(buttons[infinity_mode_button].texture_default);
	buttons[infinity_mode_button].radius = 75 * window.getView().getSize().y / 1080;
    std::vector<std::wstring> buffer = phrase_container.getPhraseBuffer(L"start_inf_mode_GUI", 0);
	buttons[infinity_mode_button].advice_string = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

    /*buttons[adventure_mode_button].pos = Point(window.getView().getSize().x / 2 - 100 * window.getView().getSize().x / 1920, 400 * window.getView().getSize().y / 1080);
    buttons[adventure_mode_button].texture_default.loadFromFile("menu_buttons" + path_separator + "adv_mode.png");
    buttons[adventure_mode_button].texture_selected.loadFromFile("menu_buttons" + path_separator + "adv_mode_selected.png");
    buttons[adventure_mode_button].sprite.setTexture(buttons[adventure_mode_button].texture_default);
    buttons[adventure_mode_button].radius = 75 * window.getView().getSize().y / 1080;
    buffer = phrase_container.getPhraseBuffer(L"start_adv_mode_GUI", 0);
    buttons[adventure_mode_button].advice_string = buffer[rand() * buffer.size() / (RAND_MAX + 1)];*/

	buttons[settings_button].pos = Point(window.getView().getSize().x / 2 + 0 * window.getView().getSize().x / 1920, 400 * window.getView().getSize().y / 1080);
	buttons[settings_button].texture_default.loadFromFile("menu_buttons" + path_separator + "settings.png");
	buttons[settings_button].texture_selected.loadFromFile("menu_buttons" + path_separator + "settings_selected.png");
	buttons[settings_button].sprite.setTexture(buttons[settings_button].texture_default);
	buttons[settings_button].radius = 75 * window.getView().getSize().y / 1080;
    buffer = phrase_container.getPhraseBuffer(L"settings_menu_GUI", 0);
	buttons[settings_button].advice_string = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

	buttons[shutdown_button].pos = Point(window.getView().getSize().x / 2 + 250 * window.getView().getSize().x / 1920, 400 * window.getView().getSize().y / 1080);
	buttons[shutdown_button].texture_default.loadFromFile("menu_buttons" + path_separator + "shutdown.png");
	buttons[shutdown_button].texture_selected.loadFromFile("menu_buttons" + path_separator + "shutdown_selected.png");
	buttons[shutdown_button].sprite.setTexture(buttons[shutdown_button].texture_default);
	buttons[shutdown_button].radius = 75 * window.getView().getSize().y / 1080;
    buffer = phrase_container.getPhraseBuffer(L"shutdown_GUI", 0);
	buttons[shutdown_button].advice_string = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

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

	MusicManager music_manager("music");
	music_manager.launchMusicWorker();

	std::vector<Point> buttons_pos;
	for (int i = 0; i < buttons.size(); i++) {
		buttons_pos.push_back(buttons[i].pos);
	}
	button_selector.initButtonList(ButtonSelector::main_menu, 1000, PI / 4, buttons_pos, buttons[shutdown_button].radius);

	background_manager.generateAroundCenter(Point());

    std::wstring
        setting_nickname_title,
        setting_ranking_title,
        setting_win_width_title,
        setting_win_height_title,
        setting_volume_title,
        setting_local_file_title,
        setting_discard_title,
        setting_exit_title,
        keyboard_press_title,
        gamepad_press_title,
        keyboard_nickname_title,
        gamepad_nickname_title,
        nickname_enter_title;

    auto resetStrings = [&]() {
        buffer = phrase_container.getPhraseBuffer(L"setting_nickname_title_GUI", 0);
        setting_nickname_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"setting_ranking_title_GUI", 0);
        setting_ranking_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"setting_win_width_title_GUI", 0);
        setting_win_width_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"setting_win_height_title_GUI", 0);
        setting_win_height_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"setting_volume_title_GUI", 0);
        setting_volume_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"setting_local_file_title_GUI", 0);
        setting_local_file_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"setting_discard_title_GUI", 0);
        setting_discard_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"setting_exit_title_GUI", 0);
        setting_exit_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"keyboard_press_title_GUI", 0);
        keyboard_press_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"gamepad_press_title_GUI", 0);
        gamepad_press_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"keyboard_nickname_title_GUI", 0);
        keyboard_nickname_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"gamepad_nickname_title_GUI", 0);
        gamepad_nickname_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        buffer = phrase_container.getPhraseBuffer(L"nickname_enter_title_GUI", 0);
        nickname_enter_title = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
    };

    resetStrings();

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
				return 0;
			}
			if (event.type == sf::Event::MouseWheelScrolled) {
				music_manager.setVolume(std::max(std::min(100.0f, music_manager.getVolume() + event.mouseWheelScroll.delta), 0.0f));
			}
		}

		frame_num++;

		window.setView(main_view);
#ifdef __linux__ || __APPLE__  
#elif _WIN32
        if (sf::Joystick::isConnected(0)) {
            XINPUT_VIBRATION vibration;
            ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
            vibration.wLeftMotorSpeed = 0; // use any value between 0-65535 here
            vibration.wRightMotorSpeed = 0; // use any value between 0-65535 here
            XInputSetState(0, &vibration);
        }
#else
#endif

		if (window.hasFocus()) {
            Point cursor_pos = sf::Mouse::getPosition();

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

				title.setString(keyboard_press_title + buttons[chosen_button].advice_string);
				if (sf::Joystick::isConnected(0)) {
					title.setString(gamepad_press_title + buttons[chosen_button].advice_string);
				}
				title.setOrigin(title.getGlobalBounds().width / 2, title.getGlobalBounds().height / 2);

				bool is_input_state = false;
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

					Point cursor_pos;
					sf::Vector2i mouse_pos = sf::Mouse::getPosition();
					sf::Vector2i window_pos = window.getPosition();
					cursor_pos = Point(mouse_pos.x - window.getPosition().x, mouse_pos.y - window.getPosition().y);
					for (int i = 0; i < buttons.size(); i++) {
						if ((cursor_pos - buttons[i].pos).getLength() <= buttons[i].radius) {
							if (i == infinity_mode_button) {
								game_status = game_hero_mode;
                                game_mode = GameMode::infinity_mode;
								is_input_state = true;
							}
                            /*if (i == adventure_mode_button) {
                                game_status = game_hero_mode;
                                game_mode = GameMode::adventure_mode;
                                is_input_state = true;
                            }*/
							if (i == settings_button) {
								game_status = settings_menu;
                                resetStrings();
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
				if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || (sf::Joystick::isConnected(0) && sf::Joystick::isButtonPressed(0, A)))) {
					if ((frame_num - last_menu_choice) > consts.getFPSLock() / 4) {
						if (chosen_button == infinity_mode_button) {
							game_status = game_hero_mode;
							is_input_state = true;
						}
                        /*if (chosen_button == adventure_mode_button) {
                            game_status = game_hero_mode;
                            game_mode = GameMode::adventure_mode;
                            is_input_state = true;
                        }*/
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
					is_input_state = true;
				}

				button_selector.processButtonBuffer(ButtonSelector::main_menu, cursor_pos);
				chosen_button = button_selector.getButtonList(ButtonSelector::main_menu).cur_index;

				window.clear(sf::Color::Black);
				background_manager.draw(window, Point());
				background_manager.processFrame(Point(1, 0), Point());
				window.draw(menu_background_sprite);
				for (int i = 0; i < buttons.size(); i++) {
					window.draw(buttons[i].sprite);
				}
				window.draw(title);
				window.display();
			}
			else if (game_status == settings_menu) {
				window.clear(sf::Color::Black);
				background_manager.draw(window, Point());
				background_manager.processFrame(Point(1, 0), Point());
				window.draw(menu_background_sprite);
				for (int i = 0; i < settings_list.size(); i++) {
					if (settings_list[i] == "nickname") {
						settings_sign.setString(setting_nickname_title);
					}
					if (settings_list[i] == "ranking_server") {
                        settings_sign.setString(setting_ranking_title);
					}
					if (settings_list[i] == "window_width") {
                        settings_sign.setString(setting_win_width_title);
					}
                    if (settings_list[i] == "window_height") {
                        settings_sign.setString(setting_win_height_title);
                    }
                    if (settings_list[i] == "volume") {
                        settings_sign.setString(setting_volume_title);
                    }
                    if (settings_list[i] == "localisation_file") {
                        settings_sign.setString(setting_local_file_title);
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

                    title.setString(setting_exit_title);
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
                            else if (settings_list[chosen_setting] == "volume") {
                                music_manager.setVolume(settings.getVolume());
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

                    title.setString(setting_discard_title);
				}
				if (!key_pressed) {
					last_menu_choice = frame_num - fps.getFPS();
				}
				shift = shift + (chosen_setting - shift) * 0.05;
				title.setOrigin(title.getGlobalBounds().width / 2, title.getGlobalBounds().height / 2);
				window.draw(title);
				window.display();
			}
			else if (game_status == exit_to_desktop) {
                music_manager.shutdown();
				return 0;
			}
			else if (game_status == nickname_enter) {

                title.setString(keyboard_nickname_title);

				if (sf::Joystick::isConnected(0)) {
                    title.setString(gamepad_nickname_title);
				}

				if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || (sf::Joystick::isConnected(0) && sf::Joystick::isButtonPressed(0, A))) && (frame_num - last_menu_choice) > consts.getFPSLock()) {
					settings.setNickname(keyboard_buffer.getBuffer());
					keyboard_buffer.activate();
					keyboard_buffer.clearBuffer();
					game_status = main_menu;
					continue;
				}
                std::string kbrd_buf = keyboard_buffer.getBuffer();
				nickname_title.setString(nickname_enter_title + std::wstring(kbrd_buf.begin(), kbrd_buf.end()));
				title.setOrigin(title.getGlobalBounds().width / 2, title.getGlobalBounds().height / 2);
				nickname_title.setOrigin(title.getGlobalBounds().width / 2, title.getGlobalBounds().height / 2);

				window.clear(sf::Color::Black);
				background_manager.draw(window, Point());
				background_manager.processFrame(Point(1, 0), Point());
				window.draw(menu_background_sprite);
				window.draw(title);
				window.draw(nickname_title);
				window.display();
			}
			else {
				fps.reset();
				try {
                    rpg_profile.clear();
					gameCycle("map2", window, visual_ctrl, gui_visual_ctrl, res_visual_ctrl, music_manager);
					last_menu_choice = frame_num + consts.getFPSLock();
				}
				catch (std::exception exc) {
					std::cout << "Exception handled" << std::endl;
				}
			}
		}
	}
	return 0;
}