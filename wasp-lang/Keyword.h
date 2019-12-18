#pragma once
#include <map>
#include <string>

enum KeywordType
{
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
	PURE
};

std::map<std::string, KeywordType> keyword_map = {
	{"if", IF},
	{"else", ELSE},

	{"and", AND},
	{"or", OR},

	{"let", LET},
	{"const", CONST},

	{"loop", LOOP},
	{"for", FOR},
	{"break", BREAK},
	{"continue", CONTINUE},

	{"fn", FN},
	{"return", RETURN},

	{"num", NUM},
	{"str", STR},
	{"bool", BOOL},
	{"enum", ENUM},
	{"type", TYPE},
	{"opt", OPT},

	{"true", TRUE},
	{"false", FALSE},

	{"import", IMPORT},
	{"from", FROM},

	{"pub", PUB},
	{"pure", PURE}
};
