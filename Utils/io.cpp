#include "pch.h"
#include "io.h"
#include <iostream>
#include <fstream>

using std::string;
using std::ifstream;
using std::getline;

std::string read_source(std::string path)
{
	ifstream infile(path);
	string line;
	string raw_source;

	while (getline(infile, line))
	{
		raw_source.append(line);
		raw_source.append("\n");
	}

	return raw_source;
}