#pragma once

#ifdef PARSER_EXPORTS
#define PARSER_API __declspec(dllexport)
#else
#define PARSER_API __declspec(dllimport)
#endif

#include <vector>
#include <memory>

#include "Token.h"

#define ADVANCE_PTR token_pipe->advance_pointer()
#define RETREAT_PTR token_pipe->retreat_pointer()

class PARSER_API TokenPipe
{
	std::vector<Token_ptr> tokens;
	int index;

public:
	TokenPipe(std::vector<Token_ptr>& tokens) : tokens(std::move(tokens)), index(0) {};

	// Pointer Functions

	int get_pointer_index() const;
	void advance_pointer();
	void retreat_pointer();

	// Token Functions

	Token_ptr get_current_token() const;
	Token_ptr consume_token(WTokenType token_type);
	bool expect_current_token(WTokenType token_type);
	void ignore(WTokenType token_type);
	size_t get_size() const;
};

using TokenPipe_ptr = PARSER_API std::shared_ptr<TokenPipe>;
