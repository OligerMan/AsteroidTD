#pragma once

#include "ResourceManager.h"
#include "GeometryComponents.h"
#include "ParserInput.h"

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
	std::vector<Research *> research_list;
    std::vector<ResearchNode *> graph;

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

	int getInt(std::ifstream input_file) {

	}

    /*
    Let's define research interpretation in text
	research_start
    type <research_type>
    cost <cost>
    pos <coor x> <coor y>
    parent_start
    <parent 1 type>
    <parent 2 type>
    ...
    <parent N type>
    parent_end

	<again up to the last research>

	research_end
    */

    void parseResearch(std::string path) {
        std::ifstream input_file;
        input_file.open(path);

		std::string string;

		while (true) {
			input_file >> string;
			if (string != "research_start") {
				if (string == "") {
					return;
				}
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Wrong format of research file" << std::endl;
				}
				return;
			}
			while (true) {
				input_file >> string;
				if (string == "research_end") {
					return;
				}
				if (string != "type") {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Missing type of research" << std::endl;
					}
					return;
				}
				Research * research = new Research;
				ResearchNode * research_node = new ResearchNode;
				float data_f;
				int data_i;
				input_file >> string;
				ResearchList type = stringToResearch(string);
				if (type == RESEARCH_COUNT) {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Wrong type of research" << std::endl;
					}
					return;
				}
				research->type = type;
				research_node->type = type;

				input_file >> string;
				if (string != "cost") {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Missing cost of research" << std::endl;
					}
					return;
				}

				research->cost = getFloat(input_file);
				input_file >> string;
				if (string != "pos") {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Missing position of research button" << std::endl;
					}
					return;
				}
				research_node->pos.x = getFloat(input_file);
				research_node->pos.y = getFloat(input_file);

				input_file >> string;
				if (string != "parent_start") {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Missing research's parent block" << std::endl;
					}
					return;
				}

				while (true) {
					input_file >> string;
					if (string == "parent_end") {
						break;
					}
					ResearchList parent_type = stringToResearch(string);
					if (type == RESEARCH_COUNT) {
						if (settings.isErrorOutputEnabled()) {
							std::cout << "Wrong type of research's parent" << std::endl;
						}
						return;
					}
					research_node->parents.push_back(parent_type);
				}
				input_file >> string;
				if (string != "research_end") {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Research's description end not found" << std::endl;
					}
					return;
				}
				research_list.push_back(research);
				graph.push_back(research_node);
			}
		}
    }

public:

	bool unlockResearch(ResearchList research_name) {
		if (resource_manager.spendResearch(research_list[research_name]->cost)) {
			research_list[research_name]->unlocked = true;
			return true;
		}
		else {
			return false;
		}
	}

	Research * getResearch(ResearchList research_name) {
		return research_list[(int)research_name];
	}

	std::vector<Research *> * getResearchArray() {
		return &research_list;
	}

	void initResearch(std::string info_path) {
		research_list.resize(RESEARCH_COUNT);

		for (int i = 0; i < RESEARCH_COUNT; i++) {
			research_list[i]->type = (ResearchList)i;
            graph[i]->type = (ResearchList)i;
		}


	}
} research_manager;