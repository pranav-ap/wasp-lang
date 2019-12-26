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

	std::unique_ptr<Token> consume_number_literal(char ch);
	std::unique_ptr<Token> consume_string_literal();

	std::unique_ptr<Token> consume_identifier(char ch);

	std::unique_ptr<Token> handle_plus();
	std::unique_ptr<Token> handle_minus();
	std::unique_ptr<Token> handle_star();
	std::unique_ptr<Token> handle_division();
	std::unique_ptr<Token> handle_reminder();
	std::unique_ptr<Token> handle_power();
	std::unique_ptr<Token> handle_bang();
	std::unique_ptr<Token> handle_equal();
	std::unique_ptr<Token> handle_greater_than();
	std::unique_ptr<Token> handle_lesser_than();
	std::unique_ptr<Token> consume_single_char_punctuation(char ch);

	std::unique_ptr<Token> consume_eol();
	std::unique_ptr<Token> consume_unknown_token(char ch);

public:
	Lexer(std::string raw_source) : raw_source(raw_source), position(TokenPosition()), pointer(Pointer()) {};
	std::vector<std::unique_ptr<Token>> execute();
};
