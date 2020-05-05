#pragma once
#include "pch.h"
#include "TokenPipe.h"
#include "Token.h"
#include "CommonAssertion.h"
#include <algorithm>
#include <optional>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")

using std::vector;
using std::nullopt;
using std::make_optional;
using std::optional;

TokenPipe::TokenPipe(std::vector<Token_ptr> tokens)
{
}

optional<Token_ptr> TokenPipe::token_at(int position) const
{
	if (position >= tokens.size()) {
		return nullopt;
	}

	return make_optional(tokens[position]);
}

optional<Token_ptr> TokenPipe::current() const
{
	if (index >= tokens.size()) {
		return nullopt;
	}

	return make_optional(tokens[index]);
}

optional<Token_ptr> TokenPipe::current(vector<WTokenType> ignorables)
{
	ignore(ignorables);

	auto token = current();
	return token;
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

optional<Token_ptr> TokenPipe::optional(WTokenType token_type, vector<WTokenType> ignorables)
{
	ignore(ignorables);

	auto token = optional(token_type);
	return token;
}

Token_ptr TokenPipe::required(WTokenType token_type)
{
	auto token = current();
	ASSERT(token.has_value(), "Token does not have any value");
	ASSERT(token.value()->type == token_type, "Token has an incorrect type");

	advance_pointer();
	return token.value();
}

Token_ptr TokenPipe::required(WTokenType token_type, vector<WTokenType> ignorables)
{
	ignore(ignorables);

	auto token = required(token_type);
	return token;
}

void TokenPipe::expect(WTokenType token_type)
{
	auto token = current();
	ASSERT(token.has_value(), "Token does not have any value");
	ASSERT(token.value()->type == token_type, "Token has an incorrect type");

	advance_pointer();
}

void TokenPipe::expect(WTokenType token_type, vector<WTokenType> ignorables)
{
	ignore(ignorables);
	expect(token_type);
}

void TokenPipe::expect_indent(const int expected_indent)
{
	int count = 0;

	while (count <= expected_indent)
	{
		expect(WTokenType::SPACE);
		count++;
		advance_pointer();
	}

	auto token = current();

	if (token.has_value())
	{
		ASSERT(token.value()->type != WTokenType::SPACE, "Incorrect Indentation");
	}
}

int TokenPipe::count_spaces()
{
	int space_count = 0;

	while (true)
	{
		auto token = current();

		if (token.has_value() && token.value()->type == WTokenType::SPACE)
		{
			space_count++;
			continue;
		}

		break;
	}

	return space_count;
}

// Ignore

void TokenPipe::ignore(WTokenType ignorable)
{
	while (true)
	{
		auto token = current();

		if (!token.has_value())
			break;

		if (token.value()->type != ignorable)
			break;

		advance_pointer();
	}
}

void TokenPipe::ignore(vector<WTokenType> ignorables)
{
	while (true)
	{
		auto token = current();

		if (!token.has_value())
			break;

		if (std::find(ignorables.begin(), ignorables.end(), token.value()->type) == ignorables.end())
			break;

		advance_pointer();
	}
}

// UTILS

size_t TokenPipe::get_size() const
{
	return tokens.size();
}

int TokenPipe::get_current_index() const
{
	return index;
}

void TokenPipe::advance_pointer(int steps)
{
	index = get_current_index() + steps;
}

void TokenPipe::advance_pointer()
{
	index++;
}

void TokenPipe::retreat_pointer()
{
	index--;
}