#pragma once

#include <map>
#include <string>

class DialogAdditionalInfo {
    std::map<std::wstring, std::wstring> data;

public:

    void setData(std::wstring & id, std::wstring & value) {
        auto iter = data.find(id);
        if (iter == data.end()) {
            data[id] = value;
        }
        else {
            iter->second = value;
        }
    }

    std::wstring getData(std::wstring & id) {
        auto iter = data.find(id);
        if (iter == data.end()) {
            return std::wstring();
        }
        return iter->second;
    }
} dialog_additional_info;