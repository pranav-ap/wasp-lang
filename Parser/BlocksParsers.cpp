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

#define MAKE_OPTIONAL_EXPRESSION(x) std::make_shared<Expression>(VariantTypeNode({ std::make_shared<Expression>(x), std::make_shared<Expression>(NoneTypeNode()) }))

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

Statement_ptr Parser::parse_namespace(bool is_public)
{
	Block statements;

	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	NULL_CHECK(identifier);

	while (!token_pipe->optional(WTokenType::END))
	{
		Statement_ptr node = parse_statement();
		statements.push_back(node);
	}

	return MAKE_STATEMENT(Namespace(identifier->value, statements, is_public));
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

Statement_ptr Parser::parse_non_block_statement()
{
	auto token = token_pipe->current();
	OPT_CHECK(token);

	ADVANCE_PTR;

	switch (token.value()->type)
	{
		CASE(WTokenType::RETURN_KEYWORD, parse_return());
		CASE(WTokenType::YIELD_KEYWORD, parse_yield());
		CASE(WTokenType::BREAK, parse_break());
		CASE(WTokenType::CONTINUE, parse_continue());
		CASE(WTokenType::REDO, parse_redo());
		CASE(WTokenType::ASSERT, parse_assert());
		CASE(WTokenType::IMPLORE, parse_implore());
		CASE(WTokenType::SWEAR, parse_swear());

	default:
	{
		RETREAT_PTR;
		return parse_expression_statement();
	}
	}
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

Statement_ptr Parser::parse_condition_and_consequence()
{
	auto condition = parse_expression();
	token_pipe->require(WTokenType::THEN);
	token_pipe->require(WTokenType::EOL);

	auto body = parse_conditional_block();
	return MAKE_STATEMENT(IfBranch(move(condition), body));
}

Expression_ptr Parser::parse_ternary_condition(Expression_ptr condition)
{
	Expression_ptr then_arm = parse_expression();

	token_pipe->require(WTokenType::ELSE);
	Expression_ptr else_arm = parse_expression((int)Precedence::TERNARY_CONDITION - 1);

	return MAKE_EXPRESSION(TernaryCondition(condition, then_arm, else_arm));
}

Statement_ptr Parser::parse_branching(WTokenType token_type)
{
	auto condition = parse_expression();
	token_pipe->require(WTokenType::THEN);

	if (token_type == WTokenType::IF && !token_pipe->optional(WTokenType::EOL))
	{
		Expression_ptr ternary = parse_ternary_condition(condition);
		token_pipe->require(WTokenType::EOL);

		return MAKE_STATEMENT(ExpressionStatement(move(ternary)));
	}

	Block body = parse_conditional_block();
	auto if_branch = IfBranch(condition, body);

	if (token_pipe->optional(WTokenType::ELIF))
	{
		auto alternative = parse_branching(WTokenType::ELIF);
		if_branch.alternative = alternative;
		return MAKE_STATEMENT(if_branch);
	}
	else if (token_pipe->optional(WTokenType::ELSE))
	{
		token_pipe->require(WTokenType::EOL);
		Block else_block = parse_conditional_block();
		if_branch.alternative = MAKE_STATEMENT(ElseBranch(else_block));
	}

	token_pipe->require(WTokenType::END);

	return MAKE_STATEMENT(if_branch);
}

Statement_ptr Parser::parse_while_loop()
{
	auto condition = parse_expression();
	NULL_CHECK(condition);

	token_pipe->require(WTokenType::DO);

	if (token_pipe->optional(WTokenType::EOL))
	{
		auto body = parse_block();
		return MAKE_STATEMENT(WhileLoop(condition, body));
	}

	auto statement = parse_non_block_statement();
	return MAKE_STATEMENT(WhileLoop(condition, { statement }));
}

Statement_ptr Parser::parse_for_in_loop()
{
	auto expression = parse_expression();
	NULL_CHECK(expression);

	ASSERT(holds_alternative<Infix>(*expression), "Infix must be an IN expression");
	auto infix_expression = get_if<Infix>(&*expression);
	ASSERT(infix_expression->op->type == WTokenType::IN_KEYWORD, "Infix must be IN");

	auto lhs_pattern = infix_expression->left;
	auto rhs_pattern = infix_expression->right;

	token_pipe->require(WTokenType::DO);

	if (token_pipe->optional(WTokenType::EOL))
	{
		auto body = parse_block();
		return MAKE_STATEMENT(ForInLoop(lhs_pattern, rhs_pattern, body));
	}

	auto statement = parse_non_block_statement();
	return MAKE_STATEMENT(ForInLoop(lhs_pattern, rhs_pattern, { statement }));
}
