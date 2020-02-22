#pragma once

#include <string>
#include <vector>

struct Objective {
    enum Type {
        null,
        point,
        numeric,
    }type;
    void * data;

    Objective() : type(null), data(nullptr) {}

    Objective(Type type, void * data) : type(type), data(data) {}
};


class MissionStage {
    enum Type {
        courier,
        defence,
    } type;
public:

    virtual Objective getObjective() { return Objective(); }
    virtual void setObjectiveCompleted() {}
    virtual std::wstring getCurrentDescription() { return L"Mission doesn't exist, sorry for bug"; }
    virtual std::wstring getShortDescription() { return L"Mission doesn't exist, sorry for bug"; }
    virtual std::wstring getBroadDescription() { return L"Mission doesn't exist, sorry for bug"; }
    virtual bool completed() { return true; }
};

class CourierStage : public MissionStage {
    void * objective;   // expected to contain asteroid objects pointers
    std::wstring current_description, short_description, broad_description;
public:

    CourierStage(void * courier_objective, std::wstring mission_type) : objective(courier_objective) {
        
    }

    Objective getObjective() override {
        Objective output;
        if (objective) {
            output.type = Objective::point;
            output.data = objective;
        }
        return output;
    }

    void setObjectiveCompleted() override {
        if (objective) {
            objective = nullptr;
        }
    }
    std::wstring getCurrentDescription() override {
        return current_description;
    }

    std::wstring getShortDescription() override {
        return short_description;
    }

    std::wstring getBroadDescription() override {
        return broad_description;
    }

    bool completed() override {
        return objective == nullptr;
    }
};

class DefenceStage : public MissionStage {

    void * objective;
    int wave_amount = 0, level = 0, wave_delay = 20;
    enum {
        object_search,
        get_ready,
        wave_active,
        finished
    } state;

    std::wstring short_description, broad_description, cur_desc_search, cur_desc_ready, cur_desc_wave_start, cur_desc_wave_active, cur_desc_finished;

public:
    DefenceStage(void * objective, int wave_amount, int wave_level, std::wstring mission_type) {
        state = object_search;


    }

    Objective getObjective() override {
        Objective output;
        switch (state) {
        case object_search:
            output.type = Objective::point;
            output.data = objective;
            break;
        case get_ready:
            output.type = Objective::numeric;
            output.data = new float(wave_delay);
            break;
        case wave_active:
            output.type = Objective::numeric;
            output.data = new float(level);
            break;
        case finished:
            break;
        }
        return output;
    }

    void setObjectiveCompleted() override {
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
    std::wstring getCurrentDescription() override {
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

    std::wstring getShortDescription() override {
        return short_description;
    }

    std::wstring getBroadDescription() override {
        return broad_description;
    }

    bool completed() override {
        return state == finished;
    }
};