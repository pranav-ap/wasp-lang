#pragma once

#include "TokenType.h"
#include <string>
#include <iostream>
#include <memory>

struct Token
{
	WTokenType type;
	std::wstring value;

	int line_num;
	int column_num;

	Token(WTokenType type, std::wstring value, int line_num, int column_num)
		: type(type), value(value), line_num(line_num), column_num(column_num) {};
};

using Token_ptr = std::shared_ptr<Token>;
