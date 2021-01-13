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

Statement_ptr Parser::parse_statement(bool is_public)
{
	token_pipe->ignore_whitespace();

	auto token = token_pipe->current();

	if (!token.has_value())
		return nullptr;

	ADVANCE_PTR;

	switch (token.value()->type)
	{
		CASE(WTokenType::PUB, parse_public_statement());
		CASE(WTokenType::LET, parse_variable_definition(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, parse_variable_definition(is_public, false));
		CASE(WTokenType::IF, parse_branching(token.value()->type));
		CASE(WTokenType::RETURN_KEYWORD, parse_return());
		CASE(WTokenType::YIELD_KEYWORD, parse_yield());
		CASE(WTokenType::ASSERT, parse_assert());
		CASE(WTokenType::IMPLORE, parse_implore());
		CASE(WTokenType::SWEAR, parse_swear());
		CASE(WTokenType::BREAK, parse_break());
		CASE(WTokenType::CONTINUE, parse_continue());
		CASE(WTokenType::REDO, parse_redo());
		CASE(WTokenType::WHILE, parse_while_loop());
		CASE(WTokenType::FOR, parse_for_in_loop());
		CASE(WTokenType::SCENARIO, parse_scenario());
		CASE(WTokenType::TEST, parse_test());
		CASE(WTokenType::ENUM, parse_enum_definition(is_public));
		CASE(WTokenType::FN, parse_function_definition(is_public));
		CASE(WTokenType::IMPORT, parse_import());
		CASE(WTokenType::NATIVE, parse_native());

	default:
	{
		RETREAT_PTR;
		return parse_expression_statement();
	}
	}
}

Statement_ptr Parser::parse_public_statement()
{
	auto token = token_pipe->current();
	OPT_CHECK(token);

	ADVANCE_PTR;

	const bool is_public = true;

	switch (token.value()->type)
	{
		CASE(WTokenType::LET, parse_variable_definition(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, parse_variable_definition(is_public, false));
		CASE(WTokenType::ENUM, parse_enum_definition(is_public));
		CASE(WTokenType::FN, parse_function_definition(is_public));

	default:
	{
		FATAL("UNEXPECTED_KEYWORD");
	}
	}
}

Statement_ptr Parser::parse_import()
{
	token_pipe->require(WTokenType::OPEN_CURLY_BRACE);

	StringVector goods;

	while (true)
	{
		auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
		goods.push_back(identifier->value);

		if (token_pipe->optional(WTokenType::CLOSE_CURLY_BRACE))
			break;

		token_pipe->require(WTokenType::COMMA);
	}

	token_pipe->require(WTokenType::FROM);

	auto path = token_pipe->require(WTokenType::STRING_LITERAL);
	token_pipe->require(WTokenType::EOL);

	return MAKE_STATEMENT(Import(goods, path->value));
}

Statement_ptr Parser::parse_native()
{
	auto native_identifier = token_pipe->require(WTokenType::IDENTIFIER);
	token_pipe->require(WTokenType::EOL);

	std::map<std::wstring, TypeNode_ptr> members;

	while (true)
	{
		auto [identifier, type] = consume_identifier_type_pair();
		members.insert({ identifier, type });

		if (token_pipe->optional(WTokenType::END))
			break;

		token_pipe->require(WTokenType::EOL);
	}

	return MAKE_STATEMENT(Native(native_identifier->value, members));
}

Statement_ptr Parser::parse_expression_statement()
{
	auto expression = parse_expression();
	token_pipe->require(WTokenType::EOL);

	return MAKE_STATEMENT(ExpressionStatement(move(expression)));
}

Statement_ptr Parser::parse_return()
{
	if (auto expression = parse_expression())
	{
		token_pipe->require(WTokenType::EOL);
		return MAKE_STATEMENT(Return(move(expression)));
	}

	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(Return());
}

Statement_ptr Parser::parse_yield()
{
	if (auto expression = parse_expression())
	{
		token_pipe->require(WTokenType::EOL);
		return MAKE_STATEMENT(YieldStatement(move(expression)));
	}

	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(YieldStatement());
}

Statement_ptr Parser::parse_assert()
{
	auto expression = parse_expression();
	NULL_CHECK(expression);

	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(Assert(move(expression)));
}

Statement_ptr Parser::parse_implore()
{
	auto expression = parse_expression();
	NULL_CHECK(expression);

	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(Implore(move(expression)));
}

Statement_ptr Parser::parse_swear()
{
	auto expression = parse_expression();
	NULL_CHECK(expression);

	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(Swear(move(expression)));
}

Statement_ptr Parser::parse_break()
{
	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(Break());
}

Statement_ptr Parser::parse_continue()
{
	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(Continue());
}

Statement_ptr Parser::parse_redo()
{
	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(Redo());
}