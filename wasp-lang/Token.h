#pragma once
#include <iostream>
#include <string>
#include <variant>
#include <memory>
#include "Position.h"
#include "Keyword.h"
#include "Punctuation.h"

template <class T>
class Token {
	T value;
	std::string pos;
public:
	Token(T val, std::string pos) : value(val), pos(pos) {};

	friend std::ostream& operator<< (std::ostream& out, TokenType const& token) {
		visit([&out](auto const& t) { out << t.value << " - " << t.pos; }, token);
		return out;
	}

	friend std::ostream& operator<< (std::ostream& out, unique_ptr<TokenType> const& token) {
		visit([&out](auto const& t) { out << t.value << " - " << t.pos; }, *token);
		return out;
	}
};

typedef Token<double> NumberLiteral;
typedef Token<std::string> StringLiteral;
typedef Token<bool> BooleanLiteral;
typedef Token<KeywordType> Keyword;
typedef Token<PunctuationType> Punctuation;
typedef Token<char> Unknown;

typedef variant<NumberLiteral, StringLiteral, BooleanLiteral, Keyword, Punctuation, Unknown> TokenType;
