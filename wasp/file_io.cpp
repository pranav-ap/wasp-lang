#pragma once
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include "file_io.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <locale>
#include <codecvt>

using std::wstring;
using std::string;
using std::ifstream;
using std::getline;

std::wstring convert(const std::string& input)
{
	try
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(input);
	}
	catch (std::range_error& e)
	{
		size_t length = input.length();
		std::wstring result;
		result.reserve(length);

		for (size_t i = 0; i < length; i++)
		{
			result.push_back(input[i] & 0xFF);
		}

		return result;
	}
}

wstring read_source(string path)
{
	std::ifstream code(path);
	std::string line;
	std::string raw_source;

	while (getline(code, line))
	{
		if (line.find_first_not_of(" \t\n\v\f\r") != std::string::npos)
		{
			line = std::regex_replace(line, std::regex("\t"), "    ");
			raw_source.append(line);
		}

		raw_source.append("\n");
	}

	wstring unicode_raw_source = convert(raw_source);

	return unicode_raw_source;
}