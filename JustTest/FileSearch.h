#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

std::vector<std::string> * getFileList(std::string folder) {

	std::vector<std::string> * output = new std::vector<std::string>;

	FILE * pipe;
	char buf[BUFSIZ];

	if ((pipe = _popen(("dir " + folder + " /B /N").c_str(), "r")) == NULL) {
		return output;
	}

	while (fgets(buf, BUFSIZ, pipe) != NULL) {
		std::string buffer = buf;
		buffer.pop_back();
		output->push_back(buffer);
	}

	_pclose(pipe);

	return output;
}