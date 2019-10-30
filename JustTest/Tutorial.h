#pragma once

class Tutorial {
public:

	enum step{
		no_tutorial,
		base_description,
		movement_tutorial,
		switching_view_tutorial,
		build_mode_tutorial,
		switching_modes_tutorial,
		using_skills_tutorial,
		research_tutorial,
		research_tutorial_close,
		pause_tutorial,
		unpause_tutorial,

		STEPS_COUNT
	};

private:

	int cur_step = no_tutorial;

public:

	int getCurrentStep() {
		return cur_step;
	}

	bool isWorkingOnStep(step current_step) {  // need this to unlock some steps of tutorial and not to lock if tutorial isn't activated
		return (cur_step == current_step) || (current_step == no_tutorial);
	}
	
	void nextStep() {
		cur_step++;
		if (cur_step == STEPS_COUNT) {
			cur_step = no_tutorial;
		}
	}
} tutorial;