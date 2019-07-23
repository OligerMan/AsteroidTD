#pragma once
#include <algorithm>
#include <string>
#include <fstream>
#include <algorithm>

enum FactionList {
	null_faction,
	hero_faction,
	friendly_faction,
	neutral_faction,
	aggressive_faction,

	FACTION_COUNT
};

std::vector<std::string> faction_type;


void faction_type_init() {
	faction_type.resize(FACTION_COUNT);

	faction_type[null_faction] = "null";

	faction_type[hero_faction] = "hero";
	faction_type[friendly_faction] = "friendly";
	faction_type[neutral_faction] = "neutral";
	faction_type[aggressive_faction] = "aggressive";
}

int is_faction_type_exists(std::string example) {
	for (int i = 0; i < FACTION_COUNT; i++) {
		if (faction_type[i] == example) {
			return i;
		}
	}
	return -1;
}

bool areEnemies(FactionList fact1, FactionList fact2) {
	if (fact1 > fact2) {
		FactionList tmp = fact1;
		fact1 = fact2;
		fact2 = tmp;
	}

	if (fact1 == FactionList::aggressive_faction) {
		return 0;
	}

	if (fact1 == FactionList::null_faction) {
		return 0;
	}

	if (fact2 == FactionList::aggressive_faction) {
		return 1;
	}

	//otherwise
	return 0;
}

class UnitInfo {
	float EPS = 0.00001;

	float max_hp = 0;
	float hp = 0;        // health points
	bool inf_hp = true;
	float max_mana = 0;
	float mana = 0;      // mana points
	bool inf_mana = true;
	float max_endur = 0;
	float endur = 0;     // endurance
	bool inf_endur = true;

	float attack1_range = 0;
	float attack2_range = 0;
	float attack3_range = 0;

	float attack1_damage = 0;
	float attack2_damage = 0;
	float attack3_damage = 0;

	float attack1_delay = 0;
	float attack2_delay = 0;
	float attack3_delay = 0;

	float attack1_cooldown = 0;
	float attack2_cooldown = 0;
	float attack3_cooldown = 0;

	float attack1_cur_cooldown = 0;
	float attack2_cur_cooldown = 0;
	float attack3_cur_cooldown = 0;

	float default_speed = 0;
	float speed_coef = 1;
	float turn_speed = 2;

	// AI info
	float anger_range = 550;
	int faction = 0;              // faction index: 0 for non-unit objects, 1 for hero and his allies, from 2 to infinity(2^31) for other factions
	void * enemy_object_ptr = nullptr;

	bool is_effects_allowed = true;

	bool unit_info_correct = true;


	void parseUnitInfo(std::string path, bool * status) {
		std::ifstream unit_input;
		unit_input.open(path);

		*status = true;

		if (!unit_input.is_open()) {
			if (settings.isErrorOutputEnabled()) {
				std::cout << "Unit info file " << path << " not found" << std::endl;
			}
			*status = false;
			return;
		}

		if (settings.isUnitInfoDebugEnabled()) {
			std::cout << "Unit file from " << path << " configuration" << std::endl;
		}

		std::string setting, value;
		while (true) {
			if (settings.isUnitInfoDebugEnabled()) {
				std::cout << "    ";
			}
			unit_input >> setting;
			unit_input >> value;
			if (setting == "") {
				unit_info_correct = false;
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Unit info file " << path << " error" << std::endl;
				}
				break;
			}
			if (setting == "max_hp") {
				max_hp = std::stoi(value);
				hp = max_hp;
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Max health for " << path << " stated to " << hp << std::endl;
				}
			}
			if (setting == "inf_hp") {
				inf_hp = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Infinite health is set to " << inf_hp << std::endl;
				}
			}
			if (setting == "max_mana") {
				max_mana = std::stoi(value);
				mana = max_mana;
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Max mana for " << path << " stated to " << mana << std::endl;
				}
			}
			if (setting == "inf_mana") {
				inf_mana = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Infinite mana is set to " << inf_mana << std::endl;
				}
			}
			if (setting == "max_endur") {
				max_endur = std::stoi(value);
				endur = max_endur;
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Max endurance for " << path << " stated to " << endur << std::endl;
				}
			}
			if (setting == "inf_endur") {
				inf_endur = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Infinite endurance is set to " << inf_endur << std::endl;
				}
			}
			if (setting == "attack1_range") {
				attack1_range = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack range 1 is set to " << attack1_range << std::endl;
				}
			}
			if (setting == "attack2_range") {
				attack2_range = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack range 2 is set to " << attack2_range << std::endl;
				}
			}
			if (setting == "attack3_range") {
				attack3_range = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack range 3 is set to " << attack3_range << std::endl;
				}
			}
			if (setting == "attack1_damage") {
				attack1_damage = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack damage 1 is set to " << attack1_damage << std::endl;
				}
			}
			if (setting == "attack2_damage") {
				attack2_damage = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack damage 2 is set to " << attack2_damage << std::endl;
				}
			}
			if (setting == "attack3_damage") {
				attack3_damage = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack damage 3 is set to " << attack3_damage << std::endl;
				}
			}
			if (setting == "attack1_delay") {
				attack1_delay = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack delay 1 is set to " << attack1_damage << std::endl;
				}
			}
			if (setting == "attack2_delay") {
				attack2_delay = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack delay 2 is set to " << attack2_damage << std::endl;
				}
			}
			if (setting == "attack3_delay") {
				attack3_delay = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack delay 3 is set to " << attack3_damage << std::endl;
				}
			}
			if (setting == "attack1_cooldown") {
				attack1_cooldown = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack cooldown 1 is set to " << attack1_damage << std::endl;
				}
			}
			if (setting == "attack2_cooldown") {
				attack2_delay = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack cooldown 2 is set to " << attack2_damage << std::endl;
				}
			}
			if (setting == "attack3_cooldown") {
				attack3_cooldown = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack cooldown 3 is set to " << attack3_damage << std::endl;
				}
			}
			if (setting == "default_speed") {
				default_speed = std::stoi(value); 
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Default speed is set to " << default_speed << std::endl;
				}
			}
			if (setting == "speed_coef") {
				speed_coef = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Speed coefficient is set to " << speed_coef << std::endl;
				}
			}
			if (setting == "turn_speed") {
				turn_speed = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Turn speed coefficient is set to " << turn_speed << std::endl;
				}
			}
			if (setting == "is_effects_allowed") {
				is_effects_allowed = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Effects vulnerability is set to " << is_effects_allowed << std::endl;
				}
			}
			if (setting == "faction") {
				faction = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Faction is set to " << faction << std::endl;
				}
			}
			if (setting == "anger_range") {
				anger_range = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Anger range is set to " << anger_range << std::endl;
				}
			}
			if (setting == "unit_info_end") {
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Unit info config end" << std::endl;
				}
				break;
			}
		}
	}

