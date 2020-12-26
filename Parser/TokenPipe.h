#pragma once

#include "Token.h"
#include <vector>
#include <memory>
#include <optional>
 
class TokenPipe
{
	std::vector<Token_ptr> tokens;
	int index;

public:
	TokenPipe(std::vector<Token_ptr> tokens) : tokens(tokens), index(0) {};

	std::optional<Token_ptr> current() const;
	std::optional<Token_ptr> lookahead() const;

	std::optional<Token_ptr> optional(WTokenType token_type);
	Token_ptr require(WTokenType token_type);

	int ignore_whitespace();

	// Utils

	int get_current_index() const;

	void advance_pointer();
	void retreat_pointer();

	size_t get_size() const;
};

using TokenPipe_ptr = std::shared_ptr<TokenPipe>;
