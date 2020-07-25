#pragma once

#ifdef TOKEN_EXPORTS
#define TOKEN_API __declspec(dllexport)
#else
#define TOKEN_API __declspec(dllimport)
#endif

#include "TokenType.h"
#include <string>
#include <memory>

struct TOKEN_API Token
{
	WTokenType type;
	std::wstring value;

	int line_num;
	int column_num;

	Token(WTokenType type, std::wstring value, int line_num, int column_num)
		: type(type), value(value), line_num(line_num), column_num(column_num) {};
};

using Token_ptr = TOKEN_API std::shared_ptr<Token>;
