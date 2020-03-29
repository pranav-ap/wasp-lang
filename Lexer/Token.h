#pragma once

#ifdef LEXER_EXPORTS
#define LEXER_API __declspec(dllexport)
#else
#define LEXER_API __declspec(dllimport)
#endif

#include <string>
#include <memory>
#include "TokenType.h"

class LEXER_API Token
{
	WTokenType type;
	std::string value;

	int line_num;
	int column_num;

public:
	Token(WTokenType type, std::string value) : type(type), value(value), line_num(0), column_num(0) {};
	Token(WTokenType type, std::string value, int line_num, int column_num) : type(type), value(value), line_num(line_num), column_num(column_num) {};

	std::string get_value() const;
	WTokenType get_type() const;

	int get_line_num() const;
	int get_column_num() const;

	void set_line_num(int num);
	void set_column_num(int num);

	void print() const;

	bool operator==(const Token& token) const;
};

using Token_ptr = std::shared_ptr<Token>;
