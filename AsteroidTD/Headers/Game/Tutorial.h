#pragma once

#include <fstream>
#include "../../ResourcePath.hpp"

class Tutorial {
public:

	enum step{
		no_tutorial,
		base_description,
		movement_tutorial,
		switching_view_tutorial,
		build_mode_dome_tutorial,
		build_mode_dome_description_tutorial,
		build_mode_turret_tutorial,
		build_mode_turret_description_tutorial,
		build_mode_gold_tutorial,
		build_mode_gold_description_tutorial,
		build_mode_science_tutorial,
		build_mode_science_description_tutorial,
		switching_modes_tutorial,
		using_skills_rocket_tutorial,
		using_skills_speed_tutorial,
		using_skills_damage_tutorial,
		using_skills_heal_tutorial,
		research_tutorial,
		research_tutorial_close,
		pause_tutorial,
		unpause_tutorial,
		tutorial_end,

		STEPS_COUNT
	};

private:

	int cur_step = base_description;

public:

	Tutorial() {
		std::ifstream tutorial;
		tutorial.open(resourcePath() + "tutorial.passed");
		if (tutorial.good()) {
			cur_step = no_tutorial;
		}
		tutorial.close();
	}

	int getCurrentStep() {
		return cur_step;
	}

	bool isWorkingOnStep(step current_step) {  // need this to unlock some steps of tutorial and not to lock if tutorial isn't activated
		return (cur_step == current_step) || (cur_step == no_tutorial);
	}
	
	void nextStep() {
		cur_step++;
		if (cur_step == STEPS_COUNT){

			std::ofstream tutorial_msg;
			tutorial_msg.open(resourcePath() + "tutorial.passed", std::ofstream::out);
			tutorial_msg << "Congratulations!";
			tutorial_msg.close();

			cur_step = no_tutorial;
		}
		if (cur_step == base_description) {
			cur_step = no_tutorial;
		}
	}
} tutorial;
