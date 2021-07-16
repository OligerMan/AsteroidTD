#pragma once

#include <algorithm>

#include "WorldFactions.h"

class FameInfo {
	std::vector<float> faction_fame_list = { 0, 0, 0, 0, 0, 0 };
	float global_fame = 0;

public:

	float getGlobalFame() {
		return global_fame;
	}

	void changeGlobalFame(float shift) {
		global_fame = std::max(-100.0f, std::min(100.0f, global_fame - shift));
	}

	float getFactionFame(WorldFactionList faction) {
		if (faction < 0 || faction >= WORLD_FACTIONS_COUNT) {
			return 0;
		}
		return faction_fame_list[faction];
	}

	void changeFactionFame(WorldFactionList fact, float shift) {
		if (fact < 0 || fact >= WORLD_FACTIONS_COUNT) {
			return;
		}
		for (int i = 0; i < WORLD_FACTIONS_COUNT; i++) {
			faction_fame_list[i] = std::max(-100.0f, std::min(100.0f, faction_fame_list[i] - shift * relations_graph[i][fact]));
		}
	}
} fame_info;