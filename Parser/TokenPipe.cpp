#pragma once
#include "pch.h"
#include "TokenPipe.h"

Token_ptr TokenPipe::get_current_token() const
{
	if (index >= tokens.size()) {
		return nullptr;
	}

	return tokens[index];
}

bool TokenPipe::expect_current_token_to_be(WTokenType token_type)
{
	auto token = get_current_token();

	if (token != nullptr && token_type == token->type)
	{
		advance_pointer();
		return true;
	}

	return false;
}

Token_ptr TokenPipe::consume_token(WTokenType token_type)
{
	auto token = get_current_token();

	if (token != nullptr && token_type == token->type)
	{
		advance_pointer();
		return token;
	}

	return nullptr;
}

void TokenPipe::ignore(WTokenType token_type)
{
	while (expect_current_token_to_be(token_type))
	{
	}
}

size_t TokenPipe::get_size() const
{
	return tokens.size();
}

int TokenPipe::get_pointer_index() const
{
	return index;
}

void TokenPipe::advance_pointer()
{
	index++;
}

void TokenPipe::retreat_pointer()
{
	index--;
}