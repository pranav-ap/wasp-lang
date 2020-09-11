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
#include <stack>
#include <map>
#include <memory>
#include <utility>

// Parser

class PARSER_API Parser
{
	// Statement Parser

	Statement_ptr parse_statement(bool is_public = false);
	Statement_ptr parse_public_statement();

	Statement_ptr parse_expression_statement();

	// Definition Parsers

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

public:
	TokenPipe_ptr token_pipe;

	Expression_ptr parse_expression();
	Expression_ptr parse_expression(int precedence);
	ExpressionVector parse_expressions();

	Parser();
	Module_ptr execute(std::vector<Token_ptr>& tokens);
};

using Parser_ptr = PARSER_API Parser*;
