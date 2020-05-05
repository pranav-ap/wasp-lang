#pragma once

#ifdef LEXER_EXPORTS
#define LEXER_API __declspec(dllexport)
#else
#define LEXER_API __declspec(dllimport)
#endif

#include "Token.h"
#include "TokenPosition.h"
#include "TokenType.h"
#include "Pointer.h"

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <regex>

const std::map<std::string, WTokenType> keyword_map = {
	{ "if", WTokenType::IF },
	{ "elif", WTokenType::ELIF },
	{ "else", WTokenType::ELSE },

	{ "and", WTokenType::AND },
	{ "or", WTokenType::OR },

	{ "let", WTokenType::LET },
	{ "const", WTokenType::CONST_KEYWORD },

	{ "while", WTokenType::WHILE },
	{ "for", WTokenType::FOR },
	{ "in", WTokenType::IN_KEYWORD },
	{ "break", WTokenType::BREAK },
	{ "continue", WTokenType::CONTINUE },

	{ "fn", WTokenType::FN },
	{ "return", WTokenType::RETURN },
	{ "gen", WTokenType::GEN },
	{ "yield", WTokenType::YIELD },

	{ "type", WTokenType::TYPE },
	{ "num", WTokenType::NUM },
	{ "str", WTokenType::STR },
	{ "bool", WTokenType::BOOL },
	{ "enum", WTokenType::ENUM },
	{ "opt", WTokenType::OPT },

	{ "none", WTokenType::NONE },

	{ "true", WTokenType::TRUE_KEYWORD },
	{ "false", WTokenType::FALSE_KEYWORD },

	{ "import", WTokenType::IMPORT },
	{ "from", WTokenType::FROM },

	{ "pub", WTokenType::PUB },
	{ "pass", WTokenType::PASS }
};

class LEXER_API Lexer
{
	std::string raw_source;

	TokenPosition position;
	Pointer pointer;

	std::vector<Token_ptr> tokens;

	// Consumers

	Token_ptr consume_number_literal(char ch);
	Token_ptr consume_string_literal();

	Token_ptr consume_identifier(char ch);

	Token_ptr consume_plus();
	Token_ptr consume_minus();
	Token_ptr consume_star();
	Token_ptr consume_division();
	Token_ptr consume_reminder();
	Token_ptr consume_power();
	Token_ptr consume_bang();
	Token_ptr consume_equal();
	Token_ptr consume_greater_than();
	Token_ptr consume_lesser_than();
	Token_ptr consume_dot();
	Token_ptr consume_colon();
	Token_ptr consume_open_parenthesis();
	Token_ptr consume_single_char_punctuation(char ch);

	Token_ptr consume_eol();
	Token_ptr consume_space();

	Token_ptr consume_unknown_token(char ch);

	// Utils

	char get_char_at(int index) const;
	char get_current_char() const;
	char get_right_char() const;

	Token_ptr get_previous_significant_token();
	bool is_unary();

	bool expect_current_char(char ch);

public:
	Lexer(std::string raw_source) : position(TokenPosition()), pointer(Pointer())
	{
		this->raw_source = std::regex_replace(raw_source, std::regex("\t"), "    ");
	};

	std::vector<Token_ptr> execute();
};
