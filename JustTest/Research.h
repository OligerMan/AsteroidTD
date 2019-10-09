#pragma once

#include "ResourceManager.h"
#include "GeometryComponents.h"
#include "ParserInput.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

enum ResearchList {
	turret_initial,
	turret_damage_lvl1,
	turret_damage_lvl2,
	turret_damage_lvl3,
	turret_attack_speed_lvl1,
	turret_attack_speed_lvl2,
	turret_max_health_lvl1,
	turret_max_health_lvl2,
	turret_max_health_lvl3,
	turret_regen_lvl1,
	turret_regen_lvl2,
	turret_damage_reduction_lvl1,
	turret_damage_reduction_lvl2,
	turret_damage_reduction_lvl3,
	turret_damage_reduction_lvl4,
	turret_lifesteal_lvl1,
	turret_lifesteal_lvl2,
	turret_lifesteal_lvl3,
	turret_range_lvl1,
	turret_range_lvl2,
	turret_range_lvl3,
	turret_range_lvl4,
	turret_ultimate,

	dome_initial,
	dome_global_regen_lvl1,
	dome_global_regen_lvl2,
	dome_global_regen_lvl3,
	dome_global_health_lvl1,
	dome_global_health_lvl2,
	dome_global_health_lvl3,
	dome_max_health_lvl1,
	dome_max_health_lvl2,
	dome_max_health_lvl3,
	dome_global_damage_reduction_lvl1,
	dome_global_damage_reduction_lvl2,
	dome_global_damage_reduction_lvl3,
	dome_global_damage_lvl1,
	dome_global_damage_lvl2,
	dome_global_damage_lvl3,
	dome_global_research_income_lvl1,
	dome_global_research_income_lvl2,
	dome_global_research_income_lvl3,
	dome_global_gold_income_lvl1,
	dome_global_gold_income_lvl2,
	dome_global_gold_income_lvl3,
	dome_ultimate,

	research_initial,
	research_income_lvl1,
	research_income_lvl2,
	research_income_lvl3,
	research_max_health_lvl1,
	research_max_health_lvl2,
	research_max_health_lvl3,
	research_regen_lvl1,
	research_regen_lvl2,

	gold_initial,
	gold_income_lvl1,
	gold_income_lvl2,
	gold_income_lvl3,
	gold_max_health_lvl1,
	gold_max_health_lvl2,
	gold_max_health_lvl3,
	gold_regen_lvl1,
	gold_regen_lvl2,

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
	Point graph_size;
	Point left_up_graph_corner;

    std::vector<std::string> research_name = {
		"turret_init",
		"turret_dmg1",    // dmg = damage
		"turret_dmg2",
		"turret_dmg3",
		"turret_asp1",    // asp = attack speed
		"turret_asp2",
		"turret_mhp1",    // mhp = max health points
		"turret_mhp2",
		"turret_mhp3",
		"turret_regen1",
		"turret_regen2",
		"turret_dmg_red1",     // damage reduction
		"turret_dmg_red2",
		"turret_dmg_red3",
		"turret_dmg_red4",
		"turret_lfsteal1",
		"turret_lfsteal2",
		"turret_lfsteal3",
		"turret_range1",
		"turret_range2",
		"turret_range3",
		"turret_range4",
		"turret_ultimate",

		"dome_init",
		"dome_g_regen1",
		"dome_g_regen2",
		"dome_g_regen3",
		"dome_g_hp1",
		"dome_g_hp2",
		"dome_g_hp3",
		"dome_mhp1",
		"dome_mhp2",
		"dome_mhp3",
		"dome_g_dmg_red1",  // global damage reduction
		"dome_g_dmg_red2",
		"dome_g_dmg_red3",
		"dome_g_dmg1",
		"dome_g_dmg2",
		"dome_g_dmg3",
		"dome_g_ri1",     // ri = research income
		"dome_g_ri2",
		"dome_g_ri3",
		"dome_g_gi1",     // gi = gold income
		"dome_g_gi2",
		"dome_g_gi3",
		"dome_ultimate",

		"research_init",
		"research_inc1",   // inc = income
		"research_inc2",
		"research_inc3",
		"research_mhp1",
		"research_mhp2",
		"research_mhp3",
		"research_regen1",
		"research_regen2",

		"gold_init",
		"gold_inc1",
		"gold_inc2",
		"gold_inc3",
		"gold_mhp1",
		"gold_mhp2",
		"gold_mhp3",
		"gold_regen1",
		"gold_regen2"
    };

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

