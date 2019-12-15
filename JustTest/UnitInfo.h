#pragma once
#include <algorithm>
#include <string>
#include <fstream>
#include <algorithm>

#include "Effect.h"
#include "GameConstants.h"
#include "FPS.h"
#include "Research.h"

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

struct AttackInfo {
	float
		range = 0,
		damage = 0,
		delay = 0,
		cooldown = 0,
		cur_cooldown = 0;
};

class UnitInfo {

	float EPS = 0.00001;

	std::vector<Effect> effects;

	float max_hp = 0;
	float hp = 0;        // health points
	bool inf_hp = true;
	float max_mana = 0;
	float mana = 0;      // mana points
	bool inf_mana = true;
	float max_endur = 0;
	float endur = 0;     // endurance
	bool inf_endur = true;

	int dome_cnt = 0;   // for applying researches

	std::vector<AttackInfo> attack;

	float default_speed = 0;
	float speed_coef = 1;
	float turn_speed = 2;

	// AI info
	float anger_range = 600;
	float min_flight_range = 450;
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
			unit_input.close();
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
			if (setting == "min_flight_range") {
				min_flight_range = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Minimal flight range is set to " << min_flight_range << std::endl;
				}
			}
			if (setting.substr(0,6) == "attack") {
				float f_value = std::stof(value);
				int n = setting.find("_");
				int num = std::stoi(setting.substr(6, n - 6));
				if (attack.size() <= num) {
					attack.resize(num + 1);
				}
				if (setting.substr(n + 1) == "range") {
					attack[num].range = f_value;
					if (settings.isUnitInfoDebugEnabled()) {
						std::cout << "Attack range 1 is set to " << attack[num].range << std::endl;
					}
				}
				if (setting.substr(n + 1) == "damage") {
					attack[num].damage = f_value;
					if (settings.isUnitInfoDebugEnabled()) {
						std::cout << "Attack damage 1 is set to " << attack[num].damage << std::endl;
					}
				}
				if (setting.substr(n + 1) == "delay") {
					attack[num].delay = f_value;
					if (settings.isUnitInfoDebugEnabled()) {
						std::cout << "Attack delay 1 is set to " << attack[num].delay << std::endl;
					}
				}
				if (setting.substr(n + 1) == "cooldown") {
					attack[num].cooldown = f_value;
					if (settings.isUnitInfoDebugEnabled()) {
						std::cout << "Attack cooldown 1 is set to " << attack[num].cooldown << std::endl;
					}
				}
			}
			
			if (setting == "default_speed") {
				default_speed = std::stof(value); 
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
		unit_input.close();
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

	void setMaxHealth(float new_max_hp) {
		hp = hp * new_max_hp / max_hp;
		max_hp = new_max_hp;
	}

	void setMana(float new_mana) {
		mana = new_mana;
	}

	void setEndurance(float new_endurance) {
		endur = new_endurance;
	}

	bool dealDamage(float damage) {
		if (!inf_hp && damage > 0) {
			hp = std::max(-EPS, hp - damage * (1 - research_manager.getTurretDamageReductionCoef() - research_manager.getDomeLocalDamageReductionCoef() * dome_cnt));
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

	size_t getAttackInfoSize() {
		return attack.size();
	}

	float getAttackRange(int num) {
		if (attack.size() <= num) {
			return 0.0;
		}
		return attack[num].range;
	}

	float getAttackDamage(int num) {
		if (attack.size() <= num) {
			return 0.0;
		}
		return attack[num].damage;
	}

	float getAttackDelay(int num) {
		if (attack.size() <= num) {
			return 0.0;
		}
		return attack[num].delay;
	}

	float getAttackCooldown(int num) {
		if (attack.size() <= num) {
			return 0.0;
		}
		return attack[num].cooldown;
	}

	bool attackReady(int num) {
		bool ans = attack[num].cur_cooldown < 0.1;
		if (ans) {
			attack[num].cur_cooldown = attack[num].cooldown;
		}
		return ans;
	}

	void processCooldown() {
		for (int i = 0; i < attack.size(); i++) {
			attack[i].cur_cooldown = std::max(0.0f, attack[i].cur_cooldown - consts.getFPSLock() / fps.getFPS());
			if (isAffected(EffectList::attack_speed_buff)) {
				attack[i].cur_cooldown = std::max(0.0f, attack[i].cur_cooldown - consts.getFPSLock() / fps.getFPS());
			}
		}
	}

	void processEffects() {
		for (int i = 0; i < effects.size(); i++) {
			effects[i].time -= consts.getFPSLock() / fps.getFPS();
			if (effects[i].time < 0) {
				effects.erase(effects.begin() + i);
				continue;
			}
			if (effects[i].effect == fire_debuff) {
				dealDamage(consts.getFireDamage());
			}
			if (effects[i].effect == regen_buff) {
				grantHeal(consts.getRegenBuff());
			}
			if (effects[i].effect == const_heal) {
				grantHeal(consts.getConstHeal());
			}
		}
	}

	float getDefaultSpeed() {
		return default_speed;
	}

	bool canObjectAttack() {
		for (int i = 0; i < attack.size(); i++) {
			if (attack[i].range > 0.000001) {
				return true;
			}
		}
		return false;
	}

	bool isAffected(EffectList effect) {
		for (int i = 0; i < effects.size(); i++) {
			if (effects[i].effect == effect) {
				return true;
			}
		}
		return false;
	}

	void setEffect(Effect effect) {
		effects.push_back(effect);
	}

	void researchApply(ObjectType type, int dome_count) {
		dome_cnt = dome_count;
		switch (type) {
		case turret:
			max_hp *= (research_manager.getDomeLocalMaxHealthCoef() * dome_count + research_manager.getTurretMaxHealthCoef());
			hp *= (research_manager.getDomeLocalMaxHealthCoef() * dome_count + research_manager.getTurretMaxHealthCoef());
			for (int i = 0; i < attack.size(); i++) {
				attack[i].damage *= (research_manager.getTurretDamageCoef() + research_manager.getDomeLocalDamageBonusCoef() * dome_count);
				attack[i].cooldown /= research_manager.getTurretAttackSpeedCoef();
			}
			break;
		case dome:
			max_hp *= (research_manager.getDomeLocalMaxHealthCoef() * dome_count + research_manager.getDomeMaxHealthCoef());
			hp *= (research_manager.getDomeLocalMaxHealthCoef() * dome_count + research_manager.getDomeMaxHealthCoef());
			break;
		case science:
			max_hp *= (research_manager.getDomeLocalMaxHealthCoef() * dome_count + research_manager.getScienceMaxHealthCoef());
			hp *= (research_manager.getDomeLocalMaxHealthCoef() * dome_count + research_manager.getScienceMaxHealthCoef());
			break;
		case gold:
			max_hp *= (research_manager.getDomeLocalMaxHealthCoef() * dome_count + research_manager.getGoldMaxHealthCoef());
			hp *= (research_manager.getDomeLocalMaxHealthCoef() * dome_count + research_manager.getGoldMaxHealthCoef());
			break;
		}
	}

	int getDomeCount() {
		return dome_cnt;
	}

	float getMinimalFlightRange() {
		return min_flight_range;
	}
};

std::vector<UnitInfo> unit_info_set;

void unit_info_init() {
	bool status;
	for (int i = 0; i < object_type.size(); i++) {
		unit_info_set.push_back(UnitInfo("unit_info/" + object_type[i] + ".unit", &status));
		if (!status) {
			unit_info_set[unit_info_set.size() - 1] = UnitInfo();
		}
	}
}