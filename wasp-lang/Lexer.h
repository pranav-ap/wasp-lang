#pragma once

#include "Token.h"
#include "TokenPosition.h"
#include "TokenType.h"
#include "Pointer.h"
#include "utils.h"

#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <map>

const std::map<STRING, WTokenType> keyword_map =
{
	{ L"if", WTokenType::IF },
	{ L"elif", WTokenType::ELIF },
	{ L"else", WTokenType::ELSE },

	{ L"and", WTokenType::AND },
	{ L"or", WTokenType::OR },

	{ L"let", WTokenType::LET },
	{ L"const", WTokenType::CONST_KEYWORD },

	{ L"while", WTokenType::WHILE },
	{ L"for", WTokenType::FOR },
	{ L"in", WTokenType::IN_KEYWORD },
	{ L"break", WTokenType::BREAK },
	{ L"continue", WTokenType::CONTINUE },

	{ L"fn", WTokenType::FN },
	{ L"return", WTokenType::RETURN },
	{ L"gen", WTokenType::GEN },
	{ L"yield", WTokenType::YIELD },

	{ L"type", WTokenType::TYPE },
	{ L"num", WTokenType::NUM },
	{ L"str", WTokenType::STR },
	{ L"bool", WTokenType::BOOL },
	{ L"enum", WTokenType::ENUM },
	{ L"any", WTokenType::ANY },
	{ L"opt", WTokenType::OPT },

	{ L"none", WTokenType::NONE },

	{ L"true", WTokenType::TRUE_KEYWORD },
	{ L"false", WTokenType::FALSE_KEYWORD },

	{ L"import", WTokenType::IMPORT },
	{ L"from", WTokenType::FROM },

	{ L"new", WTokenType::NEW },

	{ L"pub", WTokenType::PUB },
	{ L"pass", WTokenType::PASS },

	{ L"assert", WTokenType::ASSERT }
};

class Lexer
{
	STRING raw_source;

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
	Token_ptr consume_greater_than();
	Token_ptr consume_lesser_than();
	Token_ptr consume_single_char_punctuation(wchar_t ch);

	Token_ptr consume_eol();
	Token_ptr consume_space();

	Token_ptr consume_unknown_token(wchar_t ch);

	// Utils

	wchar_t get_char_at(int index) const;
	wchar_t get_current_char() const;
	wchar_t get_right_char() const;

	std::optional<Token_ptr> get_previous_significant_token();
	bool is_unary();

	bool expect_current_char(wchar_t ch);
	void next();
	void previous();

	void init(STRING raw_source);
public:
	Lexer() {};
	std::vector<Token_ptr> execute(STRING raw_source);
};
