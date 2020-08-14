#pragma once

#include "Token.h"
#include "WTokenType.h"
#include "Expression.h"

#include <stack>
#include <vector>
#include <memory>

class OperatorStack
{
	std::stack<Token_ptr> operator_stack;

	void push_operator_into_ast(Token_ptr operator_token, ExpressionStack& ast);

	void push_unary_operator_to_ast(Token_ptr op, ExpressionStack& ast);
	void push_binary_operator_to_ast(Token_ptr op, ExpressionStack& ast);

	// UTILS

	int get_parity(WTokenType token_type);
	int get_precedence(WTokenType token_type);
	bool is_left_associative(WTokenType token_type);

public:
	void drain_into_ast(ExpressionStack& ast);
	void drain_into_ast_until_open_parenthesis(ExpressionStack& ast);

	void dumb_push(Token_ptr operator_token);
	void smart_push(Token_ptr operator_token, ExpressionStack& ast);
};

using OperatorStack_ptr = std::shared_ptr<OperatorStack>;
