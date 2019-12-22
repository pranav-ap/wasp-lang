#pragma once
#include <string>

enum class TokenType
{
	// Literals

	NumberLiteral,
	StringLiteral,
	BooleanLiteral,

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
	COLON,
	BAR,
	ARROW,

	PLUS,
	PLUS_EQUAL,

	MINUS,
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
	CONST,

	LOOP,
	FOR,
	BREAK,
	CONTINUE,

	FN,
	RETURN,

	NUM,
	STR,
	BOOL,
	ENUM,
	TYPE,
	OPT,

	TRUE,
	FALSE,

	IMPORT,
	FROM,

	PUB,
	PURE,

	// Other

	EOL,
	UNKNOWN
};

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
