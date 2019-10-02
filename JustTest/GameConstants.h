#pragma once

class Constants {

	const double speed_damage_threshold = 100;
	const double speed_damage_coef = 1;
	const double default_hero_speed = 1;
	const double strategic_camera_speed = 10;
	const double knockback_speed = 0;
	const double friction_coef = 0.01;
	const float fps_lock = 240;
	const double interaction_distance = 600;

	const float base_gold_income = 0.003;
	const float base_research_income = 0.0001;
	const float base_turret_price = 100;
	const float base_science_price = 100;
	const float base_gold_price = 100;
	const float base_dome_price = 100;

	const float attack_abil_price = 10;
	const float speed_buff_price = 30;
	const float damage_buff_price = 50;
	const float heal_buff_price = 100;

	const float enemy_spawn_range = 10000;
	const float minimal_flight_range = 450;

	const float dome_heal = 0.03;
	const float hero_heal = 0.03;

	const float fire_damage = 0.25;
	const float regen_buff = 0.06;
	const float const_heal = 250;
	const float move_speed_buff_mult = 5;

	const float skills_icon_size = 120;
	const float skills_icon_shift_radius = 100;
	const float skills_icon_border = 20;

	const float research_icon_size = 120;
	const float research_icon_border = 100;

public:

	const double getSpeedDamageThreshold() {
		return speed_damage_threshold;
	}

	const double getSpeedDamageCoef() {
		return speed_damage_coef;
	}
	
	const double getDefaultHeroSpeed() {
		return default_hero_speed;
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

	const float getEnemySpawnRange() {
		return enemy_spawn_range;
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
} consts;