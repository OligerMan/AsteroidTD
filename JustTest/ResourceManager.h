#pragma once

class ResourceManager {
	float gold = 0;
	float research = 0;

	float gold_income = 0;
	float research_income = 0;

public:

	ResourceManager(float start_gold, float start_research) {
		gold = start_gold;
		research = start_research;
	}

	bool spendGold(float price) {
		if ((gold - price) < 0) {
			return false;
		}
		else {
			gold -= price;
			return true;
		}
	}

	void addGold(float additional_gold) {
		gold += additional_gold;
	}

	bool spendResearch(float price) {
		if ((research - price) < 0) {
			return false;
		}
		else {
			research -= price;
			return true;
		}
	}

	void addResearch(float additional_research) {
		research += additional_research;
	}

	void changeGoldIncome(float income_diff) {
		gold_income += income_diff;
	}

	void changeResearchIncome(float income_diff) {
		research_income = income_diff;
	}

	void processFrame() {
		gold += gold_income;
		research += research_income;
	}

	float getGold() {
		return gold;
	}

	float getResearch() {
		return research;
	}
};