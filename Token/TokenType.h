#pragma once

#ifdef TOKEN_EXPORTS
#define TOKEN_API __declspec(dllexport)
#else
#define TOKEN_API __declspec(dllimport)
#endif

enum class TOKEN_API WTokenType
{
	// Literals

	NumberLiteral,
	StringLiteral,

	// Identifiers

	Identifier,
	CallableIdentifier,

	// Punctuations

	OPEN_PARENTHESIS,
	CLOSE_PARENTHESIS,
	OPEN_CURLY_BRACE,
	CLOSE_CURLY_BRACE,
	OPEN_SQUARE_BRACKET,
	CLOSE_SQUARE_BRACKET,
	OPEN_TUPLE_PARENTHESIS,

	BACKWARD_SLASH,
	ARROW,
	COMMA,
	DOT,
	DOT_DOT,
	DOT_DOT_DOT,
	COLON,
	COLON_COLON,
	BAR,

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
	ELIF,
	ELSE,

	AND,
	OR,

	LET,
	CONST_KEYWORD,

	WHILE,
	FOR,
	BREAK,
	CONTINUE,
	IN_KEYWORD,

	FN,
	RETURN,
	GEN,
	YIELD,

	TYPE,
	NUM,
	STR,
	BOOL,
	ENUM,
	OPT,

	NONE,

	TRUE_KEYWORD,
	FALSE_KEYWORD,

	IMPORT,
	FROM,

	PUB,
	PASS,

	// Whitespace

	EOL,
	SPACE,

	// Other

	UNKNOWN
};
