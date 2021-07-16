#pragma once

class Achievement {
	// achievements for this match
	float gold_collected_on_match = 0;
	int enemies_killed_on_match = 0;
	int asteroids_conquered = 0;
	int structures_built_on_match = 0;
	float damage_received_on_match = 0;

	// achievements for all time
	float gold_collected_all_time = 0;
	int enemies_killed_all_time = 0;
	int infinite_matches_played = 0;
	int adventure_matches_played = 0;
	int structures_built_all_time = 0;
	float damage_received_all_time = 0;

public:


	// this match
	void changeGoldCollectedOnMatch(float gold_income) {
		gold_collected_on_match += gold_income;
	}
	float getGoldCollectedOnMatch() {
		return gold_collected_on_match;
	}

	void changeEnemiesKilledOnMatch(float enemies_killed) {
		enemies_killed_on_match += enemies_killed;
	}
	float getEnemiesKilledOnMatch() {
		return enemies_killed_on_match;
	}

	void changeAsteroidsConquered(float new_asteroids) {
		asteroids_conquered += new_asteroids;
	}
	float getAsteroidsConquered() {
		return enemies_killed_on_match;
	}

	void changeStructuresBuiltOnMatch(float new_asteroids) {
		asteroids_conquered += new_asteroids;
	}
	float getStructuresBuiltOnMatch() {
		return enemies_killed_on_match;
	}
};