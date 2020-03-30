#pragma once

#ifdef LEXER_EXPORTS
#define LEXER_API __declspec(dllexport)
#else
#define LEXER_API __declspec(dllimport)
#endif

#include <string>
#include <iostream>

LEXER_API enum class WTokenType
{
	// Literals

	NumberLiteral,
	StringLiteral,

	// Identifiers

	Identifier,
	FunctionIdentifier,

	// Punctuations

	OPEN_PARENTHESIS,
	CLOSE_PARENTHESIS,
	OPEN_CURLY_BRACE,
	CLOSE_CURLY_BRACE,
	OPEN_BRACKET,
	CLOSE_BRACKET,

	BACKWARD_SLASH,
	COMMA,
	DOT,
	DOT_DOT,
	DOT_DOT_DOT,
	COLON,
	ARROW,

	PLUS,
	UNARY_PLUS,
	PLUS_EQUAL,

	MINUS,
	UNARY_MINUS,
	MINUS_EQUAL,

	STAR,
	STAR_EQUAL,

	DIVISION,
	DIVISION_EQUAL,

	REMINDER,
	REMINDER_EQUAL,

	POWER,
	POWER_EQUAL,

	EQUAL,
	EQUAL_EQUAL,

	BANG,
	BANG_EQUAL,

	LESSER_THAN,
	LESSER_THAN_EQUAL,

	GREATER_THAN,
	GREATER_THAN_EQUAL,

	// Keywords

	IF,
	ELSE,

	AND,
	OR,

	LET,
	CONST_KEYWORD,

	LOOP,
	FOR,
	BREAK,
	CONTINUE,
	IN_KEYWORD,

	FN,
	RETURN,

	NUM,
	STR,
	BOOL,
	ENUM,
	TYPE,
	OPT,

	TRUE_KEYWORD,
	FALSE_KEYWORD,

	IMPORT,
	FROM,

	PUB,

	// Other

	EOL,
	UNKNOWN
};

LEXER_API std::ostream& operator<<(std::ostream& os, const WTokenType& type);
