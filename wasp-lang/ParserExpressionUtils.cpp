#pragma once
#include "Parser.h"

void Parser::pop_all_from_stack_into_ast(stack<Token_ptr>& op_stack, vector<ExpressionNode_ptr>& ast)
{
	while (op_stack.size() > 0)
	{
		Token_ptr top_operator = op_stack.top();
		op_stack.pop();

		int parity = get_parity(top_operator->get_type());

		if (parity == 1)
			this->push_unary_operator_to_ast(top_operator, ast);
		else if (parity == 2)
			this->push_binary_operator_to_ast(top_operator, ast);
	}
}

void Parser::push_unary_operator_to_ast(Token_ptr op, vector<ExpressionNode_ptr>& ast)
{
	if (ast.size() >= 1)
	{
		ExpressionNode_ptr node = ast.back();
		ast.pop_back();

		ast.push_back(MAKE_EXPR(Unary(op, node)));
	}
}

void Parser::push_binary_operator_to_ast(Token_ptr op, vector<ExpressionNode_ptr>& ast)
{
	if (ast.size() >= 2)
	{
		ExpressionNode_ptr right_expression = ast.back();
		ast.pop_back();

		ExpressionNode_ptr left_expression = ast.back();
		ast.pop_back();

		ast.push_back(MAKE_EXPR(Binary(left_expression, op, right_expression)));
	}
}

void Parser::pop_until_open_parenthesis_from_stack_into_ast(stack<Token_ptr>& op_stack, vector<ExpressionNode_ptr>& ast)
{
	while (op_stack.size() > 0)
	{
		Token_ptr top_operator = op_stack.top();
		op_stack.pop();

		if (top_operator->get_type() == TokenType::OPEN_PARENTHESIS)
			break;

		int parity = get_parity(top_operator->get_type());

		if (parity == 1)
			this->push_unary_operator_to_ast(top_operator, ast);
		else if (parity == 2)
			this->push_binary_operator_to_ast(top_operator, ast);
	}
}

void Parser::push_operator_to_operator_stack(Token_ptr op, stack<Token_ptr>& op_stack, vector<ExpressionNode_ptr>& ast)
{
	int operator_precedence = get_precedence(op->get_type());

	while (op_stack.size() > 0)
	{
		Token_ptr top_operator = op_stack.top();
		auto top_operator_type = top_operator->get_type();
		int top_operator_precedence = get_precedence(top_operator_type);

		if (
			(
				top_operator_type == TokenType::FunctionIdentifier
				|| (top_operator_precedence > operator_precedence)
				|| (top_operator_precedence == operator_precedence && !is_right_associative(top_operator_type))
				)
			&& top_operator_type != TokenType::OPEN_PARENTHESIS
			)
		{
			op_stack.pop();

			int parity = get_parity(top_operator_type);

			if (parity == 1)
			{
				this->push_unary_operator_to_ast(top_operator, ast);
			}
			else if (parity == 2)
			{
				this->push_binary_operator_to_ast(top_operator, ast);
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