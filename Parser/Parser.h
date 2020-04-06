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
#include "TokenPipe.h"
#include "Types.h"
#include "Expression.h"
#include "Statement.h"
#include "Module.h"
#include "ExpressionParser.h"

#define CASE(token_type, call) case token_type: { return call; }

class PARSER_API Parser
{
	TokenPipe_ptr token_pipe;
	ExpressionParser_ptr expr_parser;

	Statement_ptr parse_statement(bool is_public);
	Statement_ptr parse_public_statement();

	Statement_ptr parse_expression_statement();

	Statement_ptr parse_return_statement();
	Statement_ptr parse_break_statement();
	Statement_ptr parse_continue_statement();

	Statement_ptr parse_variable_declaration(bool is_public, bool is_mutable);
	Statement_ptr consume_assignment_or_expression_statement(Token_ptr identifier);

	Statement_ptr parse_import_statement();

	// Block statement parsing

	Statement_ptr parse_branching_statement();
	Statement_ptr parse_loop_statement();
	Statement_ptr parse_foreach_loop_statement();

	// Type parsers

	Type_ptr parse_type();
	Type_ptr parse_vector_type();
	Type_ptr consume_datatype_word();

	// Definition Parsers

	Statement_ptr parse_UDT_declaration(bool is_public);
	Statement_ptr parse_function_definition(bool is_public);
	Statement_ptr parse_enum_statement(bool is_public);

	// Utils

	Block_ptr parse_block();
	void convert_to_equivalent_token(Token_ptr token);

public:
	Parser(std::vector<Token_ptr>& tokens)
		: token_pipe(std::make_shared<TokenPipe>(tokens)),
		expr_parser(std::make_shared<ExpressionParser>(token_pipe)) {};

	Module execute();
};
