#include "pch.h"
#include "file_io.h"
#include <iostream>
#include <fstream>
#include <string>

using std::string;
using std::ifstream;
using std::getline;

string read_source(string path)
{
	ifstream code(path);
	string line;
	string raw_source;

	while (getline(code, line))
	{
		if (line.find_first_not_of(" \t\n\v\f\r") != std::string::npos)
		{
			raw_source.append(line);
			raw_source.append("\n");
		}
	}

	return raw_source;
}