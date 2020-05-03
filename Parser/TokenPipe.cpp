#pragma once
#include "pch.h"
#include "TokenPipe.h"
#include "Token.h"
#include "CommonAssertion.h"
#include <algorithm>

using std::vector;

Token_ptr TokenPipe::current() const
{
	if (index >= tokens.size()) {
		return nullptr;
	}

	return tokens[index];
}

Token_ptr TokenPipe::current(vector<WTokenType> ignorables)
{
	ignore(ignorables);
	return current();
}

Token_ptr TokenPipe::optional(WTokenType token_type)
{
	if (auto token = current(); token_type == token->type)
	{
		advance_pointer();
		return token;
	}

	return nullptr;
}

Token_ptr TokenPipe::optional(WTokenType token_type, vector<WTokenType> ignorables)
{
	ignore(ignorables);
	return optional(token_type);
}

Token_ptr TokenPipe::required(WTokenType token_type)
{
	auto token = current();
	ASSERT(token != nullptr, "Oh shit! A nullptr");
	ASSERT(token_type == token->type, "Token is incorrect type");

	advance_pointer();
	return token;
}

Token_ptr TokenPipe::required(WTokenType token_type, vector<WTokenType> ignorables)
{
	ignore(ignorables);
	return required(token_type);
}

void TokenPipe::expect(WTokenType token_type, vector<WTokenType> ignorables)
{
	ignore(ignorables);

	auto token = current();
	ASSERT(token != nullptr, "Oh shit! A nullptr");
	ASSERT(token_type == token->type, "Token is incorrect type");

	advance_pointer();
}

int TokenPipe::consume_indents()
{
	int indent_level = 0;

	while (auto token = current())
	{
		if (token->type == WTokenType::INDENT)
		{
			indent_level++;
			advance_pointer();
		}
	}

	return indent_level;
}

// Utils

void TokenPipe::ignore(vector<WTokenType> ignorables)
{
	while (auto token = current())
	{
		if (std::find(ignorables.begin(), ignorables.end(), token->type) == ignorables.end())
			break;

		advance_pointer();
	}
}

size_t TokenPipe::get_size() const
{
	return tokens.size();
}

void TokenPipe::advance_pointer()
{
	index++;
}

void TokenPipe::retreat_pointer()
{
	index--;
}