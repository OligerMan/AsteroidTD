#pragma once

#include "ResourceManager.h"
#include "GeometryComponents.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

enum ResearchList {
	test_research1,
	test_research2,
	test_research3,
	test_research4,

	RESEARCH_COUNT
};


struct Research {
	ResearchList type;
	bool unlocked = false;
	float cost = 1;
};

struct ResearchNode {
    ResearchList type;
    Point pos;
    std::vector<ResearchList> parents;
};

class ResearchManager {
	std::vector<Research> research;
    std::vector<ResearchNode> graph;

    std::vector<std::string> research_name = {
        ""
    };

    ResearchList stringToResearch(std::string string) {
        for (int i = 0; i < research_name.size(); i++) {
            if (research_name[i] == string) {
                return (ResearchList)i;
            }
        }
        return RESEARCH_COUNT;
    }

    std::string researchTypeToString(ResearchList type) {
        if (type >= 0 && type < research_name.size()) {
            return research_name[type];
        }
        return "";
    }

    /*
    Let's define research interpretation in text

    type <research_type>
    cost <cost>
    pos <coor x> <coor y>
    parent start
    <parent 1 type>
    <parent 2 type>
    ...
    <parent N type>
    parent end
    */

    void parseResearch(std::string path) {
        std::ifstream input_file;
        input_file.open(path);

        std::string string;
        input_file >> string;
        if (string != "type") {
            if (settings.isErrorOutputEnabled()) {
                std::cout << "Error in "
            }
        }
    }

public:

	bool unlockResearch(ResearchList research_name) {
		if (resource_manager.spendResearch(research[research_name].cost)) {
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

	void initResearch(std::string info_path) {
		research.resize(RESEARCH_COUNT);

		for (int i = 0; i < RESEARCH_COUNT; i++) {
			research[i].type = (ResearchList)i;
            graph[i].type = (ResearchList)i;
		}


	}
} research_manager;