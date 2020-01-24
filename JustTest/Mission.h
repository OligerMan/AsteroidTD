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
	std::vector<void *> objectives;   // expected to contain asteroid objects pointers

	CourierMission(std::vector<void *> courier_objectives) {
		for (int i = 0; i < courier_objectives.size(); i++) {
			objectives.push_back(courier_objectives[i]);
		}
	}

	void nextStep() {
		objectives.erase(objectives.begin());
	}

	Objective getObjective() {
		Objective output;
		if (objectives.size()) {
			output.type = Objective::Type::point;
			output.objectiveExpansion = new PointObjective(objectives[0]);
		}
		return output;
	}

	std::string getCurrentDescription() {
		return "Just deliver your cargo to the point";
	}

	std::string getShortDescription() {
		return "Standart courier mission";
	}

	std::string getBroadDescription() {
		return "You need to deliver this standard box to special point. Coordinates will be sent further";
	}

	bool completed() {
		return objectives.size() == 0;
	}
};

struct Mission {
	enum Type {
		null,
		courier,

		TYPE_COUNT
	};
	float reward = 0;
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

	void nextStep() {
		switch (type) {
		case null:
			break;
		case courier:
			(static_cast<CourierMission *>(missionExpansion))->nextStep();
			break;
		};
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