#pragma once

#include <vector>
#include <memory>

#include "Token.h"

#define ADVANCE_PTR token_pipe->advance_pointer()
#define RETREAT_PTR token_pipe->retreat_pointer()

class TokenPipe
{
	std::vector<Token_ptr> tokens;
	int index;

public:
	TokenPipe(std::vector<Token_ptr>& tokens)
		: tokens(std::move(tokens)), index(0) {};

	// Pointer Functions

	int get_pointer_index() const;
	void advance_pointer();
	void retreat_pointer();

	// Token Functions

	Token_ptr consume_current_token() const;
	Token_ptr consume_significant_token();

	Token_ptr consume_optional_token(WTokenType token_type);
	Token_ptr consume_required_token(WTokenType token_type);

	bool next_significant_token_is(WTokenType token_type);

	void ignore(WTokenType token_type);
	size_t get_size() const;
};

using TokenPipe_ptr = std::shared_ptr<TokenPipe>;
