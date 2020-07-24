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

const std::map<std::string, WTokenType> keyword_map =
{
	{ "if", WTokenType::IF },
	{ "elif", WTokenType::ELIF },
	{ "else", WTokenType::ELSE },

	{ "and", WTokenType::AND },
	{ "or", WTokenType::OR },

	{ "let", WTokenType::LET },
	{ "const", WTokenType::CONST },

	{ "while", WTokenType::WHILE },
	{ "for", WTokenType::FOR },
	{ "in", WTokenType::IN },
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
	{ "any", WTokenType::ANY },
	{ "opt", WTokenType::OPT },

	{ "none", WTokenType::NONE },

	{ "true", WTokenType::TRUE },
	{ "false", WTokenType::FALSE },

	{ "import", WTokenType::IMPORT },
	{ "from", WTokenType::FROM },

	{ "new", WTokenType::NEW },

	{ "pub", WTokenType::PUB },
	{ "pass", WTokenType::PASS },

	{ "assert", WTokenType::ASSERT }
};

class Lexer
{
	std::string raw_source;

	TokenPosition position;
	Pointer pointer;

	std::vector<Token_ptr> tokens;

	// Consumers

	Token_ptr consume_number_literal(DECIMAL_CODE_POINT decimal_code_point);
	Token_ptr consume_identifier(DECIMAL_CODE_POINT decimal_code_point);
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
	Token_ptr consume_single_char_punctuation(DECIMAL_CODE_POINT decimal_code_point);

	Token_ptr consume_eol();
	Token_ptr consume_space();

	Token_ptr consume_unknown_token(DECIMAL_CODE_POINT decimal_code_point);

	// Utils

	DECIMAL_CODE_POINT get_char_at(int index) const;
	DECIMAL_CODE_POINT get_current_char() const;
	DECIMAL_CODE_POINT get_right_char() const;

	std::optional<Token_ptr> get_previous_significant_token();
	bool is_unary();

	bool expect_current_char(DECIMAL_CODE_POINT decimal_code_point);
	void next();
	void previous();

	void init(std::string raw_source);
public:
	Lexer() {};
	std::vector<Token_ptr> execute(std::string raw_source);
};
