#pragma once

#ifdef LEXER_EXPORTS
#define LEXER_API __declspec(dllexport)
#else
#define LEXER_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <memory>
#include "Token.h"
#include "TokenPosition.h"
#include "Pointer.h"

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

	char get_char_at(int index) const;
	char get_current_char() const;
	char get_right_char() const;

	bool expect_current_char(char ch);

	// Consumers

	Token_ptr consume_number_literal(char ch);
	Token_ptr consume_string_literal();

	Token_ptr consume_identifier(char ch);

	Token_ptr handle_plus();
	Token_ptr handle_minus();
	Token_ptr handle_star();
	Token_ptr handle_division();
	Token_ptr handle_reminder();
	Token_ptr handle_power();
	Token_ptr handle_bang();
	Token_ptr handle_equal();
	Token_ptr handle_greater_than();
	Token_ptr handle_lesser_than();
	Token_ptr handle_dot();

	Token_ptr consume_single_char_punctuation(char ch);
	Token_ptr consume_eol();
	Token_ptr consume_unknown_token(char ch);

public:
	Lexer(std::string raw_source) : raw_source(raw_source), position(TokenPosition()), pointer(Pointer()) {};
	std::vector<Token_ptr> execute();
};
