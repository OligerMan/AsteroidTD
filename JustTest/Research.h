#pragma once

#include "ResourceManager.h"
#include "GeometryComponents.h"
#include "ParserInput.h"
#include "ButtonSelector.h"

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
	dome_local_regen_lvl1,
	dome_local_regen_lvl2,
	dome_local_regen_lvl3,
	dome_local_health_lvl1,
	dome_local_health_lvl2,
	dome_local_health_lvl3,
	dome_max_health_lvl1,
	dome_max_health_lvl2,
	dome_max_health_lvl3,
	dome_local_damage_reduction_lvl1,
	dome_local_damage_reduction_lvl2,
	dome_local_damage_reduction_lvl3,
	dome_local_damage_lvl1,
	dome_local_damage_lvl2,
	dome_local_damage_lvl3,
	dome_local_research_income_lvl1,
	dome_local_research_income_lvl2,
	dome_local_research_income_lvl3,
	dome_local_gold_income_lvl1,
	dome_local_gold_income_lvl2,
	dome_local_gold_income_lvl3,
	dome_ultimate,

	science_initial,
	science_income_lvl1,
	science_income_lvl2,
	science_income_lvl3,
	science_max_health_lvl1,
	science_max_health_lvl2,
	science_max_health_lvl3,
	science_regen_lvl1,
	science_regen_lvl2,

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
	//std::string description;
	std::wstring description_id;
};

class ResearchManager {
	std::vector<Research *> research_list;
    std::vector<ResearchNode *> graph;
	Point graph_size;
	Point left_up_graph_corner;

    std::vector<std::wstring> research_name = {
		L"turret_init",
		L"turret_dmg1",    // dmg = damage
		L"turret_dmg2",
		L"turret_dmg3",
		L"turret_asp1",    // asp = attack speed
		L"turret_asp2",
		L"turret_mhp1",    // mhp = max health points
		L"turret_mhp2",
		L"turret_mhp3",
		L"turret_regen1",
		L"turret_regen2",
		L"turret_dmg_red1",     // damage reduction
		L"turret_dmg_red2",
		L"turret_dmg_red3",
		L"turret_dmg_red4",
		L"turret_lfsteal1",
		L"turret_lfsteal2",
		L"turret_lfsteal3",
		L"turret_range1",
		L"turret_range2",
		L"turret_range3",
		L"turret_range4",
		L"turret_ultimate",

		L"dome_init",
		L"dome_g_regen1",
		L"dome_g_regen2",
		L"dome_g_regen3",
		L"dome_g_hp1",
		L"dome_g_hp2",
		L"dome_g_hp3",
		L"dome_mhp1",
		L"dome_mhp2",
		L"dome_mhp3",
		L"dome_g_dmg_red1",  // local damage reduction
		L"dome_g_dmg_red2",
		L"dome_g_dmg_red3",
		L"dome_g_dmg1",
		L"dome_g_dmg2",
		L"dome_g_dmg3",
		L"dome_g_ri1",     // ri = research income
		L"dome_g_ri2",
		L"dome_g_ri3",
		L"dome_g_gi1",     // gi = gold income
		L"dome_g_gi2",
		L"dome_g_gi3",
		L"dome_ultimate",

		L"science_init",
		L"science_inc1",   // inc = income
		L"science_inc2",
		L"science_inc3",
		L"science_mhp1",
		L"science_mhp2",
		L"science_mhp3",
		L"science_regen1",
		L"science_regen2",

		L"gold_init",
		L"gold_inc1",
		L"gold_inc2",
		L"gold_inc3",
		L"gold_mhp1",
		L"gold_mhp2",
		L"gold_mhp3",
		L"gold_regen1",
		L"gold_regen2"
    };

