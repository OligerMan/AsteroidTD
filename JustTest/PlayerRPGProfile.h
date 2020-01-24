#pragma once

#include <vector>
#include <algorithm>

#include "Mission.h"

enum WorldFactionList {
	Alliance_of_Ancient_Knowledge,
	Trade_Federation,
	Church_of_Holy_Asteroid,
	Space_Empire,
	Travellers_Guild,
	Scientists_Union,

	WORLD_FACTIONS_COUNT
};

class PlayerRPGProfile {
	std::vector<std::vector<float>> relations_graph = {
		{  1.0f, -0.2f, -0.3f, -0.3f, -0.2f,  0.0f },
		{ -0.2f,  1.0f, -0.1f, -0.6f,  0.2f, -0.3f },
		{ -0.3f, -0.1f,  1.0f,  0.8f, -0.6f, -0.8f },
		{ -0.3f, -0.6f,  0.8f,  1.0f, -0.7f, -0.2f },
		{ -0.2f,  0.2f, -0.6f, -0.7f,  1.0f,  0.3f },
		{  0.0f, -0.3f, -0.8f, -0.2f,  0.3f,  1.0f }
	};

	std::vector<float> faction_fame_list = { 0, 0, 0, 0, 0, 0 };
	float global_fame = 0;
	std::vector<Mission> mission_list;

public:

	void changeFactionFame(WorldFactionList fact, float shift) {
		if (fact < 0 || fact >= WORLD_FACTIONS_COUNT) {
			return;
		}
		for (int i = 0; i < WORLD_FACTIONS_COUNT; i++) {
			faction_fame_list[i] = std::max(-100.0f, std::min(100.0f, faction_fame_list[i] - shift * relations_graph[i][fact]));
		}
	}

	void changeGlobalFame(float shift) {
		global_fame = std::max(-100.0f, std::min(100.0f, global_fame - shift));
	}

	float getRelations(int fact1, int fact2) {
		if (fact1 < 0 || fact1 >= WORLD_FACTIONS_COUNT || fact2 < 0 || fact2 >= WORLD_FACTIONS_COUNT) {
			return 0;
		}
		return relations_graph[fact1][fact2];
	}

	float getFactionFame(WorldFactionList fact) {
		if (fact < 0 || fact >= WORLD_FACTIONS_COUNT) {
			return 0;
		}
		return faction_fame_list[fact];
	}

	float getGlobalFame() {
		return global_fame;
	}

	void addMission(Mission new_mission) {
		mission_list.push_back(new_mission);
	}

	std::vector<Mission> getMissionList() {
		return mission_list;
	}
} rpg_profile;