public:

	UnitInfo() {}

	UnitInfo(std::string path, bool * status) {
		parseUnitInfo(path, status);
	}

	void setEnemy(void * ptr) {
		enemy_object_ptr = ptr;
	}

	void * getEnemy() {
		return enemy_object_ptr;
	}

	void setHealth(float new_hp) {
		hp = new_hp;
	}

	void setMana(float new_mana) {
		mana = new_mana;
	}

	void setEndurance(float new_endurance) {
		endur = new_endurance;
	}

	bool dealDamage(float damage) {
		if (!inf_hp && damage > 0) {
			hp = std::max(-EPS, hp - damage);
		}
		return (hp > 0);
	}

	void grantHeal(float heal) {
		if (!inf_hp && heal > 0) {
			hp = std::min(max_hp, hp + heal);
		}
	}

	bool dealManaBurn(float mana_burn) {
		if (!inf_mana && mana_burn > 0) {
			mana = std::max(EPS, mana - mana_burn);
		}
		return (mana > 0);
	}

	void grantManaRestore(float mana_restore) {
		if (!inf_mana && mana_restore > 0) {
			mana = std::min(max_mana, mana + mana_restore);
		}
	}

	bool dealEndurBurn(float endur_burn) {
		if (!inf_endur && endur_burn > 0) {
			endur = std::max(EPS, endur - endur_burn);
		}
		return (endur > 0);
	}

	void grantEndurRestore(float endur_restore) {
		if (!inf_endur && endur_restore > 0) {
			endur = std::min(max_endur, endur + endur_restore);
		}
	}

	float getHealth() {
		return hp;
	}

	float getMaxHealth() {
		return max_hp;
	}

	float getMana() {
		return mana;
	}

	float getMaxMana() {
		return max_mana;
	}

	float getEndurance() {
		return endur;
	}

	float getMaxEndurance() {
		return max_endur;
	}

	bool isDead() {
		return (!inf_hp && (hp < 0));
	}

	int getFaction() {
		return faction;
	}

	void setFaction(int new_faction) {
		faction = new_faction;
	}

	float getAngerRange() {
		return anger_range;
	}

	void setAngerRange(float new_anger_range) {
		anger_range = new_anger_range;
	}

	float getAttackRange1() {
		return attack1_range;
	}

	float getAttackRange2() {
		return attack2_range;
	}

	float getAttackRange3() {
		return attack3_range;
	}

	float getAttackDamage1() {
		return attack1_damage;
	}

	float getAttackDamage2() {
		return attack2_damage;
	}

	float getAttackDamage3() {
		return attack3_damage;
	}

	float getAttackDelay1() {
		return attack1_delay;
	}

	float getAttackDelay2() {
		return attack2_delay;
	}

	float getAttackDelay3() {
		return attack3_delay;
	}

	float getAttackCooldown1() {
		return attack1_cooldown;
	}

	float getAttackCooldown2() {
		return attack2_cooldown;
	}

	float getAttackCooldown3() {
		return attack3_cooldown;
	}

	bool attack1Ready() {
		bool ans = attack1_cur_cooldown < 0.1;
		if (ans) {
			attack1_cur_cooldown = attack1_cooldown;
		}
		return ans;
	}

	bool attack2Ready() {
		bool ans = attack1_cur_cooldown < 0.1;
		if (ans) {
			attack2_cur_cooldown = attack2_cooldown;
		}
		return ans;
	}

	bool attack3Ready() {
		bool ans = attack1_cur_cooldown < 0.1;
		if (ans) {
			attack3_cur_cooldown = attack3_cooldown;
		}
		return ans;
	}

	void processCooldown() {
		attack1_cur_cooldown = std::max(0.0f, attack1_cur_cooldown - 1);
		attack2_cur_cooldown = std::max(0.0f, attack2_cur_cooldown - 1);
		attack3_cur_cooldown = std::max(0.0f, attack3_cur_cooldown - 1);
	}

	float getDefaultSpeed() {
		return default_speed;
	}
};