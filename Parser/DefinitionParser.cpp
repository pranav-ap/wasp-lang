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

// Function

std::tuple<StringVector, TypeNodeVector, std::optional<TypeNode_ptr>, Block> Parser::parse_callable_definition()
{
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
	return make_tuple(arguments, argument_types, optional_return_type, body);
}

Statement_ptr Parser::parse_function_definition(bool is_public)
{
	auto function_name_token = token_pipe->require(WTokenType::IDENTIFIER);
	NULL_CHECK(function_name_token);
	auto function_name = function_name_token->value;

	auto [arguments, argument_types, optional_return_type, body] = parse_callable_definition();

	TypeNode_ptr function_type = MAKE_TYPE(FunctionTypeNode(argument_types, optional_return_type));
	return MAKE_STATEMENT(FunctionDefinition(is_public, function_name, arguments, function_type, body));
}

std::pair<std::wstring, TypeNode_ptr> Parser::consume_identifier_type_pair()
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	token_pipe->require(WTokenType::COLON);
	auto type = parse_type();

	return make_pair(identifier->value, move(type));
}

std::pair<StringVector, StringVector> Parser::parse_inheritance()
{
	StringVector parent_classes;

	if (token_pipe->optional(WTokenType::LESSER_THAN))
	{
		parent_classes = this->parse_comma_separated_identifiers();
	}

	StringVector interfaces;

	if (token_pipe->optional(WTokenType::TILDE))
	{
		interfaces = this->parse_comma_separated_identifiers();
	}

	return make_pair(parent_classes, interfaces);
}

Statement_ptr Parser::parse_type_definition(bool is_public)
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	auto type_name = identifier->value;

	if (token_pipe->optional(WTokenType::EQUAL))
	{
		auto ref_type = this->parse_type();
		token_pipe->require(WTokenType::EOL);

		TypeNode_ptr alias_type = MAKE_TYPE(AliasTypeNode(type_name, ref_type));
		return MAKE_STATEMENT(AliasDefinition(is_public, type_name, ref_type, alias_type));
	}

	token_pipe->require(WTokenType::EOL);

	std::map<std::wstring, TypeNode_ptr> member_types;
	std::map<std::wstring, Block> function_body_map;
	std::map<std::wstring, StringVector> function_argument_names_map;

	while (true)
	{
		if (token_pipe->optional(WTokenType::END))
			break;
		
		if (token_pipe->optional(WTokenType::FN))
		{
			auto function_name_token = token_pipe->require(WTokenType::IDENTIFIER);
			NULL_CHECK(function_name_token);
			auto function_name = function_name_token->value;

			auto [arguments, argument_types, optional_return_type, body] = parse_callable_definition();
			TypeNode_ptr function_type = MAKE_TYPE(FunctionTypeNode(argument_types, optional_return_type));
			
			member_types.insert({ function_name, function_type });
			function_body_map.insert({ function_name, body });
			function_argument_names_map.insert({ function_name, arguments });
		}
		else 
		{
			auto [name, type_node] = this->consume_identifier_type_pair();
			member_types.insert({ name, type_node });
			token_pipe->require(WTokenType::EOL);
		}
	}

	auto [parent_classes, interfaces] = this->parse_inheritance();
	TypeNode_ptr class_type = MAKE_TYPE(ClassTypeNode(type_name, member_types, parent_classes, interfaces));

	return MAKE_STATEMENT(ClassDefinition(is_public, type_name, member_types, function_body_map, function_argument_names_map, parent_classes, interfaces, class_type));
}
