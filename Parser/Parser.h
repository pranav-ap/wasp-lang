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

	Expression_ptr parse_ternary_condition(Expression_ptr condition);
	Block parse_conditional_block();
	Statement_ptr parse_else();
	Statement_ptr parse_branching(WTokenType token_type);

	Statement_ptr parse_non_block_statement();
	Block parse_block();

	Statement_ptr parse_while_loop();
	Statement_ptr parse_for_in_loop();

	Statement_ptr parse_return();
	Statement_ptr parse_yield();
	Statement_ptr parse_assert();
	Statement_ptr parse_implore();
	Statement_ptr parse_swear();
	Statement_ptr parse_break();
	Statement_ptr parse_continue();
	Statement_ptr parse_redo();

	// TypeNode parsers

	TypeNode_ptr parse_list_type(bool is_optional);
	TypeNode_ptr parse_set_type(bool is_optional);
	TypeNode_ptr parse_tuple_type(bool is_optional);
	TypeNode_ptr parse_map_type(bool is_optional);
	TypeNode_ptr consume_datatype_word(bool is_optional);

	// Definition Parsers

	Statement_ptr parse_variable_definition(bool is_public, bool is_mutable);

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

	std::tuple<Expression_ptr, TypeNode_ptr> deconstruct_type_pattern(Expression_ptr expr);
	std::tuple<Expression_ptr, Expression_ptr> deconstruct_UntypedAssignment(Expression_ptr expr);
	std::tuple<Expression_ptr, Expression_ptr, TypeNode_ptr> deconstruct_TypedAssignment(Expression_ptr expr);

public:
	TokenPipe_ptr token_pipe;

	Parser();
	
	TypeNode_ptr parse_type(bool is_optional = false);
	Expression_ptr parse_expression();
	Expression_ptr parse_expression(int precedence);
	ExpressionVector parse_expressions();

	Module_ptr run(std::vector<Token_ptr>& tokens);
};

using Parser_ptr = PARSER_API Parser*;
