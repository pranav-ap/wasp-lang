#pragma once

#ifdef PARSER_EXPORTS
#define PARSER_API __declspec(dllexport)
#else
#define PARSER_API __declspec(dllimport)
#endif

#include "Token.h"
#include "TokenPipe.h"
#include "TypeSystem.h"
#include "Expression.h"
#include "Statement.h"
#include "Module.h"
#include "ExpressionParser.h"
#include "StatementContext.h"

#include <vector>
#include <tuple>
#include <stack>
#include <memory>
#include <utility>

class PARSER_API Parser
{
	TokenPipe_ptr token_pipe;
	ExpressionParser_ptr expr_parser;

	std::stack<std::pair<StatementContext, int>> context_stack;

	Statement_ptr parse_statement(bool is_public);
	Statement_ptr parse_public_statement();
	Statement_ptr parse_expression_statement();

	Statement_ptr parse_pass();
	Statement_ptr parse_return();
	Statement_ptr parse_break();
	Statement_ptr parse_continue();

	Statement_ptr parse_assignment_or_expression(Token_ptr identifier);
	Statement_ptr consume_assignment(Token_ptr identifier);
	Statement_ptr consume_shortcut_assignment(Token_ptr identifier, Token_ptr shortcut_operator);
	Statement_ptr consume_multiple_assignment(Token_ptr identifier);

	Statement_ptr parse_import();

	// Block statement parsing

	Statement_ptr parse_branching();
	Statement_ptr parse_while_loop();
	Statement_ptr parse_for_in_loop();

	// Type parsers

	Type_ptr parse_type(bool is_optional = false);
	Type_ptr parse_list_type(bool is_optional);
	Type_ptr parse_map_type(bool is_optional);
	Type_ptr parse_tuple_type(bool is_optional);
	Type_ptr consume_datatype_word(bool is_optional);

	std::pair<std::string, Type_ptr> consume_identifier_type_pair();

	// Definition Parsers

	Statement_ptr parse_variable_definition(bool is_public, bool is_mutable);
	Statement_ptr parse_UDT_definition(bool is_public);
	Statement_ptr parse_function_definition(bool is_public);
	Statement_ptr parse_enum_definition(bool is_public);
	std::vector<std::string> parse_enum_members();

	// Utils

	Block parse_block(StatementContext context);
	std::pair<Expression_ptr, Block> parse_condition_and_consequence();
	void convert_shortcut_token(Token_ptr token);

	std::pair<int, int> get_indent_pair();
	void push_context(StatementContext context);
	void pop_context(StatementContext context);

public:
	Parser(std::vector<Token_ptr>& tokens)
		: token_pipe(std::make_shared<TokenPipe>(tokens)),
		expr_parser(std::make_shared<ExpressionParser>(token_pipe)) {};

	Module execute();
};
