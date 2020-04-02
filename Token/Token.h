#pragma once

#ifdef TOKEN_EXPORTS
#define TOKEN_API __declspec(dllexport)
#else
#define TOKEN_API __declspec(dllimport)
#endif

#include <string>
#include <iostream>
#include <memory>
#include "TokenType.h"

struct TOKEN_API Token
{
	WTokenType type;
	std::string value;

	int line_num;
	int column_num;

	Token(WTokenType type, std::string value) : type(type), value(value), line_num(0), column_num(0) {};
	Token(WTokenType type, std::string value, int line_num, int column_num) : type(type), value(value), line_num(line_num), column_num(column_num) {};
	bool operator==(const Token& token) const;
};

using Token_ptr = std::shared_ptr<Token>;

TOKEN_API std::ostream& operator<<(std::ostream& os, const Token& token);
TOKEN_API std::ostream& operator<<(std::ostream& os, const Token_ptr& token);
