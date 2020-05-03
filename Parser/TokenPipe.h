#pragma once

#include <vector>
#include <memory>
#include "Token.h"

class TokenPipe
{
	std::vector<Token_ptr> tokens;
	int index;

	Token_ptr optional(WTokenType token_type);
	Token_ptr required(WTokenType token_type);

	void ignore(std::vector<WTokenType> token_types);

public:
	TokenPipe(std::vector<Token_ptr>& tokens) : tokens(std::move(tokens)), index(0) {};

	// Pointer Functions

	void advance_pointer();
	void retreat_pointer();

	// Token Functions

	Token_ptr current() const;
	Token_ptr current(std::vector<WTokenType> ignorables);

	Token_ptr optional(WTokenType token_type, std::vector<WTokenType> ignorables);
	Token_ptr required(WTokenType token_type, std::vector<WTokenType> ignorables);

	void expect(WTokenType token_type, std::vector<WTokenType> ignorables);

	int consume_indents();
	size_t get_size() const;
};

using TokenPipe_ptr = std::shared_ptr<TokenPipe>;
