#pragma once

#include <vector>
#include <string>

struct PointObjective {
	void * object_ptr;

	PointObjective(void * point_objective) : object_ptr(point_objective) {}
};

struct FloatObjective {
	float value;

	FloatObjective(float value) : value(value) {}
};

struct AsteroidObjective {
	void * object_ptr;

	AsteroidObjective(void * asteroid_objective) : object_ptr(asteroid_objective) {}
};

struct Objective {
	enum Type {
		null,
		point,
		asteroid,
		wave_level,
		wave_delay,
	};

	Type type = null;
	void * objectiveExpansion = nullptr;
};

struct CourierMission {
	std::vector<void *> objectives;   // expected to contain asteroid objects pointers

	std::string current_description, short_description, broad_description;

	CourierMission(std::vector<void *> courier_objectives) {
		for (int i = 0; i < courier_objectives.size(); i++) {
			objectives.push_back(courier_objectives[i]);
		}

		current_description = "Just deliver your cargo to the point";
		short_description = "Standart courier mission";
		broad_description = "You need to deliver this standard box to special point. Coordinates will be sent further";
	}

	Objective getObjective() {
		Objective output;
		if (objectives.size()) {
			output.type = Objective::Type::point;
			output.objectiveExpansion = new PointObjective(objectives[0]);
		}
		return output;
	}

	void setObjectiveCompleted() {
		if (objectives.size()) {
			objectives.erase(objectives.begin());
		}
	}

	std::string getCurrentDescription() {
		return current_description;
	}

	std::string getShortDescription() {
		return short_description;
	}

	std::string getBroadDescription() {
		return broad_description;
	}

	bool completed() {
		return objectives.size() == 0;
	}
};

struct DefenceMission {
	void * objective;
	int wave_amount = 0, level = 0, wave_delay = 20;
	enum {
		object_search,
		get_ready,
		wave_start,
		wave_active,
		finished
	} state;

	std::string current_description, short_description, broad_description;

	DefenceMission(void * objective, int wave_amount, int wave_level) : objective(objective), wave_amount(wave_amount), level(wave_level) {

		current_description = "Defence object from enemies";
		short_description = "Standart defence mission";
		broad_description = "You need to defend asteroid from enemy waves. Coordinates will be sent further";
	}

	Objective getObjective() {
		Objective output;
		switch (state) {
		case object_search:
			output.type = Objective::asteroid;
			output.objectiveExpansion = new AsteroidObjective(objective);
			break;
		case get_ready:
			output.type = Objective::wave_delay;
			output.objectiveExpansion = new FloatObjective(wave_delay);
			break;
		case wave_start:
			output.type = Objective::wave_level;
			output.objectiveExpansion = new FloatObjective(level);
			break;
		case wave_active:
		case finished:
			break;
		}
		return output;
	}

	void setObjectiveCompleted() {
		switch (state) {
		case object_search:
			state = get_ready;
			break;
		case get_ready:
			state = wave_start;
			break;
		case wave_start:
			state = wave_active;
			break;
		case wave_active:
			wave_amount--;
			if (wave_amount) {
				state = get_ready;
			}
			else {
				state = finished;
			}
			break;
		}
	}

	std::string getCurrentDescription() {
		return current_description;
	}

	std::string getShortDescription() {
		return short_description;
	}

	std::string getBroadDescription() {
		return broad_description;
	}

	bool completed() {
		return state == finished;
	}
};

struct Mission {
	enum Type {
		null,
		courier,

		TYPE_COUNT
	};
	float reward = 0;
	bool failed = false;
	Type type = null;

	void * missionExpansion = nullptr;

	Objective getObjective() {
		switch (type) {
		case null:
			break;
		case courier:
			return (static_cast<CourierMission *>(missionExpansion))->getObjective();
			break;
		};
		return Objective();
	}

	void setObjectiveCompleted() {
		switch (type) {
		case courier:
			(static_cast<CourierMission *>(missionExpansion))->setObjectiveCompleted();
			break;
		};
	}

	std::string getCurrentDescription() {
		std::string output;
		switch (type) {
		case null:
			output = "Mission doesn't exist, sorry for bug";
			break;
		case courier:
			output = (static_cast<CourierMission *>(missionExpansion))->getCurrentDescription();
			break;
		}
		return output;
	}

	std::string getBroadDescription() {
		std::string output;
		switch (type) {
		case null:
			output = "Mission doesn't exist, sorry for bug";
			break;
		case courier:
			output = (static_cast<CourierMission *>(missionExpansion))->getBroadDescription();
			break;
		}
		return output;
	}

	std::string getShortDescription() {
		std::string output;
		switch (type) {
		case null:
			output = "Mission doesn't exist, sorry for bug";
			break;
		case courier:
			output = (static_cast<CourierMission *>(missionExpansion))->getShortDescription();
			break;
		}
		return output;
	}

	bool completed() {
		bool output = false;
		switch (type) {
		case courier:
			output = (static_cast<CourierMission *>(missionExpansion))->completed();
			break;
		};
		return output;
	}

	void clear() {
		switch (type) {
		case courier:
			delete (static_cast<CourierMission *>(missionExpansion));
			break;
		};
	}
};