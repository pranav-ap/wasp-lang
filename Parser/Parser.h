#pragma once

#ifdef PARSER_EXPORTS
#define PARSER_API __declspec(dllexport)
#else
#define PARSER_API __declspec(dllimport)
#endif

#include "Precedence.h"
#include "ExpressionParselets.h"
#include "Token.h"
#include "TokenPipe.h"
#include "Expression.h"
#include "Statement.h"

#include <vector>
#include <tuple>
#include <map>
#include <memory>
#include <utility>

class PARSER_API Parser
{
	// Statement Parser

	Statement_ptr parse_statement(bool is_public = false);
	Statement_ptr parse_public_statement();
	Statement_ptr parse_expression_statement();

	// Block parsers

	Block parse_conditional_block();
	Statement_ptr parse_else_block();
	Statement_ptr parse_branching(WTokenType token_type);

	Statement_ptr parse_non_block_statement();
	Block parse_block();

	Statement_ptr parse_while_loop();
	Statement_ptr parse_until_loop();
	Statement_ptr parse_for_in_loop();
	/*
	Statement_ptr parse_match();
	Statement_ptr parse_match_block();
	Expression_ptr parse_match_expression();
	std::pair<Expression_ptr, Block> parse_match_case_expression();
	Block parse_match_case_block();
	*/
	Statement_ptr parse_import();
	Statement_ptr parse_native();

	Statement_ptr parse_return();
	Statement_ptr parse_assert();
	Statement_ptr parse_break();
	Statement_ptr parse_continue();
	Statement_ptr parse_redo();

	// Function

	std::tuple<StringVector, TypeNodeVector, std::optional<TypeNode_ptr>, Block> parse_callable_definition();
	Statement_ptr parse_function_definition(bool is_public);
	std::pair<std::wstring, TypeNode_ptr> consume_identifier_type_pair();

	// TypeNode parsers

	TypeNode_ptr parse_list_type(bool is_optional);
	TypeNode_ptr parse_set_type(bool is_optional);
	TypeNode_ptr parse_tuple_type(bool is_optional);
	TypeNode_ptr parse_map_type(bool is_optional);
	TypeNode_ptr consume_datatype_word(bool is_optional);

	std::tuple<TypeNodeVector, std::optional<TypeNode_ptr>> parse_callable_type();
	TypeNode_ptr parse_function_type(bool is_optional);

	// Definition Parsers

	Statement_ptr parse_variable_definition(bool is_public, bool is_mutable);
	Statement_ptr parse_type_definition(bool is_public);
	Statement_ptr parse_class_definition(bool is_public);
	StringVector parse_inheritance();

	Statement_ptr parse_enum_definition(bool is_public);
	std::vector<std::wstring> parse_enum_members(std::wstring stem);

	// Pratt Parser

	std::map<WTokenType, IPrefixParselet_ptr> prefix_parselets;
	std::map<WTokenType, IInfixParselet_ptr> infix_parselets;

	void register_parselet(WTokenType token_type, IPrefixParselet_ptr parselet);
	void register_parselet(WTokenType token_type, IInfixParselet_ptr parselet);

	void register_prefix(WTokenType token_type, Precedence precedence);
	void register_infix_left(WTokenType token_type, Precedence precedence);
	void register_infix_right(WTokenType token_type, Precedence precedence);
	void register_postfix(WTokenType token_type, Precedence precedence);

	int get_next_operator_precedence();

	// Utils

	StringVector parse_comma_separated_identifiers();

public:
	TokenPipe_ptr token_pipe;

	Parser();

	TypeNode_ptr parse_type(bool is_optional = false);
	Expression_ptr parse_expression();
	Expression_ptr parse_expression(int precedence);
	ExpressionVector parse_expressions();
	Expression_ptr parse_ternary_condition(WTokenType token_type, Expression_ptr condition);

	Module_ptr run(std::vector<Token_ptr>& tokens);
};

using Parser_ptr = PARSER_API Parser*;
