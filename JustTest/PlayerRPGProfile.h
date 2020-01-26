#pragma once

#include <vector>
#include <algorithm>

#include "Mission.h"
#include "Object.h"
#include "FameInfo.h"

class PlayerRPGProfile {

	
	std::vector<Mission> mission_list;
	int current_mission = 0;

public:

	void changeFactionFame(WorldFactionList fact, float shift) {
		fame_info.changeFactionFame(fact, shift);
	}

	void changeGlobalFame(float shift) {
		fame_info.changeGlobalFame(shift);
	}

	float getFactionFame(WorldFactionList fact) {
		return fame_info.getFactionFame(fact);
	}

	float getGlobalFame() {
		return fame_info.getGlobalFame();
	}

	void addMission(Mission new_mission) {
		mission_list.push_back(new_mission);

		std::vector<Point> pos_buffer;
		for (int i = 0; i < mission_list.size(); i++) {
			pos_buffer.push_back(Point(0, i));
		}
		button_selector.initButtonList(ButtonSelector::mission_list, 300, PI / 3, pos_buffer);
	}

	std::vector<Mission> getMissionList() {
		return mission_list;
	}

	std::vector<Point> getCurrentCoordinatesList() {
		std::vector<Point> output;
		if (current_mission >= 0 && current_mission < mission_list.size()) {
			switch (mission_list[current_mission].type) {
			case Mission::courier:
				Objective objective = mission_list[current_mission].getObjective();
				Object * obj_ptr = (Object *)((PointObjective *)objective.objectiveExpansion)->object_ptr;
				output.push_back(obj_ptr->getPosition());
				break;
			};
		}
		return output;
	}

	int getCurrentMission() {
		return current_mission;
	}

	void setCurrentMission(int new_current_mission) {
		if (new_current_mission < 0 || new_current_mission >= mission_list.size()) {
			return;
		}
		current_mission = new_current_mission;
	}

} rpg_profile;