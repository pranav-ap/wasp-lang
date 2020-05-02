#include "pch.h"
#include "file_io.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using std::string;
using std::ifstream;
using std::getline;

std::string read_source(string path)
{
	ifstream code(path);
	string line;
	string raw_source;

	while (getline(code, line))
	{
		raw_source.append(line);
		raw_source.append("\n");
	}

	raw_source = std::regex_replace(raw_source, std::regex("\t"), "    ");

	return raw_source;
}