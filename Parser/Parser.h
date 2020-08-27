#pragma once

#ifdef PARSER_EXPORTS
#define PARSER_API __declspec(dllexport)
#else
#define PARSER_API __declspec(dllimport)
#endif

#include "Token.h"
#include "TokenPipe.h"
#include "Expression.h"
#include "Statement.h"
#include "ExpressionParser.h"

#include <vector>
#include <tuple>
#include <stack>
#include <memory>
#include <utility>

class PARSER_API Parser
{
	TokenPipe_ptr token_pipe;
	ExpressionParser_ptr expr_parser;

	Statement_ptr parse_statement(bool is_public = false, int expected_indent = 0);
	Statement_ptr parse_public_statement(int expected_indent);
	Statement_ptr parse_expression_statement();

	Statement_ptr parse_pass();
	Statement_ptr parse_return();
	Statement_ptr parse_yield();
	Statement_ptr parse_assert();
	Statement_ptr parse_break();
	Statement_ptr parse_continue();

	Statement_ptr consume_assignment(ExpressionVector lhs_expressions);
	Statement_ptr consume_shortcut_assignment(Expression_ptr lhs_expression, Token_ptr shortcut_operator);

	Statement_ptr parse_import();

	// Block statement parsing

	Statement_ptr parse_branching(int expected_indent = 0);
	Statement_ptr parse_while_loop(int expected_indent = 0);
	Statement_ptr parse_for_in_loop(int expected_indent = 0);

	// Type parsers

	Type_ptr parse_type(bool is_optional = false);
	Type_ptr parse_list_type(bool is_optional);
	Type_ptr parse_tuple_type(bool is_optional);
	Type_ptr parse_map_type(bool is_optional);
	Type_ptr consume_datatype_word(bool is_optional);

	std::pair<std::wstring, Type_ptr> consume_identifier_type_pair();

	// Definition Parsers

	Statement_ptr parse_variable_definition(bool is_public, bool is_mutable);
	Statement_ptr parse_type_definition(bool is_public, int expected_indent);

	Statement_ptr parse_enum_definition(bool is_public, int expected_indent);
	std::vector<std::wstring> parse_enum_members(int expected_indent);

	std::tuple<std::wstring, std::vector<std::pair<std::wstring, Type_ptr>>, TypeVector, std::optional<Type_ptr>, Block> parse_callable_definition(int expected_indent);
	Statement_ptr parse_function_definition(bool is_public, int expected_indent);
	Statement_ptr parse_generator_definition(bool is_public, int expected_indent);

	// Utils

	Block parse_block(int expected_indent);
	std::pair<Expression_ptr, Block> parse_condition_and_consequence(int expected_indent);

	void convert_shortcut_token(Token_ptr token);

	void init(std::vector<Token_ptr>& tokens);

public:
	Parser() {};
	Module_ptr execute(std::vector<Token_ptr>& tokens);
};

using Parser_ptr = PARSER_API std::unique_ptr<Parser>;
