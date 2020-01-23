#pragma once

enum GameStatus {
	main_menu,
	nickname_enter,
	game_hero_mode,
	game_strategic_mode,
	research,
	pause,
    dialog,
	mission_menu,
	game_over,
	settings_menu,
	exit_to_desktop
};

enum SkillsMode {
	set1,
	set2,
	npc_dialog,
};

GameStatus game_status;
SkillsMode skills_mode;