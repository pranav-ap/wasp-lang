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

Parser::Parser()
{
	register_parselet(WTokenType::EQUAL, make_shared<AssignmentParselet>());
	register_parselet(WTokenType::IDENTIFIER, make_shared<IdentifierParselet>());
	register_parselet(WTokenType::STRING_LITERAL, make_shared<LiteralParselet>());
	register_parselet(WTokenType::NUMBER_LITERAL, make_shared<LiteralParselet>());
	register_parselet(WTokenType::TRUE_KEYWORD, make_shared<LiteralParselet>());
	register_parselet(WTokenType::FALSE_KEYWORD, make_shared<LiteralParselet>());
	register_parselet(WTokenType::NONE, make_shared<LiteralParselet>());

	register_parselet(WTokenType::OPEN_PARENTHESIS, make_shared<CallParselet>());
	register_parselet(WTokenType::OPEN_PARENTHESIS, make_shared<GroupParselet>());
	register_parselet(WTokenType::OPEN_SQUARE_BRACKET, make_shared<ListParselet>());
	register_parselet(WTokenType::OPEN_ANGLE_BRACKET, make_shared<MapParselet>());
	register_parselet(WTokenType::OPEN_FLOOR_BRACKET, make_shared<TupleParselet>());
	register_parselet(WTokenType::OPEN_CURLY_BRACE, make_shared<SetParselet>());
	
	register_parselet(WTokenType::NEW, make_shared<NewParselet>());
	register_parselet(WTokenType::COLON, make_shared<TagPatternParselet>());
	register_parselet(WTokenType::IF, make_shared<TernaryConditionParselet>());
	register_parselet(WTokenType::COLON_COLON, make_shared<EnumMemberParselet>());
	register_parselet(WTokenType::DOT_DOT_DOT, make_shared<SpreadParselet>());
	register_parselet(WTokenType::DOT, make_shared<MemberAccessParselet>());
	register_parselet(WTokenType::QUESTION_DOT, make_shared<MemberAccessParselet>());

	register_prefix(WTokenType::PLUS, Precedence::PREFIX);
	register_prefix(WTokenType::MINUS, Precedence::PREFIX);
	register_prefix(WTokenType::BANG, Precedence::PREFIX);
	register_prefix(WTokenType::TYPE_OF, Precedence::PREFIX);
	register_prefix(WTokenType::AT_SIGN, Precedence::PREFIX);

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
	register_infix_left(WTokenType::IN_KEYWORD, Precedence::COMPARISON);
	register_infix_left(WTokenType::IS, Precedence::COMPARISON);
	register_infix_left(WTokenType::QUESTION_QUESTION, Precedence::COALESE);
	register_infix_left(WTokenType::AND, Precedence::AND);
	register_infix_left(WTokenType::OR, Precedence::OR);

	register_infix_right(WTokenType::POWER, Precedence::EXPONENT);
	register_infix_right(WTokenType::EQUAL, Precedence::ASSIGNMENT);
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

// Statement Parsers

Module_ptr Parser::execute(std::vector<Token_ptr>& tokens)
{
	token_pipe = std::make_shared<TokenPipe>(tokens);

	auto module_ast = std::make_shared<Module>();

	while ((size_t)token_pipe->get_current_index() < token_pipe->get_size())
	{
		Statement_ptr node = parse_statement();

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
		CASE(WTokenType::INTERFACE, parse_interface_definition(is_public));
		CASE(WTokenType::TYPE, parse_class_definition(is_public));
		CASE(WTokenType::FN, parse_function_definition(is_public));
		CASE(WTokenType::GEN, parse_generator_definition(is_public));

		CASE(WTokenType::NAMESPACE, parse_namespace(is_public));

		CASE(WTokenType::IF, parse_branching(token.value()->type));
		CASE(WTokenType::WHILE, parse_while_loop());
		CASE(WTokenType::FOR, parse_for_in_loop());

		CASE(WTokenType::LET, parse_variable_definition(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, parse_variable_definition(is_public, false));

		CASE(WTokenType::RETURN_KEYWORD, parse_return());
		CASE(WTokenType::YIELD_KEYWORD, parse_yield());
		CASE(WTokenType::ASSERT, parse_assert());
		CASE(WTokenType::IMPLORE, parse_implore());
		CASE(WTokenType::SWEAR, parse_swear());
		CASE(WTokenType::BREAK, parse_break());
		CASE(WTokenType::CONTINUE, parse_continue());
		CASE(WTokenType::REDO, parse_redo());

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
		CASE(WTokenType::INTERFACE, parse_interface_definition(is_public));
		CASE(WTokenType::ENUM, parse_enum_definition(is_public));
		CASE(WTokenType::TYPE, parse_class_definition(is_public));
		CASE(WTokenType::FN, parse_function_definition(is_public));
		CASE(WTokenType::GEN, parse_generator_definition(is_public));

		CASE(WTokenType::NAMESPACE, parse_namespace(is_public));

		CASE(WTokenType::LET, parse_variable_definition(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, parse_variable_definition(is_public, false));

	default:
	{
		FATAL("UNEXPECTED_KEYWORD");
	}
	}
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

