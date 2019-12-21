#pragma once
#include <map>
#include <string>

enum class KeywordType
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
	{"if", KeywordType::IF},
	{"else", KeywordType::ELSE},

	{"and", KeywordType::AND},
	{"or", KeywordType::OR},

	{"let", KeywordType::LET},
	{"const", KeywordType::CONST},

	{"loop", KeywordType::LOOP},
	{"for", KeywordType::FOR},
	{"break", KeywordType::BREAK},
	{"continue", KeywordType::CONTINUE},

	{"fn", KeywordType::FN},
	{"return", KeywordType::RETURN},

	{"num", KeywordType::NUM},
	{"str", KeywordType::STR},
	{"bool", KeywordType::BOOL},
	{"enum", KeywordType::ENUM},
	{"type", KeywordType::TYPE},
	{"opt", KeywordType::OPT},

	{"true", KeywordType::TRUE},
	{"false", KeywordType::FALSE},

	{"import", KeywordType::IMPORT},
	{"from", KeywordType::FROM},

	{"pub", KeywordType::PUB},
	{"pure", KeywordType::PURE}
};
