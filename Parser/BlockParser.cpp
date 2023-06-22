#pragma once
#include "pch.h"
#include "Parser.h"
#include "WTokenType.h"
#include "TokenPipe.h"
#include "Assertion.h"

#include <vector>
#include <tuple>
#include <stack>
#include <map>
#include <memory>
#include <utility>
#include <variant>
#include <cmath>

#define ADVANCE_PTR token_pipe->advance_pointer()
#define RETREAT_PTR token_pipe->retreat_pointer()
#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define CASE(token_type, call) case token_type: { return call; }
#define MAKE_STATEMENT(x) std::make_shared<Statement>(x)
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)
#define MAKE_TYPE(x) std::make_shared<TypeNode>(x)
#define MAKE_OPTIONAL_TYPE(x) std::make_shared<TypeNode>(VariantTypeNode({ std::make_shared<TypeNode>(x), std::make_shared<TypeNode>(NoneTypeNode()) }))

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::string;
using std::wstring;
using std::vector;
using std::stack;
using std::map;
using std::shared_ptr;
using std::make_shared;
using std::move;
using std::pair;
using std::make_pair;
using std::tuple;
using std::make_tuple;
using std::optional;
using std::make_optional;
using std::holds_alternative;

// Branching

Expression_ptr Parser::parse_ternary_condition(WTokenType token_type, Expression_ptr prev_condition)
{
	Expression_ptr then_arm = parse_expression();

	if (token_pipe->optional(WTokenType::ELIF))
	{
		auto elif_condition = parse_expression();
		token_pipe->require(WTokenType::THEN);

		auto elif_arm = parse_ternary_condition(WTokenType::ELIF, elif_condition);
		return MAKE_EXPRESSION(IfTernaryBranch(prev_condition, then_arm, elif_arm));
	}

	token_pipe->require(WTokenType::ELSE);

	auto expression = parse_expression();
	auto else_arm = MAKE_EXPRESSION(ElseTernaryBranch(expression));
	return MAKE_EXPRESSION(IfTernaryBranch(prev_condition, then_arm, else_arm));
}

Statement_ptr Parser::parse_branching(WTokenType token_type)
{
	auto condition = parse_expression();
	token_pipe->require(WTokenType::THEN);

	// Ternary 

	if (token_type == WTokenType::IF && !token_pipe->optional(WTokenType::EOL))
	{
		Expression_ptr ternary = parse_ternary_condition(WTokenType::IF, condition);
		token_pipe->require(WTokenType::EOL);

		return MAKE_STATEMENT(ExpressionStatement(move(ternary)));
	}

	// If Block

	Block body = parse_conditional_block();

	if (token_pipe->optional(WTokenType::ELIF))
	{
		auto alternative = parse_branching(WTokenType::ELIF);
		return MAKE_STATEMENT(IfBranch(condition, body, alternative));
	}
	else if (token_pipe->optional(WTokenType::ELSE))
	{
		auto alternative = parse_else_block();
		return MAKE_STATEMENT(IfBranch(condition, body, alternative));
	}

	token_pipe->require(WTokenType::END);
	return MAKE_STATEMENT(IfBranch(condition, body));
}

Block Parser::parse_conditional_block()
{
	Block statements;

	while (true)
	{
		token_pipe->ignore_whitespace();

		auto token = token_pipe->current();
		OPT_CHECK(token);

		auto token_type = token.value()->type;

		switch (token_type)
		{
		case WTokenType::END:
		case WTokenType::ELIF:
		case WTokenType::ELSE:
		{
			return statements;
		}
		}

		auto statement = parse_statement(false);

		if (!statement)
			break;

		statements.push_back(move(statement));
	}

	return statements;
}

Statement_ptr Parser::parse_else_block()
{
	token_pipe->require(WTokenType::EOL);
	Block else_block = parse_conditional_block();
	token_pipe->require(WTokenType::END);

	return MAKE_STATEMENT(ElseBranch(else_block));
}

// Looping

Statement_ptr Parser::parse_non_block_statement()
{
	auto token = token_pipe->current();
	OPT_CHECK(token);

	ADVANCE_PTR;

	switch (token.value()->type)
	{
		CASE(WTokenType::ASSERT, parse_assert());

	default:
	{
		RETREAT_PTR;
		return parse_expression_statement();
	}
	}
}

