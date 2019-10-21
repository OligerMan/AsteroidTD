#pragma once

enum GameStatus {
	main_menu,
	game_hero_mode,
	game_strategic_mode,
	research,
	pause,
	game_over,
	settings_list,
	exit_to_desktop
};

enum SkillsMode {
	set1,
	set2
};

GameStatus game_status;
SkillsMode skills_mode;