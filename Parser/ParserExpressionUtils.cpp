#pragma once
#include "pch.h"
#include "Parser.h"
#include <memory>

using std::string;
using std::vector;
using std::stack;
using std::shared_ptr;
using std::pair;
using std::make_shared;
using std::make_pair;
using std::move;

void Parser::pop_all_from_stack_into_ast(stack<Token_ptr>& op_stack, vector<Expression_ptr>& ast)
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

void Parser::push_unary_operator_to_ast(Token_ptr op, vector<Expression_ptr>& ast)
{
	if (ast.size() >= 1)
	{
		Expression_ptr node = move(ast.back());
		ast.pop_back();

		ast.push_back(make_shared<Unary>(move(op), move(node)));
	}
}

void Parser::push_binary_operator_to_ast(Token_ptr op, vector<Expression_ptr>& ast)
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

void Parser::pop_until_open_parenthesis_from_stack_into_ast(stack<Token_ptr>& op_stack, vector<Expression_ptr>& ast)
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

void Parser::push_operator_to_operator_stack(Token_ptr op, stack<Token_ptr>& op_stack, vector<Expression_ptr>& ast)
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