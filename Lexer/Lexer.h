#pragma once

#ifdef LEXER_EXPORTS
#define LEXER_API __declspec(dllexport)
#else
#define LEXER_API __declspec(dllimport)
#endif

#include "Token.h"
#include "TokenPosition.h"
#include "WTokenType.h"
#include "Pointer.h"

#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <map>

const std::map<std::wstring, WTokenType> keyword_map =
{
	{ L"if", WTokenType::IF },
	{ L"elif", WTokenType::ELIF },
	{ L"else", WTokenType::ELSE },

	{ L"not", WTokenType::NOT },
	{ L"and", WTokenType::AND },
	{ L"or", WTokenType::OR },
	{ L"or with", WTokenType::OR_WITH },

	{ L"no", WTokenType::NO },
	{ L"some", WTokenType::SOME },

	{ L"export", WTokenType::EXPORT },
	{ L"import", WTokenType::IMPORT },
	{ L"from", WTokenType::FROM },

	{ L"let", WTokenType::LET },
	{ L"const", WTokenType::CONST_KEYWORD },

	{ L"while", WTokenType::WHILE },
	{ L"until", WTokenType::UNTIL },
	{ L"for", WTokenType::FOR },
	{ L"in", WTokenType::IN_KEYWORD },
	{ L"break", WTokenType::BREAK },
	{ L"continue", WTokenType::CONTINUE },
	{ L"redo", WTokenType::REDO },

	{ L"fn", WTokenType::FN },
	{ L"return", WTokenType::RETURN_KEYWORD },

	{ L"infix", WTokenType::INFIX },
	{ L"prefix", WTokenType::PREFIX },
	{ L"postfix", WTokenType::POSTFIX },

	{ L"native", WTokenType::NATIVE },

	{ L"typeof", WTokenType::TYPE_OF },
	{ L"is", WTokenType::IS },
	{ L"as", WTokenType::AS },

	{ L"type", WTokenType::TYPE },
	{ L"class", WTokenType::CLASS},
	{ L"extends", WTokenType::EXTENDS },
	{ L"operator", WTokenType::OPERATOR },

	{ L"int", WTokenType::INT },
	{ L"float", WTokenType::FLOAT },
	{ L"string", WTokenType::STRING_KEYWORD },
	{ L"bool", WTokenType::BOOL },
	{ L"enum", WTokenType::ENUM },
	{ L"any", WTokenType::ANY },
	{ L"opt", WTokenType::OPT },
	{ L"none", WTokenType::NONE },

	{ L"match", WTokenType::MATCH },
	{ L"with", WTokenType::WITH },
	{ L"case", WTokenType::CASE },
	{ L"where", WTokenType::WHERE },
	{ L"end", WTokenType::END },

	{ L"assert", WTokenType::ASSERT },
	{ L"defer", WTokenType::DEFER },
	{ L"del", WTokenType::DEL },

	{ L"try", WTokenType::TRY },
	{ L"rescue", WTokenType::RESCUE },
	{ L"ensure", WTokenType::ENSURE },

	{ L"then", WTokenType::THEN },
	{ L"do", WTokenType::DO },

	{ L"testsuite", WTokenType::TESTSUITE },
	{ L"test", WTokenType::TEST },

	{ L"true", WTokenType::TRUE_KEYWORD },
	{ L"false", WTokenType::FALSE_KEYWORD }
};

class LEXER_API Lexer
{
	std::wstring raw_source;

	TokenPosition position;
	Pointer pointer;

	std::vector<Token_ptr> tokens;

	// Consumers

	Token_ptr consume_number_literal(wchar_t ch);
	Token_ptr consume_identifier(wchar_t ch);
	Token_ptr consume_string_literal();

	Token_ptr consume_plus();
	Token_ptr consume_minus();
	Token_ptr consume_star();
	Token_ptr consume_division();
	Token_ptr consume_reminder();
	Token_ptr consume_power();
	Token_ptr consume_bang();
	Token_ptr consume_equal();
	Token_ptr consume_colon();
	Token_ptr consume_dot();
	Token_ptr consume_question();
	Token_ptr consume_greater_than();
	Token_ptr consume_lesser_than();
	Token_ptr consume_single_char_punctuation(wchar_t ch);

	Token_ptr consume_eol();
	Token_ptr consume_unknown_token(wchar_t ch);

	// Utils

	wchar_t get_char_at(int index) const;
	wchar_t get_current_char() const;
	wchar_t get_right_char() const;

	bool expect_current_char(wchar_t ch);
	void next();
	void previous();

	void init(std::wstring raw_source);
public:
	std::vector<Token_ptr> run(std::wstring raw_source);
};

using Lexer_ptr = LEXER_API std::unique_ptr<Lexer>;
