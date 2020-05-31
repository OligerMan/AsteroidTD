#pragma once

#include <string>
#include <vector>

#include "MissionInfo.h"
#include "PhraseContainer.h"

struct Objective {
    enum Type {
        null,
        point,
        wave_delay,
        wave_level
    }type;
    void * data;

    Objective() : type(null), data(nullptr) {}

    Objective(Type type, void * data) : type(type), data(data) {}
};


class MissionStage {
public:
    enum Type {
        courier,
        defence,

        TYPE_COUNT
    };
private:
    Type type;
public:

    MissionStage(Type type) : type(type) {}
    virtual Objective getObjective() { return Objective(); }
    virtual void setObjectiveCompleted() {}
    virtual std::wstring getCurrentDescription() { return L"Mission doesn't exist, sorry for bug"; }
    virtual std::wstring getShortDescription() { return L"Mission doesn't exist, sorry for bug"; }
    virtual std::wstring getBroadDescription() { return L"Mission doesn't exist, sorry for bug"; }
    virtual bool completed() { return true; }
    Type getType() { return type; }
};

class CourierStage : public MissionStage {
    void * objective;   // expected to contain asteroid objects pointers
    std::wstring current_description, short_description, broad_description;
public:

    CourierStage(void * courier_objective, std::wstring mission_type) : objective(courier_objective), MissionStage(courier) {
        //auto buffer = phrase_container.getPhraseBuffer(PhraseContainer::courier_current_desc_miss, 0);
        auto buffer = phrase_container.getPhraseBuffer(L"courier_current_desc_miss", 0);
        current_description = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        //buffer = phrase_container.getPhraseBuffer(PhraseContainer::courier_short_desc_miss, 0);
        buffer = phrase_container.getPhraseBuffer(L"courier_short_desc_miss", 0);
        short_description = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        //buffer = phrase_container.getPhraseBuffer(PhraseContainer::courier_broad_desc_miss, 0);
        buffer = phrase_container.getPhraseBuffer(L"courier_broad_desc_miss", 0);
        broad_description = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
    }

    Objective getObjective() override {
        Objective output;
        std::cout << "kek" << std::endl;
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
    DefenceStage(void * objective, DefenceInfo info, int mission_level) : objective(objective), MissionStage(defence) {

        state = object_search;
        wave_amount = info.getWaveAmount();
        level = mission_level;
        wave_delay = info.getWaveDelay();

        //auto buffer = phrase_container.getPhraseBuffer(PhraseContainer::defence_short_desc_miss, 0);
        auto buffer = phrase_container.getPhraseBuffer(L"defence_short_desc_miss", 0);
        short_description = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        //buffer = phrase_container.getPhraseBuffer(PhraseContainer::defence_broad_desc_miss, 0);
        buffer = phrase_container.getPhraseBuffer(L"defence_broad_desc_miss", 0);
        broad_description = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        //buffer = phrase_container.getPhraseBuffer(PhraseContainer::defence_cur_search_desc_miss, 0);
        buffer = phrase_container.getPhraseBuffer(L"defence_cur_search_desc_miss", 0);
        cur_desc_search = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        //buffer = phrase_container.getPhraseBuffer(PhraseContainer::defence_cur_ready_desc_miss, 0);
        buffer = phrase_container.getPhraseBuffer(L"defence_cur_ready_desc_miss", 0);
        cur_desc_ready = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        //buffer = phrase_container.getPhraseBuffer(PhraseContainer::defence_cur_wave_start_desc_miss, 0);
        buffer = phrase_container.getPhraseBuffer(L"defence_cur_wave_start_desc_miss", 0);
        cur_desc_wave_start = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        //buffer = phrase_container.getPhraseBuffer(PhraseContainer::defence_cur_wave_active_desc_miss, 0);
        buffer = phrase_container.getPhraseBuffer(L"defence_cur_wave_active_desc_miss", 0);
        cur_desc_wave_active = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        //buffer = phrase_container.getPhraseBuffer(PhraseContainer::defence_cur_finished_desc_miss, 0);
        buffer = phrase_container.getPhraseBuffer(L"defence_cur_finished_desc_miss", 0);
        cur_desc_finished = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
    }

    Objective getObjective() override {
        Objective output;
        switch (state) {
        case object_search:
            output.type = Objective::point;
            output.data = objective;
            break;
        case get_ready:
            output.type = Objective::wave_delay;
            output.data = new float(wave_delay);
            break;
        case wave_active:
            output.type = Objective::wave_level;
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