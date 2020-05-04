#pragma once

#include "Token.h"
#include "TokenPipe.h"
#include "OperatorStack.h"
#include "Expression.h"
#include "ExpressionContext.h"
#include <stack>

class ExpressionParser
{
	TokenPipe_ptr token_pipe;
	OperatorStack_ptr operator_stack;
	ExpressionStack ast;

	std::stack<ExpressionContext> context_stack;

	// Parsers

	Expression_ptr parse_identifier(Token_ptr identifier_token);
	Expression_ptr parse_list_literal();
	Expression_ptr parse_tuple_literal();
	Expression_ptr parse_dictionary_literal();
	ExpressionVector parse_function_call_arguments();

	// Utils

	Token_ptr consume_valid_dictionary_key();
	Expression_ptr finish_parsing();

public:
	ExpressionParser(TokenPipe_ptr token_pipe)
		: token_pipe(token_pipe),
		operator_stack(std::make_shared<OperatorStack>()) {};

	Expression_ptr parse_expression();
	ExpressionVector parse_expressions();
};

using ExpressionParser_ptr = std::shared_ptr<ExpressionParser>;
