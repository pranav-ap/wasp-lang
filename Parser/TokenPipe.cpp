#pragma once
#include "pch.h"
#include "TokenPipe.h"
#include "Token.h"
#include "CommonAssertion.h"
#include <algorithm>

using std::vector;

Token_ptr TokenPipe::token_at(int i) const
{
	if (i >= tokens.size()) {
		return nullptr;
	}

	return tokens[i];
}

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

void TokenPipe::expect(WTokenType token_type)
{
	auto token = current();
	ASSERT(token != nullptr, "Oh shit! A nullptr");
	ASSERT(token_type == token->type, "Token is incorrect type");

	advance_pointer();
}

void TokenPipe::expect(WTokenType token_type, vector<WTokenType> ignorables)
{
	ignore(ignorables);
	expect(token_type);
}

int TokenPipe::consume_spaces()
{
	int space_count = 0;

	while (auto token = current())
	{
		if (token->type == WTokenType::SPACE)
		{
			space_count++;
			advance_pointer();
			continue;
		}

		break;
	}

	return space_count;
}

void TokenPipe::skip_empty_lines()
{
	while (true)
	{
		ignore({ WTokenType::EOL });

		int space_count = 0;
		int token_index = get_pointer_index();

		while (auto token = token_at(token_index))
		{
			if (token->type == WTokenType::SPACE)
			{
				space_count++;
				token_index++;
			}
			else if (token->type == WTokenType::EOL)
			{
				pointer_skip(space_count + 1);
			}
			else
			{
				break;
			}
		}

		break;
	}
}

// Ignore

void TokenPipe::ignore(WTokenType ignorable)
{
	while (auto token = current())
	{
		if (token->type != ignorable)
			break;

		advance_pointer();
	}
}

void TokenPipe::ignore(vector<WTokenType> ignorables)
{
	while (auto token = current())
	{
		if (std::find(ignorables.begin(), ignorables.end(), token->type) == ignorables.end())
			break;

		advance_pointer();
	}
}

// UTILS

size_t TokenPipe::get_size() const
{
	return tokens.size();
}

int TokenPipe::get_pointer_index() const
{
	return index;
}

void TokenPipe::pointer_skip(int skip)
{
	index = get_pointer_index() + skip;
}

void TokenPipe::advance_pointer()
{
	index++;
}

void TokenPipe::retreat_pointer()
{
	index--;
}