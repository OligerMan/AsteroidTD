#pragma once

#include "ResourceManager.h"

#include <vector>

enum ResearchList {
	test_research1,
	test_research2,
	test_research3,
	test_research4,

	RESEARCH_COUNT
};

struct Research {
	std::vector<ResearchList> prev;
	ResearchList type;
	bool unlocked = false;
	float cost = 1;
};

class ResearchManager {
	std::vector<Research> research;

public:

	bool unlockResearch(ResearchList research_name) {
		if (res_manager.spendResearch(research[research_name].cost)) {
			research[research_name].unlocked = true;
			return true;
		}
		else {
			return false;
		}
	}

	Research getResearch(ResearchList research_name) {
		return research[(int)research_name];
	}

	std::vector<Research> * getResearchArray() {
		return &research;
	}

	void initResearch() {
		research.resize(RESEARCH_COUNT);

		for (int i = 0; i < RESEARCH_COUNT; i++) {
			research[i].type = (ResearchList)i;
		}

		research[test_research1].cost = 2;
		research[test_research2].cost = 3;
		research[test_research3].cost = 5;
		research[test_research4].cost = 7;

		// set the researches, which is needed for this research
		research[test_research3].prev = std::vector<ResearchList>{
			test_research1,
		};
		research[test_research4].prev = std::vector<ResearchList>{
			test_research1,
			test_research2
		};
	}
};