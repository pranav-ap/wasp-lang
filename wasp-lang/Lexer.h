#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Token.h"
#include "TokenPosition.h"
#include "Pointer.h"

class Lexer
{
	std::string raw_source;

	TokenPosition position;
	Pointer pointer;

	char get_char_at(int index);
	char get_current_char();
	char get_right_char();

	bool peek_and_move(char ch);

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
	std::shared_ptr<Token> consume_single_char_punctuation(char ch);

	std::shared_ptr<Token> consume_eol();
	std::shared_ptr<Token> consume_unknown_token(char ch);

public:
	Lexer(std::string raw_source) : raw_source(raw_source), position(TokenPosition()), pointer(Pointer()) {};
	std::vector<std::shared_ptr<Token>> execute();
};
