#pragma once
#include "Parser.h"

Token_ptr Parser::get_current_token()
{
	int index = this->pointer.get_index();
	RETURN_IF_TRUE((size_t)index >= this->tokens.size());
	return this->tokens[index];
}

bool Parser::expect_current_token(TokenType token_type)
{
	auto token = this->get_current_token();

	if (token != nullptr && token_type == token->get_type())
	{
		ADVANCE_PTR;
		return true;
	}

	return false;
}

Token_ptr Parser::consume_token(TokenType token_type)
{
	auto token = this->get_current_token();

	if (token != nullptr && token_type == token->get_type())
	{
		ADVANCE_PTR;
		return move(token);
	}

	return nullptr;
}

int get_parity(TokenType token_type)
{
	switch (token_type)
	{
	case TokenType::BANG:
	case TokenType::UNARY_MINUS:
	case TokenType::UNARY_PLUS:
	{
		return 1;
	}
	case TokenType::POWER:
	case TokenType::DIVISION:
	case TokenType::STAR:
	case TokenType::REMINDER:
	case TokenType::PLUS:
	case TokenType::MINUS:
	case TokenType::GREATER_THAN:
	case TokenType::GREATER_THAN_EQUAL:
	case TokenType::LESSER_THAN:
	case TokenType::LESSER_THAN_EQUAL:
	case TokenType::EQUAL_EQUAL:
	case TokenType::BANG_EQUAL:
	case TokenType::EQUAL:
	case TokenType::COMMA:
	case TokenType::AND:
	case TokenType::OR:
	{
		return 2;
	}
	default:
		return 0;
	}
}

int get_precedence(TokenType token_type)
{
	switch (token_type)
	{
	case TokenType::OPEN_PARENTHESIS:
	case TokenType::CLOSE_PARENTHESIS:
	{
		return 11;
	}
	case TokenType::OPEN_BRACKET:
	case TokenType::CLOSE_BRACKET:
	{
		return 10;
	}
	case TokenType::BANG:
	case TokenType::UNARY_MINUS:
	case TokenType::UNARY_PLUS:
	{
		return 9;
	}
	case TokenType::POWER:
	{
		return 8;
	}
	case TokenType::DIVISION:
	case TokenType::STAR:
	case TokenType::REMINDER:
	{
		return 7;
	}
	case TokenType::PLUS:
	case TokenType::MINUS:
	{
		return 6;
	}
	case TokenType::GREATER_THAN:
	case TokenType::GREATER_THAN_EQUAL:
	case TokenType::LESSER_THAN:
	case TokenType::LESSER_THAN_EQUAL:
	{
		return 5;
	}
	case TokenType::EQUAL_EQUAL:
	case TokenType::BANG_EQUAL:
	{
		return 4;
	}
	case TokenType::AND:
	case TokenType::OR:
	{
		return 3;
	}
	case TokenType::EQUAL:
	{
		return 2;
	}
	case TokenType::COMMA:
	{
		return 1;
	}
	default:
		return 0;
	}
}

bool is_right_associative(TokenType token_type)
{
	switch (token_type)
	{
	case TokenType::POWER:
	case TokenType::UNARY_MINUS:
	case TokenType::UNARY_PLUS:
	case TokenType::BANG:
	case TokenType::EQUAL:
	{
		return true;
	}
	default:
		return false;
	}
}

void Parser::ignore(TokenType token_type)
{
	while (this->expect_current_token(token_type))
	{
	}
}