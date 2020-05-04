#pragma once

#include <vector>
#include <memory>
#include "Token.h"

class TokenPipe
{
	std::vector<Token_ptr> tokens;
	int index;

public:
	TokenPipe(std::vector<Token_ptr> tokens) : tokens(tokens), index(0) {};

	// Pointer Functions

	int get_pointer_index() const;
	void pointer_skip(int index);
	void advance_pointer();
	void retreat_pointer();

	// Token Functions

	Token_ptr token_at(int i) const;

	Token_ptr current() const;
	Token_ptr current(std::vector<WTokenType> ignorables);

	Token_ptr optional(WTokenType token_type);
	Token_ptr optional(WTokenType token_type, std::vector<WTokenType> ignorables);

	Token_ptr required(WTokenType token_type);
	Token_ptr required(WTokenType token_type, std::vector<WTokenType> ignorables);

	void ignore(WTokenType ignorable);
	void ignore(std::vector<WTokenType> ignorables);

	void expect(WTokenType token_type);
	void expect(WTokenType token_type, std::vector<WTokenType> ignorables);

	int consume_spaces();
	void skip_empty_lines();
	size_t get_size() const;
};

using TokenPipe_ptr = std::shared_ptr<TokenPipe>;
