#pragma once
#include <string>
#include <vector>
#include "utf8.h"

#define UNSIGNED_SHORT (unsigned short)

using DECIMAL_CODE_POINT = unsigned short;
using DECIMAL_CODE_POINT_VECTOR = std::vector<DECIMAL_CODE_POINT>;

namespace Utils
{
	enum class CHARACTER : unsigned short
	{
		// Whitespace

		NUL = 0,
		TAB = 9,
		EOL = 10,
		CARRIAGE_RETURN = 13,
		SPACE = 32,

		// Punctuation

		APOSTROPHE = 39,

		PERCENT = 37,
		ASTERISK = 42,
		PLUS = 43,
		MINUS = 45,
		POWER = 94,

		COMMA = 44,
		DOT = 46,

		FORWARD_SLASH = 47,
		BACKWARD_SLASH = 92,

		COLON = 58,
		SEMICOLON = 59,

		LESS_THAN = 60,
		EQUAL = 61,
		EXCLAMATION_MARK = 33,
		GREATER_THAN = 62,

		VERTICAL_BAR = 124,
		UNDERSCORE = 95,

		LEFT_SQUARE_BRACKET = 91,
		RIGHT_SQUARE_BRACKET = 93,
		LEFT_CURLY_BRACE = 123,
		RIGHT_CURLY_BRACE = 125,
		LEFT_PARENTHESIS = 40,
		RIGHT_PARENTHESIS = 41,
		LEFT_ANGLE_BRACKET = 12296,
		RIGHT_ANGLE_BRACKET = 12297,
		LEFT_FLOOR_BRACKET = 8970,
		RIGHT_FLOOR_BRACKET = 8971,

		// Digits

		NUM_0 = 48,
		NUM_1 = 49,
		NUM_2 = 50,
		NUM_3 = 51,
		NUM_4 = 52,
		NUM_5 = 53,
		NUM_6 = 54,
		NUM_7 = 55,
		NUM_8 = 56,
		NUM_9 = 57,

		// Alphabet

		ALPHA_A = 65,
		ALPHA_Z = 90,

		ALPHA_a = 97,
		ALPHA_z = 122
	};

	const DECIMAL_CODE_POINT_VECTOR whitespace_characters = {
		UNSIGNED_SHORT CHARACTER::NUL,
		UNSIGNED_SHORT CHARACTER::TAB,
		UNSIGNED_SHORT CHARACTER::EOL,
		UNSIGNED_SHORT CHARACTER::CARRIAGE_RETURN,
		UNSIGNED_SHORT CHARACTER::SPACE
	};

	DECIMAL_CODE_POINT_VECTOR read_source(std::string path);
	bool check_line_is_empty(DECIMAL_CODE_POINT_VECTOR utf16_line);
}
