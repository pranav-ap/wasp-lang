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

#define ADVANCE_PTR token_pipe->advance_pointer()
#define RETREAT_PTR token_pipe->retreat_pointer()
#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define CASE(token_type, call) case token_type: { return call; }
#define MAKE_STATEMENT(x) std::make_shared<Statement>(x)
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)
#define MAKE_TYPE(x) std::make_shared<Type>(x)
#define MAKE_OPTIONAL_TYPE(x) std::make_shared<Type>(VariantType({ std::make_shared<Type>(x), std::make_shared<Type>(NoneType()) }))

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

Parser::Parser()
{
	register_parselet(WTokenType::IDENTIFIER, make_shared<IdentifierParselet>());
	register_parselet(WTokenType::STRING_LITERAL, make_shared<StringParselet>());
	register_parselet(WTokenType::NUMBER_LITERAL, make_shared<NumberParselet>());

	register_parselet(WTokenType::TRUE_KEYWORD, make_shared<BooleanParselet>());
	register_parselet(WTokenType::FALSE_KEYWORD, make_shared<BooleanParselet>());

	register_parselet(WTokenType::OPEN_PARENTHESIS, make_shared<CallParselet>());
	register_parselet(WTokenType::OPEN_PARENTHESIS, make_shared<SetParselet>());
	register_parselet(WTokenType::OPEN_SQUARE_BRACKET, make_shared<ListParselet>());
	register_parselet(WTokenType::OPEN_ANGLE_BRACKET, make_shared<MapParselet>());
	register_parselet(WTokenType::OPEN_FLOOR_BRACKET, make_shared<TupleParselet>());

	register_parselet(WTokenType::NEW, make_shared<UDTCreationParselet>());

	register_parselet(WTokenType::DOT, make_shared<UDTMemberAccessParselet>());
	register_parselet(WTokenType::QUESTION, make_shared<TernaryConditionParselet>());

	register_prefix(WTokenType::PLUS, Precedence::PREFIX);
	register_prefix(WTokenType::MINUS, Precedence::PREFIX);
	register_prefix(WTokenType::BANG, Precedence::PREFIX);

	register_infix_left(WTokenType::PLUS, Precedence::TERM);
	register_infix_left(WTokenType::MINUS, Precedence::TERM);

	register_infix_left(WTokenType::STAR, Precedence::PRODUCT);
	register_infix_left(WTokenType::DIVISION, Precedence::PRODUCT);
	register_infix_left(WTokenType::REMINDER, Precedence::PRODUCT);

	register_infix_left(WTokenType::EQUAL_EQUAL, Precedence::EQUALITY);
	register_infix_left(WTokenType::BANG_EQUAL, Precedence::EQUALITY);

	register_infix_left(WTokenType::LESSER_THAN, Precedence::COMPARISON);
	register_infix_left(WTokenType::LESSER_THAN_EQUAL, Precedence::COMPARISON);
	register_infix_left(WTokenType::GREATER_THAN, Precedence::COMPARISON);
	register_infix_left(WTokenType::GREATER_THAN_EQUAL, Precedence::COMPARISON);

	register_infix_left(WTokenType::AND, Precedence::LOGICAL);
	register_infix_left(WTokenType::OR, Precedence::LOGICAL);

	register_infix_right(WTokenType::POWER, Precedence::EXPONENT);
}

// Expression Parser

Expression_ptr Parser::parse_expression()
{
	return parse_expression(0);
}

Expression_ptr Parser::parse_expression(int precedence)
{
	token_pipe->ignore_whitespace();

	auto token = token_pipe->current();
	OPT_CHECK(token);

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
	infix_parselets.insert(make_pair(token_type, make_shared<BinaryOperatorParselet>((int)precedence, false)));
}

void Parser::register_infix_right(WTokenType token_type, Precedence precedence)
{
	infix_parselets.insert(make_pair(token_type, make_shared<BinaryOperatorParselet>((int)precedence, true)));
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

// Statement Parsers

Module_ptr Parser::execute(std::vector<Token_ptr>& tokens)
{
	token_pipe = std::make_shared<TokenPipe>(tokens);

	auto module_ast = std::make_shared<Module>();

	while ((size_t)token_pipe->get_current_index() < token_pipe->get_size())
	{
		Statement_ptr node = parse_statement(false);

		if (node)
		{
			module_ast->add_statement(move(node));
		}
	}

	return module_ast;
}

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
		CASE(WTokenType::ENUM, parse_enum_definition(is_public));

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
		CASE(WTokenType::ENUM, parse_enum_definition(is_public));
	default:
	{
		FATAL("UNEXPECTED_KEYWORD");
	}
	}
}

Statement_ptr Parser::parse_expression_statement()
{
	auto expression = parse_expression();
	token_pipe->expect(WTokenType::EOL);

	return MAKE_STATEMENT(ExpressionStatement(move(expression)));
}

Statement_ptr Parser::parse_enum_definition(bool is_public)
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	token_pipe->expect(WTokenType::EOL);
	vector<wstring> members = parse_enum_members(identifier->value);
	return MAKE_STATEMENT(EnumDefinition(is_public, identifier->value, members));
}

vector<wstring> Parser::parse_enum_members(wstring stem)
{
	vector<wstring> members;

	while (true)
	{
		token_pipe->ignore_whitespace();

		if (token_pipe->optional(WTokenType::END))
			break;

		if (token_pipe->optional(WTokenType::ENUM))
		{
			auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
			token_pipe->expect(WTokenType::EOL);

			auto children = parse_enum_members(stem + L"::" + identifier->value);
			members.insert(end(members), begin(children), end(children));

			continue;
		}

		auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
		members.push_back(stem + L"::" + identifier->value);

		token_pipe->expect(WTokenType::EOL);
	}

	return members;
}