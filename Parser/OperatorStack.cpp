#pragma once
#include "pch.h"
#include "OperatorStack.h"

using std::vector;
using std::stack;
using std::make_shared;
using std::shared_ptr;
using std::pair;
using std::make_pair;
using std::move;

void OperatorStack::pop_all_from_stack_into_ast(vector<Expression_ptr>& ast)
{
	while (op_stack.size() > 0)
	{
		Token_ptr top_operator = move(op_stack.top());
		op_stack.pop();

		int parity = get_parity(top_operator->type);

		if (parity == 1)
			this->push_unary_operator_to_ast(move(top_operator), ast);
		else if (parity == 2)
			this->push_binary_operator_to_ast(move(top_operator), ast);
	}
}

void OperatorStack::push_unary_operator_to_ast(Token_ptr op, vector<Expression_ptr>& ast)
{
	if (ast.size() >= 1)
	{
		Expression_ptr node = move(ast.back());
		ast.pop_back();

		ast.push_back(make_shared<Unary>(move(op), move(node)));
	}
}

void OperatorStack::push_binary_operator_to_ast(Token_ptr op, vector<Expression_ptr>& ast)
{
	if (ast.size() >= 2)
	{
		Expression_ptr right_expression = move(ast.back());
		ast.pop_back();

		Expression_ptr left_expression = move(ast.back());
		ast.pop_back();

		ast.push_back(make_shared<Binary>(move(left_expression), move(op), move(right_expression)));
	}
}

void OperatorStack::pop_until_open_parenthesis_from_stack_into_ast(vector<Expression_ptr>& ast)
{
	while (op_stack.size() > 0)
	{
		Token_ptr top_operator = move(op_stack.top());
		op_stack.pop();

		if (top_operator->type == WTokenType::OPEN_PARENTHESIS)
			break;

		int parity = get_parity(top_operator->type);

		if (parity == 1)
			this->push_unary_operator_to_ast(move(top_operator), ast);
		else if (parity == 2)
			this->push_binary_operator_to_ast(move(top_operator), ast);
	}
}

void OperatorStack::push_operator_to_operator_stack(Token_ptr op, vector<Expression_ptr>& ast)
{
	int operator_precedence = get_precedence(op->type);

	while (op_stack.size() > 0)
	{
		Token_ptr top_operator = move(op_stack.top());
		auto top_operator_type = top_operator->type;
		int top_operator_precedence = get_precedence(top_operator_type);

		if (
			(
				top_operator_type == WTokenType::FunctionIdentifier
				|| (top_operator_precedence > operator_precedence)
				|| (top_operator_precedence == operator_precedence && !is_right_associative(top_operator_type))
				)
			&& top_operator_type != WTokenType::OPEN_PARENTHESIS
			)
		{
			op_stack.pop();

			int parity = get_parity(top_operator_type);

			if (parity == 1)
			{
				this->push_unary_operator_to_ast(move(top_operator), ast);
			}
			else if (parity == 2)
			{
				this->push_binary_operator_to_ast(move(top_operator), ast);
			}
		}
		else
		{
			op_stack.push(top_operator);
			break;
		}
	}

	op_stack.push(op);
}

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
	case WTokenType::PLUS_EQUAL:
	case WTokenType::MINUS_EQUAL:
	case WTokenType::STAR_EQUAL:
	case WTokenType::DIVISION_EQUAL:
	case WTokenType::REMINDER_EQUAL:
	case WTokenType::POWER_EQUAL:
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
	case WTokenType::PLUS_EQUAL:
	case WTokenType::MINUS_EQUAL:
	case WTokenType::STAR_EQUAL:
	case WTokenType::DIVISION_EQUAL:
	case WTokenType::REMINDER_EQUAL:
	case WTokenType::POWER_EQUAL:
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

bool OperatorStack::is_right_associative(WTokenType token_type)
{
	switch (token_type)
	{
	case WTokenType::POWER:
	case WTokenType::UNARY_MINUS:
	case WTokenType::UNARY_PLUS:
	case WTokenType::BANG:
	case WTokenType::EQUAL:

	case WTokenType::PLUS_EQUAL:
	case WTokenType::MINUS_EQUAL:
	case WTokenType::STAR_EQUAL:
	case WTokenType::DIVISION_EQUAL:
	case WTokenType::REMINDER_EQUAL:
	case WTokenType::POWER_EQUAL:
	{
		return true;
	}
	default:
		return false;
	}
}