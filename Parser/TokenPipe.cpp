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

// Current

optional<Token_ptr> TokenPipe::current() const
{
	if (index >= tokens.size())
	{
		return nullopt;
	}

	return make_optional(tokens[index]);
}

optional<Token_ptr> TokenPipe::current(vector<TokenType> ignorables)
{
	ignore(ignorables);

	auto token = current();
	return token;
}

// Optional

optional<Token_ptr> TokenPipe::optional(TokenType token_type)
{
	auto token = current();

	if (token.has_value() && token.value()->type == token_type)
	{
		advance_pointer();
		return token;
	}

	return nullopt;
}

optional<Token_ptr> TokenPipe::optional(TokenType token_type, vector<TokenType> ignorables)
{
	ignore(ignorables);

	auto token = optional(token_type);
	return token;
}

// Required

Token_ptr TokenPipe::required(TokenType token_type)
{
	auto token = current();
	OPT_CHECK(token);
	ASSERT(token.value()->type == token_type, "Token has an incorrect type");

	advance_pointer();
	return token.value();
}

Token_ptr TokenPipe::required(TokenType token_type, vector<TokenType> ignorables)
{
	ignore(ignorables);

	auto token = required(token_type);
	return token;
}

// Expect

void TokenPipe::expect(TokenType token_type)
{
	auto token = current();
	OPT_CHECK(token);
	ASSERT(token.value()->type == token_type, "Token has an incorrect type");

	advance_pointer();
}

void TokenPipe::expect(TokenType token_type, vector<TokenType> ignorables)
{
	ignore(ignorables);
	expect(token_type);
}

void TokenPipe::expect_indent(const int expected_indent)
{
	int count = 0;

	while (count < expected_indent)
	{
		expect(TokenType::SPACE);
		count++;
	}

	auto token = current();
	OPT_CHECK(token);
	ASSERT(token.value()->type != TokenType::SPACE, "Incorrect Indentation");
}

bool TokenPipe::has_indent(const int expected_indent)
{
	int count = 0;

	while (count <= expected_indent)
	{
		auto token = current();

		if (token.has_value())
		{
			if (token.value()->type == TokenType::SPACE)
			{
				advance_pointer();
				count++;
			}
			else
			{
				ASSERT(token.value()->type != TokenType::SPACE, "Incorrect Indentation");
				break;
			}
		}
		else
		{
			break;
		}
	}

	if (count == expected_indent)
	{
		return true;
	}

	while (count > 0)
	{
		retreat_pointer();
		count--;
	}

	return false;
}

// Ignore

void TokenPipe::ignore(TokenType ignorable)
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

void TokenPipe::ignore(vector<TokenType> ignorables)
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