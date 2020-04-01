#pragma once

#ifdef PARSER_EXPORTS
#define PARSER_API __declspec(dllexport)
#else
#define PARSER_API __declspec(dllimport)
#endif

#include <vector>
#include <stack>
#include <memory>
#include <utility>

#include "Token.h"
#include "Types.h"
#include "Module.h"
#include "Expression.h"
#include "Statement.h"
#include "TokenPipe.h"

#define ADVANCE_PTR token_pipe->advance_pointer()
#define RETREAT_PTR token_pipe->retreat_pointer()

#define RETURN_NULLPTR_IF_NULLPTR(x) if (x == nullptr) { return nullptr; }
#define RETURN_NULLPTR_IF_TRUE(x) if (x) { return nullptr; }
#define RETURN_NULLPTR_IF_FALSE(x) if (!x) { return nullptr; }
#define CASE(token_type, call) case token_type: { return call; }

class PARSER_API ExpressionParser
{
	TokenPipe_ptr token_pipe;
	std::stack<bool> inside_function_call;

public:
};

class PARSER_API Parser
{
	TokenPipe_ptr token_pipe;

	Statement_ptr parse_statement(bool is_public);
	Statement_ptr parse_public_statement();

	Statement_ptr parse_expression_statement();
	Expression_ptr parse_expression();

	Statement_ptr parse_return_statement();
	Statement_ptr parse_break_statement();
	Statement_ptr parse_continue_statement();

	Statement_ptr parse_variable_declaration(bool is_public, bool is_mutable);
	Statement_ptr handle_identifier(Token_ptr identifier);

	// Literal parsers

	Expression_ptr parse_vector_literal();
	Expression_ptr parse_record_literal();

	// Block statement parsing

	Statement_ptr parse_branching_statement();
	Statement_ptr parse_loop_statement();

	// Type parsers

	Type_ptr parse_type();
	Type_ptr parse_vector_type();
	Type_ptr consume_datatype_word();

	// Definition Parsers

	Statement_ptr parse_UDT_declaration(bool is_public);
	Statement_ptr parse_function_definition(bool is_public);

	// Utils
	Block_ptr parse_block();
	std::shared_ptr<std::string> consume_valid_record_key();
	void convert_to_equivalent_token(Token_ptr token);

public:
	Parser(std::vector<Token_ptr>& tokens)
		: token_pipe(std::make_shared<TokenPipe>(tokens)) {};

	Module execute();
};
