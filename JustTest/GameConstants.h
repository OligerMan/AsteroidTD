#pragma once

class Constants {

	const double speed_damage_threshold = 100;
	const double speed_damage_coef = 1;
	const double default_hero_speed = 1;
	const double knockback_speed = 0;
	const double friction_coef = 0.01;

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
} consts;