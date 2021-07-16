#pragma once

#include "../../ResourcePath.hpp"

class Constants {

    const char * default_font = (resourcePath() + "18888.ttf").c_str();

	const double speed_damage_threshold = 100;
	const double speed_damage_coef = 1;
	const double max_hero_speed = 10;
	const double strategic_camera_speed = 10;
	const double knockback_speed = 0;
	const double friction_coef = 0.1;
	const float fps_lock = 240;
	const double interaction_distance = 600;

	const float base_gold_income = 0.0045;
	const float base_research_income = 0.0005;
	const float base_turret_price = 150;
	const float base_science_price = 100;
	const float base_gold_price = 125;
	const float base_dome_price = 500;
	const float base_asteroid_gold = 7500;

    const float drone_cooldown = 1200;
    const int drone_count = 15;

	const float attack_abil_price = 3;
	const float speed_buff_price = 30;
	const float damage_buff_price = 50;
	const float heal_buff_price = 100;

	const float infinity_mode_enemy_spawn_range = 10000;
    const float defence_mission_enemy_spawn_range = 2500;
	const float minimal_flight_range = 450;
    const float drone_flight_add_range = 100;

	const float dome_heal = 0.1;
	const float hero_heal = 1;

	const float fire_damage = 0.25;
	const float regen_buff = 0.06;
	const float const_heal = 250;
	const float move_speed_buff_mult = 4;

	const float skills_icon_size = 120;
	const float skills_icon_shift_radius = 100;
	const float skills_icon_border = 80;

	const float research_icon_size = 120;
	const float research_icon_border = 100;

	const std::string balance_version = "0.1";

	const float hero_view_scale = 2;
	const float strategic_view_scale = 5;

	const float tier_range = 3000;
	const float discovery_range = 10000;

	const float minimal_mission_price = 100;
	const float max_random_mission_price_addition = 500;
	const float mission_price_change_coef = 0.975;
    const float courier_reward_coef = 1;
    const float defence_reward_coef = 5;

public:

	const double getSpeedDamageThreshold() {
		return speed_damage_threshold;
	}

	const double getSpeedDamageCoef() {
		return speed_damage_coef;
	}
	
	const double getMaxHeroSpeed() {
		return max_hero_speed;
	}

	const double getKnockbackSpeed() {
		return knockback_speed;
	}

	const double getFrictionCoef() {
		return friction_coef;
	}

	const float getFPSLock() {
		return fps_lock;
	}

	const double getInteractionDistance() {
		return interaction_distance;
	}

	const float getStrategicCameraSpeed() {
		return strategic_camera_speed;
	}

	const float getBaseGoldIncome() {
		return base_gold_income;
	}

	const float getBaseResearchIncome() {
		return base_research_income;
	}

	const float getBaseTurretPrice() {
		return base_turret_price;
	}

	const float getBaseDomePrice() {
		return base_dome_price;
	}

	const float getBaseSciencePrice() {
		return base_science_price;
	}

	const float getBaseGoldPrice() {
		return base_gold_price;
	}

	const float getInfiniteModeEnemySpawnRange() {
		return infinity_mode_enemy_spawn_range;
	}

    const float getDefenceMissionEnemySpawnRange() {
        return defence_mission_enemy_spawn_range;
    }

	const float getMinimalFlightRange() {
		return minimal_flight_range;
	}

	const float getDomeHeal() {
		return dome_heal;
	}

	const float getHeroHeal() {
		return hero_heal;
	}

	const float getFireDamage() {
		return fire_damage;
	}

	const float getRegenBuff() {
		return regen_buff;
	}

	const float getConstHeal() {
		return const_heal;
	}

	const float getAttackAbilityPrice() {
		return attack_abil_price;
	}

	const float getSpeedBuffPrice() {
		return speed_buff_price;
	}

	const float getDamageBuffPrice() {
		return damage_buff_price;
	}

	const float getHealBuffPrice() {
		return heal_buff_price;
	}

	const float getMoveSpeedBuffMultiplier() {
		return move_speed_buff_mult;
	}

	const float getSkillsIconSize() {
		return skills_icon_size;
	}

	const float getSkillsIconShiftRadius() {
		return skills_icon_shift_radius;
	}

	const float getSkillsIconBorder() {
		return skills_icon_border;
	}

	const float getResearchIconSize() {
		return research_icon_size;
	}

	const std::string getBalanceVersion() {
		return balance_version;
	}

	const float getHeroViewScale() {
		return hero_view_scale;
	}

	const float getStrategicViewScale() {
		return strategic_view_scale;
	}

	const float getTierRange() {
		return tier_range;
	}

	const float getDiscoveryRange() {
		return discovery_range;
	}

	const float getMinimalMissionPrice() {
		return minimal_mission_price;
	}

	const float getMaxRandomMissionPriceAddition() {
		return max_random_mission_price_addition;
	}

	const float getMissionPriceChangeCoef() {
		return mission_price_change_coef;
	}

    const float getCourierRewardCoef() {
        return courier_reward_coef;
    }

    const float getDefenceRewardCoef() {
        return defence_reward_coef;
    }

	const char * getDefaultFontPath() {
		return default_font;
	}

	const float getBaseAsteroidGold() {
		return base_asteroid_gold;
	}

    const float getDroneFlightAddRange() {
        return drone_flight_add_range;
    }

    const float getDroneCooldown() {
        return drone_cooldown;
    }

    const int getDroneCount() {
        return drone_count;
    }

} consts;
