#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

#if defined(__linux__) || defined(__APPLE__)
#define path_separator std::string("/")
#elif _WIN32
#define path_separator std::string("\\")
#endif

std::vector<std::string> * getFileList(std::string folder) {

	std::vector<std::string> * output = new std::vector<std::string>;

	FILE * pipe;
	char buf[BUFSIZ];
#if  defined(__linux__) || defined(__APPLE__)
	if ((pipe = popen(("ls " + folder).c_str(), "r")) == NULL) {
#elif _WIN32
	if ((pipe = _popen(("dir " + folder + " /B /N").c_str(), "r")) == NULL) {
#endif
		return output;
	}

	while (fgets(buf, BUFSIZ, pipe) != NULL) {
		std::string buffer = buf;
		buffer.pop_back();
		output->push_back(buffer);
	}
#if  defined(__linux__) || defined(__APPLE__)
	pclose(pipe);
#elif _WIN32
	_pclose(pipe);
#endif

	return output;
}