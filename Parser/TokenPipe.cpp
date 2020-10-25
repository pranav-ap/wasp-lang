#pragma once
#include "pch.h"
#include "TokenPipe.h"
#include "Token.h"
#include "Assertion.h"
#include <algorithm>
#include <optional>

#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")

using std::vector;
using std::nullopt;
using std::make_optional;
using std::optional;

optional<Token_ptr> TokenPipe::current() const
{
	if (index >= tokens.size())
	{
		return nullopt;
	}

	return make_optional(tokens[index]);
}

optional<Token_ptr> TokenPipe::lookahead() const
{
	if ((size_t)index + 1 >= tokens.size())
	{
		return nullopt;
	}

	return make_optional(tokens[(size_t)index + 1]);
}

optional<Token_ptr> TokenPipe::optional(WTokenType token_type)
{
	int ignored_tokens_number = 0;

	if (token_type != WTokenType::EOL)
	{
		ignored_tokens_number = ignore_whitespace();
	}

	auto token = current();

	if (token.has_value() && token.value()->type == token_type)
	{
		advance_pointer();
		return token;
	}

	if (token_type != WTokenType::EOL)
	{
		index -= ignored_tokens_number;
	}

	return nullopt;
}

Token_ptr TokenPipe::require(WTokenType token_type)
{
	if (token_type != WTokenType::EOL)
	{
		ignore_whitespace();
	}

	auto token = current();
	OPT_CHECK(token);
	ASSERT(token.value()->type == token_type, "Token has an incorrect type");

	advance_pointer();
	return token.value();
}

int TokenPipe::ignore_whitespace()
{
	int ignored_tokens_number = 0;

	while (true)
	{
		auto token = current();

		if (!token.has_value())
			break;

		if (token.value()->type != WTokenType::EOL)
			break;

		ignored_tokens_number++;
		advance_pointer();
	}

	return ignored_tokens_number;
}

// UTILS

int TokenPipe::get_current_index() const
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

size_t TokenPipe::get_size() const
{
	return tokens.size();
}