#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "Token.h"
#include "TokenPosition.h"
#include "Pointer.h"

#define MAKE_TOKEN(a, b, c, d) make_shared<Token>(a, b, c, d)
#define CASE_BODY(call) { token = call; break; }
#define NEXT this->pointer.advance(); this->position.increment_column_number();
#define LINE_NUM this->position.get_line_num()
#define COL_NUM this->position.get_column_num()

using std::map;
using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;

typedef shared_ptr<Token> Token_ptr;

class Lexer
{
	std::string raw_source;

	TokenPosition position;
	Pointer pointer;

	char get_char_at(int index);
	char get_current_char();
	char get_right_char();

	bool expect_current_char(char ch);
	bool followed_by(char ch);

	// Consumers

	std::shared_ptr<Token> consume_number_literal(char ch);
	std::shared_ptr<Token> consume_string_literal();

	std::shared_ptr<Token> consume_identifier(char ch);

	std::shared_ptr<Token> handle_plus();
	std::shared_ptr<Token> handle_minus();
	std::shared_ptr<Token> handle_star();
	std::shared_ptr<Token> handle_division();
	std::shared_ptr<Token> handle_reminder();
	std::shared_ptr<Token> handle_power();
	std::shared_ptr<Token> handle_bang();
	std::shared_ptr<Token> handle_equal();
	std::shared_ptr<Token> handle_greater_than();
	std::shared_ptr<Token> handle_lesser_than();
	std::shared_ptr<Token> handle_dot();
	std::shared_ptr<Token> consume_single_char_punctuation(char ch);

	std::shared_ptr<Token> consume_eol();
	std::shared_ptr<Token> consume_unknown_token(char ch);

public:
	Lexer(std::string raw_source) : raw_source(raw_source), position(TokenPosition()), pointer(Pointer()) {};
	std::vector<std::shared_ptr<Token>> execute();
};
