#pragma once
#include "pch.h"
#include "Lexer.h"
#include <vector>
#include <algorithm>

bool Lexer::expect_current_char(char ch)
{
	if (ch == get_current_char())
	{
		pointer.advance();
		return true;
	}

	return false;
}

char Lexer::get_char_at(int index) const
{
	if ((size_t)index >= raw_source.size() || index < 0)
		return NULL;

	return raw_source[index];
}

char Lexer::get_current_char() const
{
	int index = pointer.get_index();
	return get_char_at(index);
}

char Lexer::get_right_char() const
{
	int index = pointer.get_index();
	return get_char_at(index + 1);
}

bool Lexer::is_unary() const
{
	Token_ptr previous_token = nullptr;

	for (auto t = tokens.rbegin(); t != tokens.rend(); t++)
	{
		auto token = *t;

		if (token->type != WTokenType::EOL)
		{
			previous_token = token;
			break;
		}
	}

	if (previous_token == nullptr)
		return true;

	auto previous_token_type = previous_token->type;

	switch (previous_token_type)
	{
	case WTokenType::NumberLiteral:
	case WTokenType::Identifier:
	case WTokenType::FunctionIdentifier:
	{
		return false;
	}

	default:
	{
		return true;
	}
	}
}