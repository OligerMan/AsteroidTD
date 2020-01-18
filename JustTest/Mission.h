#pragma once

struct PointObjective {
	void * object_ptr;

	PointObjective(void * point_objective) : object_ptr(point_objective) {}
};

struct Objective {
	enum Type {
		null,
		point,
	};

	Type type = null;
	void * objectiveExpansion = nullptr;
};

struct CourierMission {
	std::vector<void *> courier_objectives;

	CourierMission(std::vector<void *> courier_objectives) : courier_objectives(courier_objectives) {}

	void nextStep() {
		courier_objectives.erase(courier_objectives.begin());
	}

	void getObjective() {
		Objective output;
		if (courier_objectives.size()) {
			output.type = Objective::Type::point;
			//output.objectiveExpansion = 
		}
	}

	bool completed() {
		return courier_objectives.size() == 0;
	}
};

struct Mission {
	enum Type {
		courier,
	};
	float price = 0;
	Type type;

	void * missionExpansion;

	Objective getObjective() {
		switch (type) {
		case courier:
			break;
		};
		return Objective();
	}

	void nextStep() {
		switch (type) {
		case courier:
			break;
		};
	}

	bool completed() {

	}
};