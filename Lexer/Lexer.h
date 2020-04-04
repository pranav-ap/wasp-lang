#pragma once

#ifdef LEXER_EXPORTS
#define LEXER_API __declspec(dllexport)
#else
#define LEXER_API __declspec(dllimport)
#endif

#include "logger.h"
#include "Token.h"
#include "TokenPosition.h"
#include "Pointer.h"

#include <string>
#include <vector>
#include <memory>

#define MAKE_TOKEN(type, token, line_num, col_num) std::make_shared<Token>(type, token, line_num, col_num)
#define CASE_BODY(call) { token = call; break; }
#define NEXT this->pointer.advance(); this->position.increment_column_number();
#define LINE_NUM this->position.get_line_num()
#define COL_NUM this->position.get_column_num()

class LEXER_API Lexer
{
	std::string raw_source;

	TokenPosition position;
	Pointer pointer;

	std::vector<Token_ptr> tokens;

	// Utils

	char get_char_at(int index) const;
	char get_current_char() const;
	char get_right_char() const;

	bool is_unary() const;

	bool expect_current_char(char ch);

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

	Token_ptr consume_single_char_punctuation(char ch);
	Token_ptr consume_eol();
	Token_ptr consume_unknown_token(char ch);

public:
	Lexer(std::string raw_source) : raw_source(raw_source), position(TokenPosition()), pointer(Pointer()) {};
	std::vector<Token_ptr> execute();
};
