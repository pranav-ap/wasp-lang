#pragma once

#ifdef LEXER_EXPORTS
#define LEXER_API __declspec(dllexport)
#else
#define LEXER_API __declspec(dllimport)
#endif

#include <string>
#include <iostream>
#include <memory>
#include "TokenType.h"

struct LEXER_API Token
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

LEXER_API std::ostream& operator<<(std::ostream& os, const Token& t);
