#pragma once

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

		STEPS_COUNT
	};

private:

	int cur_step = base_description;

public:

	int getCurrentStep() {
		return cur_step;
	}

	bool isWorkingOnStep(step current_step) {  // need this to unlock some steps of tutorial and not to lock if tutorial isn't activated
		return (cur_step == current_step) || (cur_step == no_tutorial);
	}
	
	void nextStep() {
		cur_step++;
		if (cur_step == STEPS_COUNT || cur_step == base_description) {
			cur_step = no_tutorial;
		}
	}
} tutorial;