#pragma once

#include <vector>

enum WorldFactionList {
	Alliance_of_Ancient_Knowledge,
	Trade_Federation,
	Church_of_Holy_Asteroid,
	Space_Empire,
	Travellers_Guild,
	Scientists_Union,

	WORLD_FACTIONS_COUNT
};

std::vector<std::vector<float>> relations_graph = {
	{ 1.0f, -0.2f, -0.3f, -0.3f, -0.2f,  0.0f },
	{ -0.2f,  1.0f, -0.1f, -0.6f,  0.2f, -0.3f },
	{ -0.3f, -0.1f,  1.0f,  0.8f, -0.6f, -0.8f },
	{ -0.3f, -0.6f,  0.8f,  1.0f, -0.7f, -0.2f },
	{ -0.2f,  0.2f, -0.6f, -0.7f,  1.0f,  0.3f },
	{ 0.0f, -0.3f, -0.8f, -0.2f,  0.3f,  1.0f }
};

float getRelations(int fact1, int fact2) {
	if (fact1 < 0 || fact1 >= WORLD_FACTIONS_COUNT || fact2 < 0 || fact2 >= WORLD_FACTIONS_COUNT) {
		return 0;
	}
	return relations_graph[fact1][fact2];
}