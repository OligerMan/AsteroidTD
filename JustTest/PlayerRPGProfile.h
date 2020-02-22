#pragma once

#include <vector>
#include <algorithm>

#include "Mission.h"
#include "Object.h"
#include "FameInfo.h"

class PlayerRPGProfile {

	std::vector<Mission> mission_list;
	std::vector<Mission> completed_mission_list;
	int current_mission = 0;

	void resetButtonList() {
		std::vector<Point> pos_buffer;
		for (int i = 0; i < mission_list.size(); i++) {
			pos_buffer.push_back(Point(0, i));
		}
		button_selector.initButtonList(ButtonSelector::mission_list, 300, PI / 3, pos_buffer);
		pos_buffer.clear();
		for (int i = 0; i < completed_mission_list.size(); i++) {
			pos_buffer.push_back(Point(0, i));
		}
		button_selector.initButtonList(ButtonSelector::completed_mission_list, 300, PI / 3, pos_buffer);
	}

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
		resetButtonList();
	}

	std::vector<Mission> getMissionList() {
		return mission_list;
	}

	std::vector<Mission> getCompletedMissionList() {
		return completed_mission_list;
	}

	std::vector<Point> getCurrentCoordinatesList() {
		std::vector<Point> output;
		if (current_mission >= 0 && current_mission < mission_list.size()) {
            Object * obj_ptr;
            void * objective;
            switch (mission_list[current_mission].getObjective().type) {
            case Objective::point:
                objective = mission_list[current_mission].getObjective().data;
                if (objective) {
                    output.push_back(((Object *)objective)->getPosition());
                }
                break;
            }
		}
		return output;
	}

	int getCurrentMissionNumber() {
		return current_mission;
	}

    Mission & getCurrentMission() {
        Mission output(L"", 0);
		if (current_mission >= 0 && current_mission < mission_list.size()) {
			output = mission_list[current_mission];
		}
		return output;
	}

	void setCurrentMission(int new_current_mission) {
		if (new_current_mission < 0 || new_current_mission >= mission_list.size()) {
			return;
		}
		current_mission = new_current_mission;
	}

	void processCompletedMissions() {
		for (int i = 0; i < mission_list.size(); i++) {
			if (mission_list[i].completed()) {
				completed_mission_list.push_back(mission_list[i]);
				mission_list.erase(mission_list.begin() + i);
				resetButtonList();
			}
		}
	}

} rpg_profile;