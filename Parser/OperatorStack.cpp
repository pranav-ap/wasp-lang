#pragma once
#include "pch.h"
#include "OperatorStack.h"
#include "CommonAssertion.h"
#include <iostream>

using std::vector;
using std::stack;
using std::make_shared;
using std::shared_ptr;
using std::pair;
using std::make_pair;
using std::move;

void OperatorStack::push_operator_into_ast(Token_ptr operator_token, ExpressionStack& ast)
{
	int parity = get_parity(operator_token->type);

	switch (parity)
	{
	case 1:
	{
		this->push_unary_operator_to_ast(move(operator_token), ast);
		break;
	}
	case 2:
	{
		this->push_binary_operator_to_ast(move(operator_token), ast);
		break;
	}
	default:
	{
		FATAL(operator_token->value + " operator parity is neither 1 nor 2");
	}
	}
}

void OperatorStack::push_unary_operator_to_ast(Token_ptr operator_token, ExpressionStack& ast)
{
	ASSERT(
		ast.size() != 0,
		operator_token->value + " requries one operand. But the AST is empty."
	);

	Expression_ptr expression = move(ast.top());
	ast.pop();

	ast.push(
		make_shared<Unary>(
			move(operator_token),
			move(expression)
			)
	);
}

void OperatorStack::push_binary_operator_to_ast(Token_ptr operator_token, ExpressionStack& ast)
{
	ASSERT(
		ast.size() < 2,
		operator_token->value + " requires two operands"
	);

	Expression_ptr rhs = move(ast.top());
	ast.pop();

	Expression_ptr lhs = move(ast.top());
	ast.pop();

	ast.push(
		make_shared<Binary>(
			move(lhs),
			move(operator_token),
			move(rhs)
			)
	);
}

void OperatorStack::drain_into_ast(ExpressionStack& ast)
{
	while (operator_stack.size() > 0)
	{
		Token_ptr top_operator = move(operator_stack.top());
		operator_stack.pop();

		push_operator_into_ast(top_operator, ast);
	}
}

void OperatorStack::drain_into_ast_until_open_parenthesis(ExpressionStack& ast)
{
	while (operator_stack.size() > 0)
	{
		Token_ptr top_operator = move(operator_stack.top());
		operator_stack.pop();

		if (top_operator->type == WTokenType::OPEN_PARENTHESIS)
			break;

		push_operator_into_ast(top_operator, ast);
	}
}

void OperatorStack::dumb_push(Token_ptr operator_token)
{
	operator_stack.push(move(operator_token));
}

void OperatorStack::smart_push(Token_ptr operator_token, ExpressionStack& ast)
{
	int operator_precedence = get_precedence(operator_token->type);

	while (operator_stack.size() > 0)
	{
		Token_ptr top_operator = operator_stack.top();
		auto top_operator_type = top_operator->type;
		int top_operator_precedence = get_precedence(top_operator_type);

		if (top_operator_type != WTokenType::OPEN_PARENTHESIS)
		{
			if (top_operator_precedence > operator_precedence ||
				(top_operator_precedence == operator_precedence && is_left_associative(top_operator_type)))
			{
				operator_stack.pop();
				push_operator_into_ast(move(top_operator), ast);
			}
		}
		else
		{
			break;
		}
	}

	operator_stack.push(operator_token);
}

// UTILS

int OperatorStack::get_parity(WTokenType token_type)
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

int OperatorStack::get_precedence(WTokenType token_type)
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

bool OperatorStack::is_left_associative(WTokenType token_type)
{
	switch (token_type)
	{
	case WTokenType::POWER:
	case WTokenType::UNARY_MINUS:
	case WTokenType::UNARY_PLUS:
	case WTokenType::BANG:
	case WTokenType::EQUAL:
	{
		return false;
	}
	default:
		return true;
	}
}