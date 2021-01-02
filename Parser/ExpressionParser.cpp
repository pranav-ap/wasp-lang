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


Expression_ptr Parser::parse_expression()
{
	return parse_expression(0);
}

Expression_ptr Parser::parse_expression(int precedence)
{
	token_pipe->ignore_whitespace();

	auto token = token_pipe->current();
	OPT_CHECK(token);

	if (token.value()->type == WTokenType::CLOSE_PARENTHESIS)
	{
		return nullptr;
	}

	IPrefixParselet_ptr prefix_parselet = prefix_parselets.at(token.value()->type);
	NULL_CHECK(prefix_parselet);

	Expression_ptr left = prefix_parselet->parse(this, token.value());
	NULL_CHECK(left);

	while (precedence < get_next_operator_precedence())
	{
		token = token_pipe->current();
		OPT_CHECK(token);

		ADVANCE_PTR;

		IInfixParselet_ptr infix_parselet = infix_parselets.at(token.value()->type);
		left = infix_parselet->parse(this, left, token.value());
	}

	return left;
}

ExpressionVector Parser::parse_expressions()
{
	ExpressionVector elements;

	while (auto element = parse_expression())
	{
		elements.push_back(move(element));

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		break;
	}

	return elements;
}

void Parser::register_parselet(WTokenType token_type, IPrefixParselet_ptr parselet)
{
	prefix_parselets.insert(make_pair(token_type, parselet));
}

void Parser::register_parselet(WTokenType token_type, IInfixParselet_ptr parselet)
{
	infix_parselets.insert(make_pair(token_type, parselet));
}

void Parser::register_prefix(WTokenType token_type, Precedence precedence)
{
	prefix_parselets.insert(make_pair(token_type, make_shared<PrefixOperatorParselet>((int)precedence)));
}

void Parser::register_infix_left(WTokenType token_type, Precedence precedence)
{
	infix_parselets.insert(make_pair(token_type, make_shared<InfixOperatorParselet>((int)precedence, false)));
}

void Parser::register_infix_right(WTokenType token_type, Precedence precedence)
{
	infix_parselets.insert(make_pair(token_type, make_shared<InfixOperatorParselet>((int)precedence, true)));
}

void Parser::register_postfix(WTokenType token_type, Precedence precedence)
{
	infix_parselets.insert(make_pair(token_type, make_shared<PostfixOperatorParselet>((int)precedence)));
}

int Parser::get_next_operator_precedence()
{
	auto token = token_pipe->current();

	if (token.has_value() && infix_parselets.contains(token.value()->type))
	{
		IInfixParselet_ptr infix_parselet = infix_parselets.at(token.value()->type);
		return infix_parselet->get_precedence();
	}

	return 0;
}
