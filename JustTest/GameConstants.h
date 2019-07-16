#pragma once

class Constants {

	const double speed_damage_threshold = 100;
	const double speed_damage_coef = 1;
	const double default_hero_speed = 1;
	const double strategic_camera_speed = 10;
	const double knockback_speed = 0;
	const double friction_coef = 0.01;
	const int fps_lock = 240;
	const double interaction_distance = 600;

	const float base_gold_income = 0.003;
	const float base_research_income = 0.0001;
	const float base_turret_price = 100;
	const float base_science_price = 100;
	const float base_gold_price = 100;
	const float base_dome_price = 100;

	const float enemy_spawn_range = 10000;
	const float minimal_flight_range = 500;

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

	const int getFPSLock() {
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
} consts;