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

Statement_ptr Parser::parse_enum_definition(bool is_public)
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	token_pipe->require(WTokenType::EOL);
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

Statement_ptr Parser::parse_variable_definition(bool is_public, bool is_mutable)
{
	auto expression = parse_expression();
	token_pipe->require(WTokenType::EOL);

	ASSERT(holds_alternative<Assignment>(*expression), "Must be an Assignment");
	auto assignment = get_if<Assignment>(&*expression);

	if (holds_alternative<TagPattern>(*assignment->lhs_expression), "Must be a TagPattern")
	{
		auto tag_pattern = get_if<TagPattern>(&*assignment->lhs_expression);
		return MAKE_STATEMENT(VariableDefinition(is_public, is_mutable, move(tag_pattern->tag), move(tag_pattern->expression), move(assignment->rhs_expression)));
	}

	return MAKE_STATEMENT(VariableDefinition(is_public, is_mutable, move(assignment->lhs_expression), move(assignment->rhs_expression)));
}

StringVector Parser::parse_comma_separated_identifiers()
{
	StringVector identifiers;

	while (true)
	{
		token_pipe->ignore_whitespace();

		auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
		NULL_CHECK(identifier);

		identifiers.push_back(identifier->value);

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		break;
	}

	return identifiers;
}

tuple<map<wstring, Expression_ptr>, std::map<std::wstring, bool>, StringVector, StringVector> Parser::parse_class_and_interface_definition()
{
	map<wstring, Expression_ptr> member_types;
	std::map<std::wstring, bool> is_public_member;

	StringVector interfaces;

	if (token_pipe->optional(WTokenType::TILDE))
	{
		interfaces = parse_comma_separated_identifiers();
	}

	StringVector base_types;

	if (token_pipe->optional(WTokenType::LESSER_THAN))
	{
		base_types = parse_comma_separated_identifiers();
	}

	while (true)
	{
		token_pipe->ignore_whitespace();

		if (token_pipe->optional(WTokenType::END))
			break;

		bool is_public = false;

		if (token_pipe->optional(WTokenType::PUB))
			is_public = true;

		auto [identifier, type] = consume_identifier_type_pair();
		token_pipe->require(WTokenType::EOL);

		const auto [_, success] = member_types.insert({ identifier, type });
		ASSERT(success, "Duplicate members are found!");

		const auto [_x, ok] = is_public_member.insert({ identifier, is_public });
		ASSERT(ok, "Duplicate members are found!");
	}

	return make_tuple(member_types, is_public_member, interfaces, base_types);
}

Statement_ptr Parser::parse_interface_definition(bool is_public)
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);

	auto [member_types, public_members, interfaces, base_types] = parse_class_and_interface_definition();
	return MAKE_STATEMENT(InterfaceDefinition(is_public, identifier->value, member_types, public_members, interfaces, base_types));
}

Statement_ptr Parser::parse_class_definition(bool is_public)
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);

	if (token_pipe->optional(WTokenType::EQUAL))
	{
		auto type = parse_type();
		token_pipe->require(WTokenType::EOL);
		return MAKE_STATEMENT(AliasDefinition(is_public, identifier->value, move(type)));
	}

	auto [member_types, public_members, interfaces, base_types] = parse_class_and_interface_definition();
	return MAKE_STATEMENT(ClassDefinition(is_public, identifier->value, member_types, public_members, interfaces, base_types));
}

tuple<wstring, wstring, bool, StringVector, ExpressionVector, optional<Expression_ptr>, Block> Parser::parse_callable_definition()
{
	auto first_identifier = token_pipe->require(WTokenType::IDENTIFIER);
	NULL_CHECK(first_identifier);

	bool is_method = false;
	Token_ptr second_identifier;

	if (token_pipe->optional(WTokenType::COLON_COLON))
	{
		is_method = true;
		second_identifier = token_pipe->require(WTokenType::IDENTIFIER);
	}

	token_pipe->require(WTokenType::OPEN_PARENTHESIS);

	StringVector arguments;
	ExpressionVector argument_types;

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

	optional<Expression_ptr> optional_return_type = std::nullopt;

	if (token_pipe->optional(WTokenType::ARROW))
	{
		auto return_type = parse_type();
		optional_return_type = std::make_optional(return_type);
	}

	token_pipe->require(WTokenType::EOL);

	Block body = parse_block();

	if (is_method)
	{
		return make_tuple(first_identifier->value, second_identifier->value, is_method, arguments, argument_types, optional_return_type, body);
	}

	return make_tuple(first_identifier->value, L"", is_method, arguments, argument_types, optional_return_type, body);
}

Statement_ptr Parser::parse_function_definition(bool is_public)
{
	auto [first_identifier, second_identifier, is_method, arguments, argument_types, optional_return_type, body] = parse_callable_definition();

	if (is_method)
	{
		Expression_ptr function_method_type = MAKE_EXPRESSION(FunctionMemberTypeNode(first_identifier, argument_types, optional_return_type));
		//second_identifier += stringify_type(function_type);
		return MAKE_STATEMENT(FunctionMemberDefinition(first_identifier, second_identifier, is_public, arguments, function_method_type, body));
	}

	Expression_ptr function_type = MAKE_EXPRESSION(FunctionTypeNode(argument_types, optional_return_type));
	//first_identifier += stringify_type(function_type);
	return MAKE_STATEMENT(FunctionDefinition(is_public, first_identifier, arguments, function_type, body));
}

Statement_ptr Parser::parse_generator_definition(bool is_public)
{
	auto [first_identifier, second_identifier, is_method, arguments, argument_types, optional_return_type, body] = parse_callable_definition();

	if (is_method)
	{
		Expression_ptr generator_method_type = MAKE_EXPRESSION(GeneratorMemberTypeNode(first_identifier, argument_types, optional_return_type));
		//second_identifier += stringify_type(function_type);
		return MAKE_STATEMENT(GeneratorMemberDefinition(first_identifier, second_identifier, is_public, arguments, generator_method_type, body));
	}

	Expression_ptr function_type = std::make_shared<Expression>(GeneratorTypeNode(argument_types, optional_return_type));
	//first_identifier += stringify_type(function_type);
	return MAKE_STATEMENT(GeneratorDefinition(is_public, first_identifier, arguments, function_type, body));
}