	bool isParserKeyword(std::string str) {
		return (str == "research_start" || str == "research_end" || str == "type" || str == "cost" || str == "pos" || str == "parent_start" || str == "parent_end");
	}
public: 

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

	void setGraphSize() {
		float left = graph[0]->pos.x, right = graph[0]->pos.x, up = graph[0]->pos.y, down = graph[0]->pos.y;
		for (int i = 0; i < graph.size(); i++) {
			left = std::min(left, (float)graph[i]->pos.x);
			right = std::max(right, (float)graph[i]->pos.x);
			down = std::min(down, (float)graph[i]->pos.y);
			up = std::max(up, (float)graph[i]->pos.y);
		}
		graph_size = Point(right - left, up - down);
		left_up_graph_corner = Point(left, up);
	}

    void initResearch(std::string path) {
        std::ifstream input_file;
        input_file.open(path);

		std::string string;

		/*input_file >> string;
		if (string != "list_start") {
			if (settings.isErrorOutputEnabled()) {
				std::cout << "Wrong format of research file" << std::endl;
			}
		}

		while (true) {
			input_file >> string;
			if (string == "list_end") {
				break;
			}
			if (isParserKeyword(string)) {

			}
		}*/

		graph.resize(research_name.size());
		research_list.resize(research_name.size());
			
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
			input_file >> string;
			if (string == "research_end") {
				break;
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
				break;
			}

			research_list[(int)research->type] = research;
			graph[(int)research->type] = research_node;
		}
    }

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

	ResearchNode * getResearchNode(int research_name) {
		return graph[research_name];
	}

	bool isResearchActive(int num) {
		for (int i = 0; i < graph[num]->parents.size(); i++) {
			if (!research_list[graph[num]->parents[i]]->unlocked) {
				return false;
			}
		}
		return resource_manager.isEnoughResearch(research_list[num]->cost);
	}

	bool isResearchUnlocked(int num) {
		return research_list[num]->unlocked;
	}

	std::vector<Research *>  getResearchArray() {
		return research_list;
	}

	std::vector<ResearchNode *>  getResearchGraph() {
		return graph;
	}

	std::vector<std::string> getResearchName() {
		return research_name;
	}

	int getResearchAmount() {
		return research_name.size();
	}

	Point getGraphSize() {
		return graph_size;
	}

	Point getLeftUpGraphCorner() {
		return left_up_graph_corner;
	}

// start of in-game coefficients getters

	/// turret researches block

	float getTurretMaxHealthCCoef() {
		float coef = 1;
		return coef;
	}

	float getTurretDamageCoef() {
		float coef = 1;
		return coef;
	}

	float getTurretAttackSpeedCoef() {
		float coef = 1;
		return coef;
	}

	float getTurretRegenerationCoef() {
		float coef = 1;
		return coef;
	}

	float getTurretDamageReductionCoef() {
		float coef = 0;
		return coef;
	}

	float getTurretAttackRangeCoef() {
		float coef = 1;
		return coef;
	}

	float getTurretLifestealCoef() {
		float coef = 0;
		return coef;
	}

	// dome researches block

	float getDomeGlobalRegenCoef() {
		float coef = 0;
		return coef;
	}

	float getDomeMaxHealthCoef() {
		float coef = 1;
		return coef;
	}

	float getDomeGlobalMaxHealthCoef() {
		float coef = 0;
		return coef;
	}

	float getDomeGlobalDamageReductionCoef() {
		float coef = 0;
		return coef;
	}

	float getDomeGlobalDamageBonusCoef() {
		float coef = 0;
		return coef;
	}

	float getDomeGlobalGoldIncomeCoef() {
		float coef = 0;
		return coef;
	}

	float getDomeGlobalResearchIncomeCoef() {
		float coef = 0;
		return coef;
	}

	// science base researches block

	float getScienceIncomeCoef() {
		float coef = 1;
		return coef;
	}

	float getScienceMaxHealthCoef() {
		float coef = 1;
		return coef;
	}

	float getScienceRegenCoef() {
		float coef = 1;
		return coef;
	}

	// gold base researches block

	float getGoldIncomeCoef() {
		float coef = 1;
		return coef;
	}

	float getGoldMaxHealthCoef() {
		float coef = 1;
		return coef;
	}

	float getGoldRegenCoef() {
		float coef = 1;
		return coef;
	}

} research_manager;