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


TypeNode_ptr Parser::parse_type(bool is_optional)
{
	if (token_pipe->optional(WTokenType::OPT))
		is_optional = true;

	vector<TypeNode_ptr> types;

	while (true)
	{
		TypeNode_ptr type;

		if (token_pipe->optional(WTokenType::OPEN_SQUARE_BRACKET))
		{
			type = parse_list_type(is_optional);
		}
		else if (token_pipe->optional(WTokenType::OPEN_ANGLE_BRACKET))
		{
			type = parse_map_type(is_optional);
		}
		else if (token_pipe->optional(WTokenType::OPEN_CURLY_BRACE))
		{
			type = parse_set_type(is_optional);
		}
		else if (token_pipe->optional(WTokenType::OPEN_FLOOR_BRACKET))
		{
			type = parse_tuple_type(is_optional);
		}
		else if (token_pipe->optional(WTokenType::OPEN_PARENTHESIS))
		{
			type = parse_function_type(is_optional);
		}
		else if (token_pipe->optional(WTokenType::FN))
		{
			token_pipe->require(WTokenType::OPEN_PARENTHESIS);
			type = parse_function_type(is_optional);
		}
		else
		{
			type = consume_datatype_word(is_optional);
		}

		types.push_back(type);

		if (token_pipe->optional(WTokenType::VERTICAL_BAR))
		{
			continue;
		}

		if (types.size() > 1)
			return MAKE_TYPE(VariantTypeNode(types));

		return move(types.front());
	}
}

TypeNode_ptr Parser::parse_list_type(bool is_optional)
{
	auto type = parse_type();
	token_pipe->require(WTokenType::CLOSE_SQUARE_BRACKET);

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(ListTypeNode(move(type)));
	}

	return MAKE_TYPE(ListTypeNode(move(type)));
}

TypeNode_ptr Parser::parse_set_type(bool is_optional)
{
	auto type = parse_type();
	token_pipe->require(WTokenType::CLOSE_CURLY_BRACE);

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(SetTypeNode(move(type)));
	}

	return MAKE_TYPE(SetTypeNode(move(type)));
}

TypeNode_ptr Parser::parse_tuple_type(bool is_optional)
{
	vector<TypeNode_ptr> types;

	while (true)
	{
		auto type = parse_type();
		types.push_back(type);

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		token_pipe->require(WTokenType::CLOSE_FLOOR_BRACKET);
		break;
	}

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(TupleTypeNode(types));
	}

	return MAKE_TYPE(TupleTypeNode(types));
}

TypeNode_ptr Parser::parse_map_type(bool is_optional)
{
	auto key_type = parse_type();
	token_pipe->require(WTokenType::ARROW);

	auto value_type = parse_type();
	token_pipe->require(WTokenType::CLOSE_ANGLE_BRACKET);

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(MapTypeNode(move(key_type), move(value_type)));
	}

	return MAKE_TYPE(MapTypeNode(move(key_type), move(value_type)));
}

TypeNode_ptr Parser::consume_datatype_word(bool is_optional)
{
	auto token = token_pipe->current();
	OPT_CHECK(token);

	switch (token.value()->type)
	{
	case WTokenType::NUMBER_LITERAL:
	{
		ADVANCE_PTR;

		auto value = std::stod(token.value()->value);

		if (std::fmod(value, 1.0) == 0.0)
		{
			return is_optional
				? MAKE_OPTIONAL_TYPE(IntLiteralTypeNode((int)value))
				: MAKE_TYPE(IntLiteralTypeNode((int)value));
		}

		return is_optional
			? MAKE_OPTIONAL_TYPE(FloatLiteralTypeNode(value))
			: MAKE_TYPE(FloatLiteralTypeNode(value));
	}
	case WTokenType::STRING_LITERAL:
	{
		ADVANCE_PTR;
		return is_optional
			? MAKE_OPTIONAL_TYPE(StringLiteralTypeNode(token.value()->value))
			: MAKE_TYPE(StringLiteralTypeNode(token.value()->value));
	}
	case WTokenType::TRUE_KEYWORD:
	{
		ADVANCE_PTR;
		return is_optional
			? MAKE_OPTIONAL_TYPE(BooleanLiteralTypeNode(true))
			: MAKE_TYPE(BooleanLiteralTypeNode(true));
	}
	case WTokenType::FALSE_KEYWORD:
	{
		ADVANCE_PTR;
		return is_optional
			? MAKE_OPTIONAL_TYPE(BooleanLiteralTypeNode(false))
			: MAKE_TYPE(BooleanLiteralTypeNode(false));
	}
	case WTokenType::INT:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(IntTypeNode()) : MAKE_TYPE(IntTypeNode());
	}
	case WTokenType::FLOAT:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(FloatTypeNode()) : MAKE_TYPE(FloatTypeNode());
	}
	case WTokenType::STRING_KEYWORD:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(StringTypeNode()) : MAKE_TYPE(StringTypeNode());
	}
	case WTokenType::BOOL:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(BooleanTypeNode()) : MAKE_TYPE(BooleanTypeNode());
	}
	case WTokenType::ANY:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(AnyTypeNode()) : MAKE_TYPE(AnyTypeNode());
	}
	case WTokenType::IDENTIFIER:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(TypeIdentifierNode(token.value()->value)) : MAKE_TYPE(TypeIdentifierNode(token.value()->value));
	}
	}

	FATAL("EXPECTED_DATATYPE");
}

tuple<TypeNodeVector, optional<TypeNode_ptr>> Parser::parse_callable_type()
{
	TypeNodeVector input_types;

	if (!token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
	{
		while (true)
		{
			auto [identifier, type] = consume_identifier_type_pair();
			input_types.push_back(type);

			if (token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
				break;

			token_pipe->require(WTokenType::COMMA);
		}
	}

	std::optional<TypeNode_ptr> return_type = std::nullopt;

	if (token_pipe->optional(WTokenType::ARROW))
	{
		return_type = parse_type();
		OPT_CHECK(return_type);
	}

	return make_tuple(input_types, return_type);
}

TypeNode_ptr Parser::parse_function_type(bool is_optional)
{
	auto [input_types, return_type] = parse_callable_type();

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(FunctionTypeNode(input_types, return_type));
	}

	return MAKE_TYPE(FunctionTypeNode(input_types, return_type));
}
