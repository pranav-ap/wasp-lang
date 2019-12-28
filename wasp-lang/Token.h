#pragma once
#include <string>
#include <memory>
#include "TokenType.h"

class Token
{
	TokenType type;
	std::string value;

	int line_num;
	int column_num;

public:
	Token(TokenType type, std::string value, int line_num, int column_num) : type(type), value(value), line_num(line_num), column_num(column_num) {};

	std::string get_value() const;
	TokenType get_type() const;

	int get_line_num() const;
	int get_column_num() const;

	void print() const;

	bool operator==(const Token& token) const;
};

using Token_ptr = std::shared_ptr<Token>;
