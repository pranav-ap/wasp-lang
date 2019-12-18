#pragma once
#include "Position.h"
#include "Keyword.h"
#include "Punctuation.h"
#include <iostream>
#include <string>
#include <vector>

class Token
{
public:
	std::string position;
	virtual void print();
};

// NumberLiteral

class NumberLiteral : public Token
{
	double number;

public:
	NumberLiteral(double value, std::string pos);
	void print();
};

// StringLiteral

class StringLiteral : public Token
{
	std::string string_literal;

public:
	StringLiteral(std::string value, std::string pos);
	void print();
};

// BooleanLiteral

class BooleanLiteral : public Token
{
	bool truth_value;

public:
	BooleanLiteral(bool value, std::string pos);
	void print();
};

// Identifier

class Identifier : public Token
{
	std::string name;

public:
	Identifier(std::string value, std::string pos);
	void print();
};

// FunctionIdentifier

class FunctionIdentifier : public Token
{
	std::string name;

public:
	FunctionIdentifier(std::string value, std::string pos);
	void print();
};

// Punctuation

class Punctuation : public Token
{
	PunctuationType punc;

public:
	Punctuation(PunctuationType value, std::string pos);
	void print();
	bool type_is(PunctuationType punctuation_type);
};

// Keyword

class Keyword : public Token
{
	KeywordType keyword;

public:
	Keyword(KeywordType value, std::string pos);
	void print();
	bool type_is(KeywordType keyword_type);
};

// Unknown

class Unknown : public Token
{
	std::string word;

public:
	Unknown(std::string value, std::string pos);
	void print();
};
