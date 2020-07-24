#pragma once
#include "utils.h"
#include "Assertion.h"
#include <fstream>
#include <regex>
#include <string>
#include <algorithm>

using std::begin;
using std::end;
using std::ifstream;
using std::string;
using std::wstring;

bool valid_utf8_file(string path)
{
	ifstream fs(path);
	Utils::ASSERT(fs.is_open(), "File cannot be opened.");

	std::istreambuf_iterator<char> start_it(fs.rdbuf());
	std::istreambuf_iterator<char> end_it;

	return utf8::is_valid(start_it, end_it);
}

STRING decimal_code_to_wstring(DECIMAL_CODE_POINT_VECTOR utf16_file)
{
	STRING str;

	for (CHARACTER ch : utf16_file)
	{
		str.push_back(ch);
	}

	return str;
}

STRING Utils::read_source(string path)
{
	ASSERT(valid_utf8_file(path), "File encoding must be UTF-8");

	ifstream fs(path);
	ASSERT(fs.is_open(), "File cannot be opened");

	string line;
	STRING raw_source_file;

	while (getline(fs, line))
	{
		DECIMAL_CODE_POINT_VECTOR utf16_line;
		utf8::utf8to16(begin(line), end(line), back_inserter(utf16_line));

		STRING raw_source_line = decimal_code_to_wstring(utf16_line);

		if (raw_source_line.find_first_not_of(L" \t\n\v\f\r") != wstring::npos)
		{
			raw_source_line = std::regex_replace(raw_source_line, std::wregex(L"\t"), L"    ");
			raw_source_file.append(raw_source_line);
		}

		raw_source_file.push_back('\n');
	}

	return raw_source_file;
}