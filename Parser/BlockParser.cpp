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

Expression_ptr Parser::parse_ternary_condition(Expression_ptr condition)
{
	Expression_ptr then_arm = parse_expression();

	token_pipe->require(WTokenType::ELSE);
	Expression_ptr else_arm = parse_expression((int)Precedence::TERNARY_CONDITION - 1);

	return MAKE_EXPRESSION(TernaryCondition(condition, then_arm, else_arm));
}

// Branching

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

Statement_ptr Parser::parse_else()
{
	token_pipe->require(WTokenType::EOL);
	Block else_block = parse_conditional_block();
	token_pipe->require(WTokenType::END);

	return MAKE_STATEMENT(ElseBranch(else_block));
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

	if (holds_alternative<TypedAssignment>(*condition))
	{
		auto [lhs, rhs, type_node] = deconstruct_TypedAssignment(condition);

		if (token_pipe->optional(WTokenType::ELIF))
		{
			auto alternative = parse_branching(WTokenType::ELIF);
			return MAKE_STATEMENT(TaggedIfBranch(lhs, rhs, body, type_node, alternative));
		}
		else if (token_pipe->optional(WTokenType::ELSE))
		{
			auto alternative = parse_else();
			return MAKE_STATEMENT(TaggedIfBranch(lhs, rhs, body, alternative));
		}

		token_pipe->require(WTokenType::END);
		return MAKE_STATEMENT(TaggedIfBranch(lhs, rhs, body, type_node));
	}
	else if (holds_alternative<UntypedAssignment>(*condition))
	{
		auto [lhs, rhs] = deconstruct_UntypedAssignment(condition);

		if (token_pipe->optional(WTokenType::ELIF))
		{
			auto alternative = parse_branching(WTokenType::ELIF);
			return MAKE_STATEMENT(TaggedIfBranch(lhs, rhs, body, alternative));
		}
		else if (token_pipe->optional(WTokenType::ELSE))
		{
			auto alternative = parse_else();
			return MAKE_STATEMENT(TaggedIfBranch(lhs, rhs, body, alternative));
		}

		token_pipe->require(WTokenType::END);
		return MAKE_STATEMENT(TaggedIfBranch(lhs, rhs, body));
	}

	// Simple If Branch

	if (token_pipe->optional(WTokenType::ELIF))
	{
		auto alternative = parse_branching(WTokenType::ELIF);
		return MAKE_STATEMENT(SimpleIfBranch(condition, body, alternative));
	}
	else if (token_pipe->optional(WTokenType::ELSE))
	{
		auto alternative = parse_else();
		return MAKE_STATEMENT(SimpleIfBranch(condition, body, alternative));
	}

	token_pipe->require(WTokenType::END);
	return MAKE_STATEMENT(SimpleIfBranch(condition, body));
}

