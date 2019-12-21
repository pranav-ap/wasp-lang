#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Token.h"
#include "Position.h"

class Lexer
{
	std::string raw_source;
	Position pos;
	int index;

	void advance();
	char get_char_at(int index);
	char get_current_char();
	char get_right_char();

	bool peek_and_move(char ch);

	// Consumers

	std::unique_ptr<NumberLiteral> consume_number_literal(char ch);
	std::unique_ptr<StringLiteral> consume_string_literal();
	std::unique_ptr<Identifier> consume_identifier(char ch);

	std::unique_ptr<Punctuation> handle_plus();
	std::unique_ptr<Punctuation> handle_minus();
	std::unique_ptr<Punctuation> handle_star();
	std::unique_ptr<Punctuation> handle_division();
	std::unique_ptr<Punctuation> handle_reminder();
	std::unique_ptr<Punctuation> handle_power();
	std::unique_ptr<Punctuation> handle_bang();
	std::unique_ptr<Punctuation> handle_equal();
	std::unique_ptr<Punctuation> handle_greater_than();
	std::unique_ptr<Punctuation> handle_lesser_than();
	std::unique_ptr<Punctuation> consume_single_char_punctuation(char ch);

	std::unique_ptr<Unknown> consume_unknown_token(char ch);

public:
	Lexer(std::string raw_source) : raw_source(raw_source), pos(Position()), index(0) {};
	std::vector<std::unique_ptr<TokenType>> execute();
};
