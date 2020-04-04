#pragma once

#ifdef PARSER_EXPORTS
#define PARSER_API __declspec(dllexport)
#else
#define PARSER_API __declspec(dllimport)
#endif

#include <stack>
#include "TokenPipe.h"
#include "OperatorStack.h"
#include "Expression.h"

#define RETURN_NULLPTR_IF_NULLPTR(x) if (x == nullptr) { return nullptr; }
#define RETURN_NULLPTR_IF_TRUE(x) if (x) { return nullptr; }
#define RETURN_NULLPTR_IF_FALSE(x) if (!x) { return nullptr; }

class PARSER_API ExpressionParser
{
	TokenPipe_ptr token_pipe;

	OperatorStack_ptr operator_stack;
	ExpressionStack ast;

	std::stack<bool> inside_function_call;

	// Parsers

	Expression_ptr consume_member_access(Token_ptr identifier_token);
	ExpressionVector parse_function_call_arguments();

	// Literal parsers

	Expression_ptr parse_vector_literal();
	Expression_ptr parse_UDT_literal();

	// Utils

	Expression_ptr finish_parsing();
	std::shared_ptr<std::string> consume_valid_record_key();

public:
	ExpressionParser(TokenPipe_ptr token_pipe)
		: token_pipe(token_pipe),
		operator_stack(std::make_shared<OperatorStack>())
	{};
	Expression_ptr parse_expression();
};

using ExpressionParser_ptr = std::shared_ptr<ExpressionParser>;