// Looping

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

	if (holds_alternative<TypedAssignment>(*condition))
	{
		auto [lhs, rhs, type_node] = deconstruct_TypedAssignment(condition);

		if (token_pipe->optional(WTokenType::EOL))
		{
			auto body = parse_block();
			return MAKE_STATEMENT(AssignedWhileLoop(body, lhs, rhs, type_node));
		}

		auto statement = parse_non_block_statement();
		auto body = { statement };
		return MAKE_STATEMENT(AssignedWhileLoop(body, lhs, rhs, type_node));
	}
	else if (holds_alternative<UntypedAssignment>(*condition))
	{
		auto [lhs, rhs] = deconstruct_UntypedAssignment(condition);

		if (token_pipe->optional(WTokenType::EOL))
		{
			auto body = parse_block();
			return MAKE_STATEMENT(AssignedWhileLoop(body, lhs, rhs));
		}

		auto statement = parse_non_block_statement();
		auto body = { statement };
		return MAKE_STATEMENT(AssignedWhileLoop(body, lhs, rhs));
	}

	// Simple While Loop

	if (token_pipe->optional(WTokenType::EOL))
	{
		auto body = parse_block();
		return MAKE_STATEMENT(SimpleWhileLoop(body, condition));
	}

	auto statement = parse_non_block_statement();
	auto body = { statement };
	return MAKE_STATEMENT(SimpleWhileLoop(body, condition));
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

	// type annotation is present

	if (holds_alternative<TypePattern>(*lhs))
	{
		auto [deconstruction, type_node] = deconstruct_type_pattern(lhs);

		if (holds_alternative<Identifier>(*deconstruction))
		{
			auto identifier = get_if<Identifier>(&*deconstruction);

			if (token_pipe->optional(WTokenType::EOL))
			{
				auto body = parse_block();
				return MAKE_STATEMENT(SimpleForInLoop(body, identifier->name, iterable_expression, type_node));
			}

			auto statement = parse_non_block_statement();
			return MAKE_STATEMENT(SimpleForInLoop({ statement }, identifier->name, iterable_expression, type_node));
		}
		
		if (token_pipe->optional(WTokenType::EOL))
		{
			auto body = parse_block();
			return MAKE_STATEMENT(DeconstructedForInLoop(body, deconstruction, iterable_expression, type_node));
		}

		auto statement = parse_non_block_statement();
		auto body = { statement };
		return MAKE_STATEMENT(DeconstructedForInLoop(body, deconstruction, iterable_expression, type_node));
	}

	// type annotation is absent

	else if (holds_alternative<Identifier>(*lhs))
	{
		auto identifier = get_if<Identifier>(&*lhs);

		if (token_pipe->optional(WTokenType::EOL))
		{
			auto body = parse_block();
			return MAKE_STATEMENT(SimpleForInLoop(body, identifier->name, iterable_expression));
		}

		auto statement = parse_non_block_statement();
		return MAKE_STATEMENT(SimpleForInLoop({ statement }, identifier->name, iterable_expression));
	}

	if (token_pipe->optional(WTokenType::EOL))
	{
		auto body = parse_block();
		return MAKE_STATEMENT(DeconstructedForInLoop(body, lhs, iterable_expression));
	}

	auto statement = parse_non_block_statement();
	auto body = { statement };
	return MAKE_STATEMENT(DeconstructedForInLoop(body, lhs, iterable_expression));
}

// Testing

Statement_ptr Parser::parse_scenario()
{
	auto statements = parse_block();
	auto title = token_pipe->require(WTokenType::STRING_LITERAL);
	return MAKE_STATEMENT(Scenario(title->value, statements));
}

Statement_ptr Parser::parse_test()
{
	auto statements = parse_block();
	auto title = token_pipe->require(WTokenType::STRING_LITERAL);
	return MAKE_STATEMENT(Test(title->value, statements));
}

Statement_ptr Parser::parse_namespace()
{
	auto title = token_pipe->require(WTokenType::IDENTIFIER);
	auto statements = parse_block();
	return MAKE_STATEMENT(Namespace(title->value, statements));
}

// Utils

tuple<Expression_ptr, Expression_ptr, TypeNode_ptr> Parser::deconstruct_TypedAssignment(Expression_ptr expr)
{
	auto typed_assignment = get_if<TypedAssignment>(&*expr);

	auto lhs = typed_assignment->lhs_expression;
	auto rhs = typed_assignment->rhs_expression;
	auto type_node = typed_assignment->type_node;

	return make_tuple(lhs, rhs, type_node);
}

tuple<Expression_ptr, Expression_ptr> Parser::deconstruct_UntypedAssignment(Expression_ptr expr)
{
	auto untyped_assignment = get_if<UntypedAssignment>(&*expr);

	auto lhs = untyped_assignment->lhs_expression;
	auto rhs = untyped_assignment->rhs_expression;

	return make_tuple(lhs, rhs);
}

tuple<Expression_ptr, TypeNode_ptr> Parser::deconstruct_type_pattern(Expression_ptr expression)
{
	ASSERT(holds_alternative<TypePattern>(*expression), "Expected a TypePattern");
	auto type_pattern = get_if<TypePattern>(&*expression);

	return std::make_tuple(type_pattern->expression, type_pattern->type_node);
}
