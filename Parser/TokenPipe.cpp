#pragma once
#include "pch.h"
#include "TokenPipe.h"
#include "CommonAssertion.h"

Token_ptr TokenPipe::consume_current_token() const
{
	if (index >= tokens.size()) {
		return nullptr;
	}

	return tokens[index];
}

Token_ptr TokenPipe::consume_significant_token()
{
	ignore(WTokenType::EOL);
	auto token = consume_current_token();

	ASSERT(token != nullptr, "Token == nullptr");

	//advance_pointer();
	return token;
}

Token_ptr TokenPipe::consume_optional_token(WTokenType token_type)
{
	auto token = consume_significant_token();

	if (token_type == token->type)
	{
		advance_pointer();
		return token;
	}

	return nullptr;
}

Token_ptr TokenPipe::consume_required_token(WTokenType token_type)
{
	auto token = consume_significant_token();
	ASSERT(token_type == token->type, "Token is incorrect type");

	advance_pointer();
	return token;
}

bool TokenPipe::next_significant_token_is(WTokenType token_type)
{
	auto token = consume_significant_token();

	if (token_type == token->type)
	{
		advance_pointer();
		return true;
	}

	return false;
}

void TokenPipe::ignore(WTokenType token_type)
{
	while (true)
	{
		auto token = consume_current_token();

		if (token)
		{
			if (token->type != token_type)
			{
				break;
			}

			advance_pointer();
		}
		else
		{
			break;
		}
	}
}

// Utils

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