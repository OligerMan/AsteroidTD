#pragma once

#include <vector>
#include <string>

#include "IDGenerator.h"

struct PointObjective {     // contains Object pointer to know it's position
	void * object_ptr;

	PointObjective(void * point_objective) : object_ptr(point_objective) {}
};

struct FloatObjective {
	float value;

	FloatObjective(float value) : value(value) {}
};

struct Objective {
	enum Type {
		null,
		point,
		asteroid,
		wave_level,
		wave_delay,
        wave_active,
	};

	Type type = null;
	void * objectiveExpansion = nullptr;
};

struct CourierMission {
	std::vector<void *> objectives;   // expected to contain asteroid objects pointers

	std::wstring current_description, short_description, broad_description;

	CourierMission(std::vector<void *> courier_objectives) {
		for (int i = 0; i < courier_objectives.size(); i++) {
			objectives.push_back(courier_objectives[i]);
		}

		current_description = L"Just deliver your cargo to the point";
		short_description = L"Standart courier mission";
		broad_description = L"You need to deliver this standard box to special point. Coordinates will be sent further";
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

	std::wstring getCurrentDescription() {
		return current_description;
	}

	std::wstring getShortDescription() {
		return short_description;
	}

	std::wstring getBroadDescription() {
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
		wave_active,
		finished
	} state;

    std::wstring short_description, broad_description, cur_desc_search, cur_desc_ready, cur_desc_wave_start, cur_desc_wave_active, cur_desc_finished;

	DefenceMission(void * objective, int wave_amount, int wave_level) : objective(objective), wave_amount(wave_amount), level(wave_level) {
        state = object_search;

        cur_desc_search = L"Find object what you need to defend";
        cur_desc_ready = L"Get ready!";
        cur_desc_wave_active = L"Kill all enemies";
        cur_desc_finished = L"Thanks for completed mission";
		short_description = L"Standart defence mission";
		broad_description = L"You need to defend asteroid from enemy waves. Coordinates will be sent further";
	}

	Objective getObjective() {
		Objective output;
		switch (state) {
		case object_search:
			output.type = Objective::point;
			output.objectiveExpansion = new PointObjective(objective);
			break;
		case get_ready:
			output.type = Objective::wave_delay;
			output.objectiveExpansion = new FloatObjective(wave_delay);
			break;
		case wave_active:
			output.type = Objective::wave_level;
			output.objectiveExpansion = new FloatObjective(level);
			break;
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
			state = wave_active;
			break;
		case wave_active:
			wave_amount--;
            level++;
			if (wave_amount > 0) {
				state = get_ready;
			}
			else {
				state = finished;
			}
			break;
		}
	}

	std::wstring getCurrentDescription() {
        std::wstring output;
        switch (state) {
        case object_search:
            output = cur_desc_search;
            break;
        case get_ready:
            output = cur_desc_ready;
            break;
        case wave_active:
            output = cur_desc_wave_active;
            break;
        case finished:
            output = cur_desc_finished;
            break;
        }
        return output;
	}

	std::wstring getShortDescription() {
		return short_description;
	}

	std::wstring getBroadDescription() {
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
        defence,

		TYPE_COUNT
	};
	float reward = 0;
	bool failed = false;
	Type type = null;
    unsigned long long id = id_generator.getID();

	void * missionExpansion = nullptr;

	Objective getObjective() {
		switch (type) {
		case null:
			break;
		case courier:
			return (static_cast<CourierMission *>(missionExpansion))->getObjective();
			break;
        case defence:
            return (static_cast<DefenceMission *>(missionExpansion))->getObjective();
            break;
		};
		return Objective();
	}

	void setObjectiveCompleted() {
		switch (type) {
		case courier:
			(static_cast<CourierMission *>(missionExpansion))->setObjectiveCompleted();
			break;
        case defence:
            (static_cast<DefenceMission *>(missionExpansion))->setObjectiveCompleted();
            break;
		};
	}

	std::wstring getCurrentDescription() {
		std::wstring output;
		switch (type) {
		case null:
			output = L"Mission doesn't exist, sorry for bug";
			break;
		case courier:
			output = (static_cast<CourierMission *>(missionExpansion))->getCurrentDescription();
			break;
        case defence:
            output = (static_cast<DefenceMission *>(missionExpansion))->getCurrentDescription();
            break;
		}
		return output;
	}

	std::wstring getBroadDescription() {
		std::wstring output;
		switch (type) {
		case null:
			output = L"Mission doesn't exist, sorry for bug";
			break;
		case courier:
			output = (static_cast<CourierMission *>(missionExpansion))->getBroadDescription();
			break;
        case defence:
            output = (static_cast<DefenceMission *>(missionExpansion))->getBroadDescription();
            break;
		}
		return output;
	}

	std::wstring getShortDescription() {
		std::wstring output;
		switch (type) {
		case null:
			output = L"Mission doesn't exist, sorry for bug";
			break;
		case courier:
			output = (static_cast<CourierMission *>(missionExpansion))->getShortDescription();
			break;
        case defence:
            output = (static_cast<DefenceMission *>(missionExpansion))->getShortDescription();
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
        case defence:
            output = (static_cast<DefenceMission *>(missionExpansion))->completed();
            break;
		};
		return output;
	}

	void clear() {
		switch (type) {
		case courier:
			delete (static_cast<CourierMission *>(missionExpansion));
			break;
        case defence:
            delete (static_cast<DefenceMission *>(missionExpansion));
            break;
		};
	}
};