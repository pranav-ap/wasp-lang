#pragma once
#include "utils.h"
#include "Assertion.h"
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <codecvt>

using std::begin;
using std::end;
using std::ifstream;
using std::string;
using std::find_if_not;

bool valid_utf8_file(string path)
{
	ifstream fs(path);
	Utils::ASSERT(fs.is_open(), "File cannot be opened.");

	std::istreambuf_iterator<char> start_it(fs.rdbuf());
	std::istreambuf_iterator<char> end_it;

	return utf8::is_valid(start_it, end_it);
}

bool Utils::check_line_is_empty(DECIMAL_CODE_POINT_VECTOR utf16_line)
{
	const bool is_empty_line = std::none_of(
		begin(utf16_line),
		end(utf16_line),
		[=](DECIMAL_CODE_POINT decimal_code_point)
		{
			return std::find(
				begin(whitespace_characters),
				end(whitespace_characters),
				decimal_code_point
			) == end(whitespace_characters);
		}
	);

	return is_empty_line;
}

DECIMAL_CODE_POINT_VECTOR Utils::read_source(string path)
{
	ifstream fs(path);
	ASSERT(valid_utf8_file(path), "File encoding must be UTF-8");
	ASSERT(fs.is_open(), "File cannot be opened");

	string line;
	DECIMAL_CODE_POINT_VECTOR utf16_file;

	while (getline(fs, line))
	{
		// Convert to UTF-16

		DECIMAL_CODE_POINT_VECTOR utf16_line;
		utf8::utf8to16(begin(line), end(line), back_inserter(utf16_line));

		const bool is_empty_line = check_line_is_empty(utf16_line);

		// ADD '\n' escape character & Replace Tabs with 4 Spaces

		if (!is_empty_line)
		{
			for (DECIMAL_CODE_POINT decimal_code_point : utf16_line)
			{
				if (decimal_code_point == UNSIGNED_SHORT CHARACTER::TAB)
				{
					std::fill_n(end(utf16_file), 4, UNSIGNED_SHORT CHARACTER::SPACE);
				}
				else
				{
					utf16_file.push_back(decimal_code_point);
				}
			}
		}

		utf16_file.push_back(UNSIGNED_SHORT CHARACTER::EOL);
	}

	return utf16_file;
}