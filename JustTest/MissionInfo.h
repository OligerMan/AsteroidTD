#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "Settings.h"
#include "ParserInput.h"

class BaseInfo {
public:
    enum Type {
        courier,
        defence,

        TYPE_COUNT
    };
private:
    Type type = TYPE_COUNT;
    std::wstring string_id;
    float reward_coef;
    int min_lvl = 0;
    int max_lvl = 1e9;
public:
    BaseInfo(Type type, std::wstring id, float reward_coef, int min_lvl, int max_lvl) :
        type(type),
        string_id(id),
        reward_coef(reward_coef),
        min_lvl(min_lvl),
        max_lvl(max_lvl) {}

    std::wstring getStringID() {
        return string_id;
    }

    float getRewardCoef() {
        return reward_coef;
    }

    int getMinLvl() {
        return min_lvl;
    }

    int getMaxLvl() {
        return max_lvl;
    }
};

class CourierInfo : public BaseInfo {
public:
    CourierInfo(std::wstring id, float reward_coef, int min_lvl, int max_lvl) : 
        BaseInfo(courier, id, reward_coef, min_lvl, max_lvl) {}
};

class DefenceInfo : public BaseInfo {
    unsigned int wave_num, wave_delay;
public:
    DefenceInfo(std::wstring id, float reward_coef, int min_lvl, int max_lvl, int wave_num, int wave_delay) : 
        BaseInfo(courier, id, reward_coef, min_lvl, max_lvl), 
        wave_num(wave_num), 
        wave_delay(wave_delay) {}
};

/*
Mission Parser Format

mission_list_start

mission_start
<then required type of mission + list of parameters or mission end>
<examples:>
courier <string id> <reward coef> <min lvl> <max lvl>
defence <string id> <reward coef> <min lvl> <max lvl> <wave num> <wave delay>
mission_end

<some other mission descriptions>

mission_list_end

*/
class MissionStageInfo {
    std::vector<std::vector<BaseInfo *>> info;

public:

    MissionStageInfo(std::wstring path) {

        std::wifstream mission_file(path);
        if (!mission_file.is_open()) {
            return;
        }

        std::wstring input;
        mission_file >> input;

        if (input != L"mission_list_start") {
            if (settings.isErrorOutputEnabled()) {
                std::wcout << L"Mission file is broken" << std::endl;
            }
        }

        while (true) {
            mission_file >> input;
            if (input == L"") {
                if (settings.isErrorOutputEnabled()) {
                    std::wcout << L"Mission file is broken" << std::endl;
                }
                break;
            }
            if (input == L"mission_list_end"){
                break;
            }
            if (input == L"mission_start") {
                std::vector<BaseInfo *> mission_description;
                while (true) {
                    mission_file >> input;
                    if (input == L"mission_end") {
                        break;
                    }
                    if (input == L"courier") {
                        std::wstring id;
                        float reward_coef;
                        unsigned int min_lvl, max_lvl;
                        mission_file >> id;
                        reward_coef = getFloat(mission_file);
                        min_lvl = getInt(mission_file);
                        max_lvl = getInt(mission_file);
                        mission_description.push_back(new CourierInfo(id, reward_coef, min_lvl, max_lvl));
                    }
                    if (input == L"defence") {
                        std::wstring id;
                        float reward_coef;
                        unsigned int min_lvl, max_lvl, wave_num, wave_delay;
                        mission_file >> id;
                        reward_coef = getFloat(mission_file);
                        min_lvl = getInt(mission_file);
                        max_lvl = getInt(mission_file);
                        wave_num = getInt(mission_file);
                        wave_delay = getInt(mission_file);
                        mission_description.push_back(new DefenceInfo(id, reward_coef, min_lvl, max_lvl, wave_num, wave_delay));
                    }
                }
            }
        }
    }

    std::vector<BaseInfo *> getRandomMissionDescription() {
        return info[info.size() * rand() / (RAND_MAX + 1)];
    }
};