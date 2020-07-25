#pragma once
#include "pch.h"
#include "utils.h"
#include "Assertion.h"
#include <fstream>
#include <regex>
#include <string>
#include <algorithm>

using DECIMAL_CODE_POINT = unsigned short;
using DECIMAL_CODE_POINT_VECTOR = std::vector<DECIMAL_CODE_POINT>;

using std::begin;
using std::end;
using std::ifstream;
using std::string;
using std::wstring;

bool valid_utf8_file(string path)
{
	ifstream fs(path);
	ASSERT(fs.is_open(), "File cannot be opened");

	std::istreambuf_iterator<char> start_it(fs.rdbuf());
	std::istreambuf_iterator<char> end_it;

	return utf8::is_valid(start_it, end_it);
}

std::wstring decimal_code_to_wstring(DECIMAL_CODE_POINT_VECTOR utf16_file)
{
	std::wstring wstring_file;

	for (wchar_t ch : utf16_file)
	{
		wstring_file.push_back(ch);
	}

	return wstring_file;
}

std::wstring read_source(string path)
{
	ASSERT(valid_utf8_file(path), "File encoding must be UTF-8");

	ifstream fs(path);
	ASSERT(fs.is_open(), "File cannot be opened");

	string line;
	std::wstring raw_source_file;

	while (getline(fs, line))
	{
		DECIMAL_CODE_POINT_VECTOR utf16_line;
		utf8::utf8to16(begin(line), end(line), back_inserter(utf16_line));

		std::wstring raw_source_line = decimal_code_to_wstring(utf16_line);

		if (raw_source_line.find_first_not_of(L" \t\n\v\f\r") != wstring::npos)
		{
			raw_source_line = std::regex_replace(raw_source_line, std::wregex(L"\t"), L"    ");
			raw_source_file.append(raw_source_line);
		}

		raw_source_file.append(L"\n");
	}

	return raw_source_file;
}