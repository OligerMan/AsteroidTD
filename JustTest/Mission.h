#pragma once

#include <vector>
#include <string>

#include "IDGenerator.h"
#include "MissionStage.h"

class Mission {
    std::wstring type;
    std::vector<MissionStage *> task_list;
    float reward = 0;
    bool failed = false;
    unsigned long long id;

public:

    Mission(std::wstring type, float reward) : type(type), reward(reward) {
        id = id_generator.getID();
    }

    void addMissionStage(MissionStage * stage) {
        task_list.push_back(stage);
    }

    unsigned long long getID() {
        return id;
    }

    Objective getObjective() {
        if (!task_list.empty()) {
            return task_list[0]->getObjective();
        }
        else {
            return Objective();
        }
    }

    void setObjectiveCompleted() {
        if (!task_list.empty()) {
            task_list[0]->setObjectiveCompleted();
        }
    }

    std::wstring getCurrentDescription() {
        if (!task_list.empty()) {
            return task_list[0]->getCurrentDescription();
        }
        return std::wstring();
    }

    std::wstring getBroadDescription() {
        if (!task_list.empty()) {
            return task_list[0]->getBroadDescription();
        }
        return std::wstring();
    }

    std::wstring getShortDescription() {
        if (!task_list.empty()) {
            return task_list[0]->getShortDescription();
        }
        return std::wstring();
    }

    bool completed() {
        if (!task_list.empty()) {
            return task_list[0]->completed();
        }
        return true;
    }

    void clear() {
        task_list.clear();
    }
};