    /*
    Let's define research interpretation in text

	research_start
    type <research_type>
    cost <cost>
    pos <coor x> <coor y>
	description |incredibly giant string with description of that research|   @ as \n
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

	ResearchList stringToResearch(std::wstring string) {
		for (int i = 0; i < research_name.size(); i++) {
			if (research_name[i] == string) {
				return (ResearchList)i;
			}
		}
		return RESEARCH_COUNT;
	}

	std::wstring researchTypeToString(ResearchList type) {
		if (type >= 0 && type < research_name.size()) {
			return research_name[type];
		}
		return L"";
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
		for (int i = 0; i < research_list.size(); i++) {
			delete research_list[i];
		}
		for (int i = 0; i < graph.size(); i++) {
			delete graph[i];
		}
		research_list.clear();
		graph.clear();

        std::wifstream input_file;
        input_file.open(path);

		std::wstring string;

		graph.resize(research_name.size());
		research_list.resize(research_name.size());
			
		while (true) {
			input_file >> string;
			if (string != L"research_start") {
				if (string == L"") {
					input_file.close();
					return;
				}
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Wrong format of research file" << std::endl;
				}
				input_file.close();
				return;
			}
			input_file >> string;
			if (string == L"research_end") {
				break;
			}
			if (string != L"type") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Missing type of research" << std::endl;
				}
				input_file.close();
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
				input_file.close();
				return;
			}
			research->type = type;
			research_node->type = type;

			input_file >> string;
			if (string != L"cost") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Missing cost of research" << std::endl;
				}
				input_file.close();
				return;
			}

			research->cost = getFloat(input_file);
			input_file >> string;
			if (string != L"pos") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Missing position of research button" << std::endl;
				}
				input_file.close();
				return;
			}
			research_node->pos.x = getFloat(input_file);
			research_node->pos.y = getFloat(input_file);

			input_file >> string;
			if (string != L"description") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Missing research's description block" << std::endl;
				}
				input_file.close();
				return;
			}
			/*while (input_file.get() != '\|') {}
			string.clear();
			while (true) {
				unsigned char input_char = input_file.get();
				if (input_char == '\|') {
					break;
				}
				if (input_char == '@') {
					input_char = '\n';
				}
				string.push_back(input_char);
			}
			research_node->description = string;*/
			input_file >> research_node->description_id;

