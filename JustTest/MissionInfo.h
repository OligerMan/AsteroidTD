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
    int min_lvl = 0;
    int max_lvl = 1e9;
public:
    BaseInfo(Type type, std::wstring id, int min_lvl, int max_lvl) :
        type(type),
        string_id(id),
        min_lvl(min_lvl),
        max_lvl(max_lvl) {}

    std::wstring getStringID() {
        return string_id;
    }

    int getMinLvl() {
        return min_lvl;
    }

    int getMaxLvl() {
        return max_lvl;
    }

    int getType() {
        return type;
    }
};

class CourierInfo : public BaseInfo {
public:
    CourierInfo(std::wstring id, int min_lvl, int max_lvl) : 
        BaseInfo(courier, id, min_lvl, max_lvl) {}
};

class DefenceInfo : public BaseInfo {
    unsigned int wave_delay, wave_amount;
public:
    DefenceInfo(std::wstring id, int min_lvl, int max_lvl, unsigned int wave_delay, unsigned int wave_amount) :
        BaseInfo(defence, id, min_lvl, max_lvl),
        wave_delay(wave_delay),
        wave_amount(wave_amount) {}

    unsigned int getWaveDelay() {
        return wave_delay;
    }

    unsigned int getWaveAmount() {
        return wave_amount;
    }
};

/*
Mission Parser Format

mission_list_start

mission_start
reward <reward>
<then required type of mission + list of parameters or mission end>
<examples:>
courier <string id> <min lvl> <max lvl>
defence <string id> <min lvl> <max lvl> <wave num> <wave delay>
mission_end

<some other mission descriptions>

mission_list_end

*/
class MissionStageInfo {
    std::vector<std::pair<float, std::vector<BaseInfo *>>> info;

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
                float reward_coef = 0;
                mission_file >> input;
                if (input == L"reward") {
                    if (settings.isErrorOutputEnabled()) {
                        std::wcout << L"Mission file is broken" << std::endl;
                    }
                }
                reward_coef = getFloat(mission_file);
                while (true) {
                    mission_file >> input;
                    if (input == L"mission_end") {
                        info.push_back(std::pair<float, std::vector<BaseInfo *>> (reward_coef, mission_description));
                        break;
                    }
                    if (input == L"courier") {
                        std::wstring id;
                        unsigned int min_lvl, max_lvl;
                        mission_file >> id;
                        min_lvl = getInt(mission_file);
                        max_lvl = getInt(mission_file);
                        mission_description.push_back(new CourierInfo(id, min_lvl, max_lvl));
                    }
                    if (input == L"defence") {
                        std::wstring id;
                        unsigned int min_lvl, max_lvl, wave_delay, wave_amount;
                        mission_file >> id;
                        min_lvl = getInt(mission_file);
                        max_lvl = getInt(mission_file);
                        wave_delay = getInt(mission_file);
                        wave_amount = getInt(mission_file);
                        mission_description.push_back(new DefenceInfo(id, min_lvl, max_lvl, wave_delay, wave_amount));
                    }
                }
            }
        }
    }

    std::pair<float, std::vector<BaseInfo *>> getRandomMissionDescription() {
        return info[info.size() * rand() / (RAND_MAX + 1)];
    }
};