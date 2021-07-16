#pragma once

#include <fstream>
#include <iostream>
#include <locale>
#include <codecvt>

#include "../Utils/Settings.h"

std::wstring stringToWstring(const std::string& t_str)
{
    //setup converter
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.from_bytes(t_str);
}

std::string wstringToString(std::wstring wstr) {
    return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.to_bytes(wstr);
}

int getInt(std::wifstream & input_file) {
	std::wstring string;
	input_file >> string;
	int data_i;
	try {
		data_i = std::stoi(string);
	}
	catch (std::invalid_argument err) {
		if (settings.isErrorOutputEnabled()) {
			std::cout << "Invalid argument exception" << std::endl;
		}
		return 0;
	}
	catch (std::out_of_range err) {
		if (settings.isErrorOutputEnabled()) {
			std::cout << "Out of range exception" << std::endl;
		}
		return 0;
	}
	return data_i;
}

int getInt(std::ifstream & input_file) {
    std::string string;
    input_file >> string;
    int data_i;
    try {
        data_i = std::stoi(string);
    }
    catch (std::invalid_argument err) {
        if (settings.isErrorOutputEnabled()) {
            std::cout << "Invalid argument exception" << std::endl;
        }
        return 0;
    }
    catch (std::out_of_range err) {
        if (settings.isErrorOutputEnabled()) {
            std::cout << "Out of range exception" << std::endl;
        }
        return 0;
    }
    return data_i;
}

float getFloat(std::wifstream & input_file) {
	std::wstring string;
	input_file >> string;
	float data_f;
	try {
		data_f = std::stof(string);
	}
	catch (std::invalid_argument err) {
		if (settings.isErrorOutputEnabled()) {
			std::cout << "Invalid argument exception" << std::endl;
		}
		return 0;
	}
	catch (std::out_of_range err) {
		if (settings.isErrorOutputEnabled()) {
			std::cout << "Out of range exception" << std::endl;
		}
		return 0;
	}
	return data_f;
}

float getFloat(std::ifstream & input_file) {
    std::string string;
    input_file >> string;
    float data_f;
    try {
        data_f = std::stof(string);
    }
    catch (std::invalid_argument err) {
        if (settings.isErrorOutputEnabled()) {
            std::cout << "Invalid argument exception" << std::endl;
        }
        return 0;
    }
    catch (std::out_of_range err) {
        if (settings.isErrorOutputEnabled()) {
            std::cout << "Out of range exception" << std::endl;
        }
        return 0;
    }
    return data_f;
}
