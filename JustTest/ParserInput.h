#pragma once

#include <fstream>
#include <iostream>

#include "Settings.h"

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