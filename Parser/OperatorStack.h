#pragma once

#ifdef PARSER_EXPORTS
#define PARSER_API __declspec(dllexport)
#else
#define PARSER_API __declspec(dllimport)
#endif

#include <stack>
#include <vector>

#include "Token.h"
#include "Expression.h"

class PARSER_API OperatorStack
{
	std::stack<Token_ptr> op_stack;

public:
	void pop_all_from_stack_into_ast(std::vector<Expression_ptr>& ast);

	void push_unary_operator_to_ast(Token_ptr op, std::vector<Expression_ptr>& ast);
	void push_binary_operator_to_ast(Token_ptr op, std::vector<Expression_ptr>& ast);

	void pop_until_open_parenthesis_from_stack_into_ast(std::vector<Expression_ptr>& ast);
	void push_operator_to_operator_stack(Token_ptr op, std::vector<Expression_ptr>& ast);

	// utils

	int get_parity(WTokenType token_type);
	int get_precedence(WTokenType token_type);
	bool is_right_associative(WTokenType token_type);
};

using OperatorStack_ptr = PARSER_API std::shared_ptr<OperatorStack>;