Block Parser::parse_block()
{
	Block statements;

	while (true)
	{
		token_pipe->ignore_whitespace();

		auto token = token_pipe->current();
		OPT_CHECK(token);

		if (token.value()->type == WTokenType::END)
		{
			token_pipe->advance_pointer();
			break;
		}

		auto statement = parse_statement(false);

		if (!statement)
			break;

		statements.push_back(move(statement));
	}

	return statements;
}

Statement_ptr Parser::parse_while_loop()
{
	auto condition = parse_expression();
	NULL_CHECK(condition);

	token_pipe->require(WTokenType::DO);
	
	if (token_pipe->optional(WTokenType::EOL))
	{
		auto body = parse_block();
		return MAKE_STATEMENT(WhileLoop(body, condition));
	}

	auto statement = parse_non_block_statement();
	auto body = { statement };
	return MAKE_STATEMENT(WhileLoop(body, condition));
}

Statement_ptr Parser::parse_until_loop() 
{
	auto condition = parse_expression();
	NULL_CHECK(condition);

	token_pipe->require(WTokenType::DO);

	if (token_pipe->optional(WTokenType::EOL))
	{
		auto body = parse_block();
		return MAKE_STATEMENT(UntilLoop(body, condition));
	}

	auto statement = parse_non_block_statement();
	auto body = { statement };
	return MAKE_STATEMENT(UntilLoop(body, condition));
}

Statement_ptr Parser::parse_for_in_loop()
{
	auto expression = parse_expression();
	NULL_CHECK(expression);

	ASSERT(holds_alternative<Infix>(*expression), "Infix must be an IN expression");
	auto infix_expression = get_if<Infix>(&*expression);
	ASSERT(infix_expression->op->type == WTokenType::IN_KEYWORD, "Infix must be IN");

	Expression_ptr lhs = infix_expression->left;
	Expression_ptr iterable_expression = infix_expression->right;

	token_pipe->require(WTokenType::DO);

	if (token_pipe->optional(WTokenType::EOL))
	{
		auto body = parse_block();
		return MAKE_STATEMENT(ForInLoop(body, lhs, iterable_expression));
	}

	auto statement = parse_non_block_statement();
	auto body = { statement };
	return MAKE_STATEMENT(ForInLoop(body, lhs, iterable_expression));
}

// Matching
/*
Statement_ptr Parser::parse_match()
{
	auto match_top_expression = parse_expression();
	token_pipe->require(WTokenType::WITH);

	std::vector<Expression_ptr> case_expressions;
	std::vector<Block> case_blocks;

	if (token_pipe->optional(WTokenType::EOL))
	{
		auto body = parse_match_cases();
		token_pipe->require(WTokenType::END);
		return MAKE_STATEMENT(Matching(match_top_expression, case_expressions, case_blocks));
	}

	// parse withs

	auto statement = parse_with_expression();
	auto body = { statement };
	return MAKE_STATEMENT(Matching(match_expression, case_expressions, case_blocks));
}

std::pair<Expression_ptr, Block> Parser::parse_match_cases()
{
	auto case_expression = parse_expression();
	token_pipe->require(WTokenType::THEN);

	if (token_pipe->optional(WTokenType::EOL))
	{
		auto body = parse_match_case_block();
		return make_pair(case_expression, body);
	}

	auto statement = parse_non_block_statement();
	auto body = { statement };
	return make_pair(case_expression, body);
}

Block Parser::parse_match_case_block()
{
	Block statements;

	while (true)
	{
		token_pipe->ignore_whitespace();

		auto token = token_pipe->current();
		OPT_CHECK(token);

		if (token.value()->type == WTokenType::CASE)
		{
			token_pipe->advance_pointer();
			break;
		}

		auto statement = parse_statement(false);

		if (!statement)
			break;

		statements.push_back(move(statement));
	}

	return statements;
}*/

// Utils

StringVector Parser::parse_comma_separated_identifiers()
{
	StringVector identifiers;

	while (true)
	{
		auto token = token_pipe->require(WTokenType::IDENTIFIER);
		NULL_CHECK(token);

		identifiers.push_back(token->value);

		if (!token_pipe->optional(WTokenType::COMMA))
		{
			break;
		}
	}

	return identifiers;
}
