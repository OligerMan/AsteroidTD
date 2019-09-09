#pragma once

enum GameStatus {
	menu,
	game_hero_mode,
	game_strategic_mode,
	research,
	pause,
	game_over
};

enum SkillsMode {
	set1,
	set2
};

GameStatus game_status;
SkillsMode skills_mode;