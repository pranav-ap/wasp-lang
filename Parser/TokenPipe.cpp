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

std::optional<Token_ptr> TokenPipe::lookahead() const
{
	if ((size_t)index + 1 >= tokens.size())
	{
		return nullopt;
	}

	return make_optional(tokens[(size_t)index + 1]);
}

optional<Token_ptr> TokenPipe::optional(WTokenType token_type)
{
	auto token = current();

	if (token.has_value() && token.value()->type == token_type)
	{
		advance_pointer();
		return token;
	}

	return nullopt;
}

Token_ptr TokenPipe::require(WTokenType token_type)
{
	auto token = current();
	OPT_CHECK(token);
	ASSERT(token.value()->type == token_type, "Token has an incorrect type");

	advance_pointer();
	return token.value();
}

void TokenPipe::expect(WTokenType token_type)
{
	auto token = current();
	OPT_CHECK(token);
	ASSERT(token.value()->type == token_type, "Token has an incorrect type");

	advance_pointer();
}

void TokenPipe::ignore_whitespace()
{
	while (true)
	{
		auto token = current();

		if (!token.has_value())
			break;

		if (token.value()->type != WTokenType::EOL)
			break;

		advance_pointer();
	}
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