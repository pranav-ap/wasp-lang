#pragma once
#include <iostream>
#include <string>
#include "Position.h"
#include "Keyword.h"
#include "Punctuation.h"

// Base Class

class TokenType
{
public:
	int line_num;
	int column_num;
	TokenType(int line_num, int column_num) : line_num(line_num), column_num(column_num) {};
};

// Sub Classes

class NumberLiteral : public TokenType
{
public:
	double value;
	NumberLiteral(double value, int line_num, int col_num) : value(value), TokenType(line_num, col_num) {};
};

class StringLiteral : public TokenType
{
public:
	std::string value;
	StringLiteral(std::string value, int line_num, int col_num) : TokenType(line_num, col_num) {};
};

class BooleanLiteral : public TokenType
{
public:
	bool value;
	BooleanLiteral(bool value, int line_num, int col_num) : value(value), TokenType(line_num, col_num) {};
};

class Identifier : public TokenType
{
public:
	std::string value;
	bool is_function;
	Identifier(std::string value, bool is_function, int line_num, int col_num) : value(value), is_function(is_function), TokenType(line_num, col_num) {};
};

class Keyword : public TokenType
{
public:
	KeywordType value;
	Keyword(KeywordType value, int line_num, int col_num) : value(value), TokenType(line_num, col_num) {};
};

class Punctuation : public TokenType
{
public:
	PunctuationType value;
	Punctuation(PunctuationType value, int line_num, int col_num) : value(value), TokenType(line_num, col_num) {};
};

class Unknown : public TokenType
{
public:
	std::string value;
	Unknown(std::string value, int line_num, int col_num) : value(value), TokenType(line_num, col_num) {};
};
