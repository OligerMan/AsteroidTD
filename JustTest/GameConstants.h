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
} consts;