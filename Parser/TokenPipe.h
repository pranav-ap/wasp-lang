#pragma once

#include <vector>
#include <memory>
#include <optional>
#include "Token.h"

class TokenPipe
{
	std::vector<Token_ptr> tokens;
	int index;

public:
	TokenPipe(std::vector<Token_ptr> tokens) : tokens(tokens), index(0) {};

	// Token Functions

	std::optional<Token_ptr> current() const;
	std::optional<Token_ptr> current(std::vector<TokenType> ignorables);

	std::optional<Token_ptr> optional(TokenType token_type);
	std::optional<Token_ptr> optional(TokenType token_type, std::vector<TokenType> ignorables);

	Token_ptr required(TokenType token_type);
	Token_ptr required(TokenType token_type, std::vector<TokenType> ignorables);

	void expect(TokenType token_type);
	void expect(TokenType token_type, std::vector<TokenType> ignorables);

	void expect_indent(const int expected_indent);
	bool has_indent(const int expected_indent);

	void ignore(TokenType ignorable);
	void ignore(std::vector<TokenType> ignorables);

	// Utils

	int get_current_index() const;

	void advance_pointer();
	void retreat_pointer();

	size_t get_size() const;
};

using TokenPipe_ptr = std::shared_ptr<TokenPipe>;
