#include "stdafx.h"
// #include "loader.h"

#include <string>

#include <vector>
#include <iostream>
#include <sstream>
#include <istream>
#include <fstream>
#include <iterator>

#include "glm\glm.hpp"

bool get_group_length(std::string path, unsigned int* gl) {

	std::string line;
	path += "\\grouplength";

	std::ifstream gl_file(path.c_str());
	if (!gl_file.is_open()) {
		perror("Cannot open grouplength file. Wrong path?");
		return false;
	}
	getline(gl_file, line);
	*gl = atoi(line.c_str());
	return true;
}