			input_file >> string;
			if (string != L"parent_start") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Missing research's parent block" << std::endl;
				}
				input_file.close();
				return;
			}

			while (true) {
				input_file >> string;
				if (string == L"parent_end") {
					break;
				}
				ResearchList parent_type = stringToResearch(string);
				if (type == RESEARCH_COUNT) {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Wrong type of research's parent" << std::endl;
					}
					input_file.close();
					return;
				}
				research_node->parents.push_back(parent_type);
			}
			input_file >> string;
			if (string != L"research_end") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Research's description end not found" << std::endl;
				}
				break;
			}

			research_list[(int)research->type] = research;
			graph[(int)research->type] = research_node;
		}
		input_file.close();
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

	std::vector<std::wstring> getResearchName() {
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

	float getTurretMaxHealthCoef() {
		float coef = 1;
		if (research_list[turret_initial]->unlocked) {
			coef = 1.1;
		}
		if (research_list[turret_max_health_lvl1]->unlocked) {
			coef = 1.25;
		}
		if (research_list[turret_max_health_lvl2]->unlocked) {
			coef = 1.6;
		}
		if (research_list[turret_max_health_lvl3]->unlocked) {
			coef = 2;
		}
		if (research_list[turret_ultimate]->unlocked) {
			coef = 11;
		}
		return coef;
	}

	float getTurretDamageCoef() {
		float coef = 1;
		if (research_list[turret_initial]->unlocked) {
			coef = 1.1;
		}
		if (research_list[turret_damage_lvl1]->unlocked) {
			coef = 1.25;
		}
		if (research_list[turret_damage_lvl2]->unlocked) {
			coef = 1.6;
		}
		if (research_list[turret_damage_lvl3]->unlocked) {
			coef = 2;
		}
		if (research_list[turret_ultimate]->unlocked) {
			coef = 6;
		}
		return coef;
	}

	float getTurretAttackSpeedCoef() {
		float coef = 1;
		if (research_list[turret_attack_speed_lvl1]->unlocked) {
			coef = 1.5;
		}
		if (research_list[turret_attack_speed_lvl2]->unlocked) {
			coef = 1.75;
		}
		return coef;
	}

	float getTurretRegenCoef() {
		float coef = 1;
		if (research_list[turret_regen_lvl1]->unlocked) {
			coef = 2;
		}
		if (research_list[turret_regen_lvl2]->unlocked) {
			coef = 3;
		}
		if (research_list[turret_ultimate]->unlocked) {
			coef = 11;
		}
		return coef;
	}

	float getTurretDamageReductionCoef() {
		float coef = 0;
		if (research_list[turret_damage_reduction_lvl1]->unlocked) {
			coef = 0.1;
		}
		if (research_list[turret_damage_reduction_lvl2]->unlocked) {
			coef = 0.15;
		}
		if (research_list[turret_damage_reduction_lvl3]->unlocked) {
			coef = 0.2;
		}
		if (research_list[turret_damage_reduction_lvl4]->unlocked) {
			coef = 0.25;
		}
		if (research_list[turret_ultimate]->unlocked) {
			coef = 0.5;
		}
		return coef;
	}

	float getTurretAttackRangeCoef() {
		float coef = 1;
		if (research_list[turret_range_lvl1]->unlocked) {
			coef = 1.1;
		}
		if (research_list[turret_range_lvl2]->unlocked) {
			coef = 1.15;
		}
		if (research_list[turret_range_lvl3]->unlocked) {
			coef = 1.2;
		}
		if (research_list[turret_range_lvl4]->unlocked) {
			coef = 1.25;
		}
		if (research_list[turret_ultimate]->unlocked) {
			coef = 1.5;
		}
		return coef;
	}

	float getTurretLifestealCoef() {
		float coef = 0;
		if (research_list[turret_lifesteal_lvl1]->unlocked) {
			coef = 0.05;
		}
		if (research_list[turret_lifesteal_lvl2]->unlocked) {
			coef = 0.1;
		}
		if (research_list[turret_lifesteal_lvl3]->unlocked) {
			coef = 0.15;
		}
		if (research_list[turret_ultimate]->unlocked) {
			coef = 1;
		}
		return coef;
	}

	// dome researches block

	float getDomeLocalRegenCoef() {
		float coef = 0;
		if (research_list[dome_initial]->unlocked) {
			coef = 0.1;
		}
		if (research_list[dome_local_regen_lvl1]->unlocked) {
			coef = 0.25;
		}
		if (research_list[dome_local_regen_lvl1]->unlocked) {
			coef = 0.6;
		}
		if (research_list[dome_local_regen_lvl1]->unlocked) {
			coef = 1;
		}
		if (research_list[dome_ultimate]->unlocked) {
			coef = 6;
		}
		return coef;
	}

	float getDomeMaxHealthCoef() {
		float coef = 1;
		if (research_list[dome_initial]->unlocked) {
			coef = 1.1;
		}
		if (research_list[dome_max_health_lvl1]->unlocked) {
			coef = 1.25;
		}
		if (research_list[dome_max_health_lvl2]->unlocked) {
			coef = 1.6;
		}
		if (research_list[dome_max_health_lvl3]->unlocked) {
			coef = 2;
		}
		if (research_list[dome_ultimate]->unlocked) {
			coef = 6;
		}
		return coef;
	}

	float getDomeLocalMaxHealthCoef() {
		float coef = 0;
		if (research_list[dome_initial]->unlocked) {
			coef = 0.1;
		}
		if (research_list[dome_local_health_lvl1]->unlocked) {
			coef = 0.25;
		}
		if (research_list[dome_local_health_lvl2]->unlocked) {
			coef = 0.65;
		}
		if (research_list[dome_local_health_lvl3]->unlocked) {
			coef = 1.5;
		}
		if (research_list[dome_ultimate]->unlocked) {
			coef = 4;
		}
		return coef;
	}

	float getDomeLocalDamageReductionCoef() {
		float coef = 0;
		if (research_list[dome_local_damage_reduction_lvl1]->unlocked) {
			coef = 0.02;
		}
		if (research_list[dome_local_damage_reduction_lvl2]->unlocked) {
			coef = 0.03;
		}
		if (research_list[dome_local_damage_reduction_lvl3]->unlocked) {
			coef = 0.04;
		}
		if (research_list[dome_ultimate]->unlocked) {
			coef = 0.05;
		}
		return coef;
	}

	float getDomeLocalDamageBonusCoef() {
		float coef = 0;
		if (research_list[dome_local_damage_lvl1]->unlocked) {
			coef = 0.05;
		}
		if (research_list[dome_local_damage_lvl1]->unlocked) {
			coef = 0.1;
		}
		if (research_list[dome_local_damage_lvl1]->unlocked) {
			coef = 0.15;
		}
		if (research_list[dome_ultimate]->unlocked) {
			coef = 0.5;
		}
		return coef;
	}

	float getDomeLocalGoldIncomeCoef() {
		float coef = 0;
		if (research_list[dome_local_gold_income_lvl1]->unlocked) {
			coef = 0.05;
		}
		if (research_list[dome_local_gold_income_lvl1]->unlocked) {
			coef = 0.1;
		}
		if (research_list[dome_local_gold_income_lvl1]->unlocked) {
			coef = 0.15;
		}
		if (research_list[dome_ultimate]->unlocked) {
			coef = 0.5;
		}
		return coef;
	}

	float getDomeLocalResearchIncomeCoef() {
		float coef = 0;
		if (research_list[dome_local_research_income_lvl1]->unlocked) {
			coef = 1.05;
		}
		if (research_list[dome_local_research_income_lvl1]->unlocked) {
			coef = 1.1;
		}
		if (research_list[dome_local_research_income_lvl1]->unlocked) {
			coef = 1.15;
		}
		if (research_list[dome_ultimate]->unlocked) {
			coef = 1.5;
		}
		return coef;
	}

	// science base researches block

	float getScienceIncomeCoef() {
		float coef = 1;
		if (research_list[science_initial]->unlocked) {
			coef = 1.1;
		}
		if (research_list[science_income_lvl1]->unlocked) {
			coef = 1.25;
		}
		if (research_list[science_income_lvl2]->unlocked) {
			coef = 1.6;
		}
		if (research_list[science_income_lvl3]->unlocked) {
			coef = 2;
		}
		return coef;
	}

	float getScienceMaxHealthCoef() {
		float coef = 1;
		if (research_list[science_initial]->unlocked) {
			coef = 1.1;
		}
		if (research_list[science_max_health_lvl1]->unlocked) {
			coef = 1.25;
		}
		if (research_list[science_max_health_lvl1]->unlocked) {
			coef = 1.6;
		}
		if (research_list[science_max_health_lvl1]->unlocked) {
			coef = 2;
		}
		return coef;
	}

	float getScienceRegenCoef() {
		float coef = 1;
		if (research_list[science_regen_lvl1]->unlocked) {
			coef = 2;
		}
		if (research_list[science_regen_lvl1]->unlocked) {
			coef = 3;
		}
		return coef;
	}

	// gold base researches block

	float getGoldIncomeCoef() {
		float coef = 1;
		if (research_list[gold_initial]->unlocked) {
			coef = 1.1;
		}
		if (research_list[gold_income_lvl1]->unlocked) {
			coef = 1.25;
		}
		if (research_list[gold_income_lvl2]->unlocked) {
			coef = 1.6;
		}
		if (research_list[gold_income_lvl3]->unlocked) {
			coef = 2;
		}
		return coef;
	}

	float getGoldMaxHealthCoef() {
		float coef = 1;
		if (research_list[gold_initial]->unlocked) {
			coef = 1.1;
		}
		if (research_list[gold_max_health_lvl1]->unlocked) {
			coef = 1.25;
		}
		if (research_list[gold_max_health_lvl1]->unlocked) {
			coef = 1.6;
		}
		if (research_list[gold_max_health_lvl1]->unlocked) {
			coef = 2;
		}
		return coef;
	}

	float getGoldRegenCoef() {
		float coef = 1;
		if (research_list[gold_regen_lvl1]->unlocked) {
			coef = 2;
		}
		if (research_list[gold_regen_lvl1]->unlocked) {
			coef = 3;
		}
		return coef;
	}

} research_manager;