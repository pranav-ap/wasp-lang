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

Statement_ptr Parser::parse_variable_definition(bool is_public, bool is_mutable)
{
	auto expression = parse_expression();
	token_pipe->require(WTokenType::EOL);

	if (holds_alternative<UntypedAssignment>(*expression))
	{
		auto [lhs, rhs] = deconstruct_UntypedAssignment(expression);

		if (holds_alternative<Identifier>(*lhs))
		{
			auto identifier = get_if<Identifier>(&*lhs);
			return MAKE_STATEMENT(SingleVariableDefinition(is_public, is_mutable, identifier->name, move(rhs)));
		}

		return MAKE_STATEMENT(DeconstructedVariableDefinition(is_public, is_mutable, lhs, move(rhs)));
	}

	ASSERT(holds_alternative<TypedAssignment>(*expression), "Must be an TypedAssignment");
	auto [lhs, rhs, type_node] = deconstruct_TypedAssignment(expression);

	if (holds_alternative<Identifier>(*lhs))
	{
		auto identifier = get_if<Identifier>(&*lhs);
		return MAKE_STATEMENT(SingleVariableDefinition(is_public, is_mutable, identifier->name, move(rhs), type_node));
	}

	return MAKE_STATEMENT(DeconstructedVariableDefinition(is_public, is_mutable, lhs, move(rhs), type_node));
}

// Enum

Statement_ptr Parser::parse_enum_definition(bool is_public)
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	token_pipe->require(WTokenType::EOL);
	vector<wstring> members = parse_enum_members(identifier->value);
	return MAKE_STATEMENT(EnumDefinition(is_public, identifier->value, members));
}

std::vector<std::wstring> Parser::parse_enum_members(std::wstring stem)
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
			token_pipe->require(WTokenType::EOL);
			members.push_back(stem + L"::" + identifier->value);

			auto children = parse_enum_members(stem + L"::" + identifier->value);
			members.insert(end(members), begin(children), end(children));

			continue;
		}

		auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
		members.push_back(stem + L"::" + identifier->value);

		token_pipe->require(WTokenType::EOL);
	}

	return members;
}

// Func and Gen

std::tuple<std::wstring, StringVector, TypeNodeVector, std::optional<TypeNode_ptr>, Block> Parser::parse_callable_definition()
{
	auto first_identifier = token_pipe->require(WTokenType::IDENTIFIER);
	NULL_CHECK(first_identifier);

	token_pipe->require(WTokenType::OPEN_PARENTHESIS);

	StringVector arguments;
	TypeNodeVector argument_types;

	if (!token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
	{
		while (true)
		{
			auto [identifier, type] = consume_identifier_type_pair();
			arguments.push_back(identifier);
			argument_types.push_back(type);

			if (token_pipe->optional(WTokenType::COMMA))
				continue;

			token_pipe->require(WTokenType::CLOSE_PARENTHESIS);
			break;
		}
	}

	optional<TypeNode_ptr> optional_return_type = std::nullopt;

	if (token_pipe->optional(WTokenType::ARROW))
	{
		auto return_type = parse_type();
		optional_return_type = std::make_optional(return_type);
	}

	token_pipe->require(WTokenType::EOL);
	Block body = parse_block();
	return make_tuple(first_identifier->value, arguments, argument_types, optional_return_type, body);
}

Statement_ptr Parser::parse_function_definition(bool is_public)
{
	auto [name, arguments, argument_types, optional_return_type, body] = parse_callable_definition();

	TypeNode_ptr function_type = MAKE_TYPE(FunctionTypeNode(argument_types, optional_return_type));
	return MAKE_STATEMENT(FunctionDefinition(is_public, name, arguments, function_type, body));
}

std::pair<std::wstring, TypeNode_ptr> Parser::consume_identifier_type_pair()
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	token_pipe->require(WTokenType::COLON);
	auto type = parse_type();

	return make_pair(identifier->value, move(type));
}
