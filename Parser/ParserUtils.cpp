#pragma once
#include "pch.h"
#include "Parser.h"

Token_ptr Parser::get_current_token() const
{
	size_t index = (size_t)this->pointer.get_index();
	RETURN_NULLPTR_IF_TRUE(index >= this->tokens.size());
	return this->tokens[index];
}

bool Parser::expect_current_token(WTokenType token_type)
{
	auto token = this->get_current_token();

	if (token != nullptr && token_type == token->type)
	{
		ADVANCE_PTR;
		return true;
	}

	return false;
}

Token_ptr Parser::consume_token(WTokenType token_type)
{
	auto token = this->get_current_token();

	if (token != nullptr && token_type == token->type)
	{
		ADVANCE_PTR;
		return token;
	}

	return nullptr;
}

void Parser::ignore(WTokenType token_type)
{
	while (this->expect_current_token(token_type))
	{
	}
}

int get_parity(WTokenType token_type)
{
	switch (token_type)
	{
	case WTokenType::BANG:
	case WTokenType::UNARY_MINUS:
	case WTokenType::UNARY_PLUS:
	{
		return 1;
	}
	case WTokenType::POWER:
	case WTokenType::DIVISION:
	case WTokenType::STAR:
	case WTokenType::REMINDER:
	case WTokenType::PLUS:
	case WTokenType::MINUS:
	case WTokenType::GREATER_THAN:
	case WTokenType::GREATER_THAN_EQUAL:
	case WTokenType::LESSER_THAN:
	case WTokenType::LESSER_THAN_EQUAL:
	case WTokenType::EQUAL_EQUAL:
	case WTokenType::BANG_EQUAL:
	case WTokenType::EQUAL:
	case WTokenType::COMMA:
	case WTokenType::AND:
	case WTokenType::OR:
	{
		return 2;
	}
	default:
		return 0;
	}
}

int get_precedence(WTokenType token_type)
{
	switch (token_type)
	{
	case WTokenType::OPEN_PARENTHESIS:
	case WTokenType::CLOSE_PARENTHESIS:
	{
		return 11;
	}
	case WTokenType::OPEN_BRACKET:
	case WTokenType::CLOSE_BRACKET:
	{
		return 10;
	}
	case WTokenType::BANG:
	case WTokenType::UNARY_MINUS:
	case WTokenType::UNARY_PLUS:
	{
		return 9;
	}
	case WTokenType::POWER:
	{
		return 8;
	}
	case WTokenType::DIVISION:
	case WTokenType::STAR:
	case WTokenType::REMINDER:
	{
		return 7;
	}
	case WTokenType::PLUS:
	case WTokenType::MINUS:
	{
		return 6;
	}
	case WTokenType::GREATER_THAN:
	case WTokenType::GREATER_THAN_EQUAL:
	case WTokenType::LESSER_THAN:
	case WTokenType::LESSER_THAN_EQUAL:
	{
		return 5;
	}
	case WTokenType::EQUAL_EQUAL:
	case WTokenType::BANG_EQUAL:
	{
		return 4;
	}
	case WTokenType::AND:
	case WTokenType::OR:
	{
		return 3;
	}
	case WTokenType::EQUAL:
	{
		return 2;
	}
	case WTokenType::COMMA:
	{
		return 1;
	}
	default:
		return 0;
	}
}

bool is_right_associative(WTokenType token_type)
{
	switch (token_type)
	{
	case WTokenType::POWER:
	case WTokenType::UNARY_MINUS:
	case WTokenType::UNARY_PLUS:
	case WTokenType::BANG:
	case WTokenType::EQUAL:
	{
		return true;
	}
	default:
		return false;
	}
}