#pragma once

#include <fstream>
#include <iostream>

#include "Settings.h"

int getInt(std::ifstream & input_file) {
	std::string string;
	input_file >> string;
	try {
		int data_i = std::stoi(string);
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
}

float getFloat(std::ifstream & input_file) {
	std::string string;
	input_file >> string;
	try {
		float data_f = std::stof(string);
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
}