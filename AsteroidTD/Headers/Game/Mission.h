#pragma once

#include <vector>
#include <string>

#include "../Utils/IDGenerator.h"
#include "MissionStage.h"

class Mission {
    std::vector<MissionStage *> task_list;
    float reward = 0;
    bool failed = false;
    unsigned long long id;

public:

    Mission(float reward) : reward(reward) {
        id = id_generator.getID();
    }

    MissionStage::Type getMissionStageType() {
        return task_list.empty() ? MissionStage::TYPE_COUNT : task_list[0]->getType();
    }

    void addMissionStage(MissionStage * stage) {
        task_list.push_back(stage);
    }

    unsigned long long getID() {
        return id;
    }

    float getReward() {
        return reward;
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
            if (task_list[0]->completed()) {
                task_list.erase(task_list.begin());
            }
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
        return task_list.empty();
    }

    void clear() {
        task_list.clear();
    }

    void setFailState() {
        failed = true;
    }

    bool isFailed() {
        return failed;
    }
};
