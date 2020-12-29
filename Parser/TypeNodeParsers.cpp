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

Expression_ptr Parser::parse_type(bool is_optional)
{
	if (token_pipe->optional(WTokenType::OPT))
		is_optional = true;

	vector<Expression_ptr> types;

	while (true)
	{
		Expression_ptr type;

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
		else if (token_pipe->optional(WTokenType::GEN))
		{
			token_pipe->require(WTokenType::OPEN_PARENTHESIS);
			type = parse_generator_type(is_optional);
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
			return MAKE_EXPRESSION(VariantTypeNode(types));

		return move(types.front());
	}
}

Expression_ptr Parser::parse_list_type(bool is_optional)
{
	vector<Expression_ptr> types;

	while (true)
	{
		auto type = parse_type();
		types.push_back(type);

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		token_pipe->require(WTokenType::CLOSE_SQUARE_BRACKET);
		break;
	}

	if (is_optional)
	{
		return MAKE_OPTIONAL_EXPRESSION(ListTypeNode(move(types.front())));
	}

	return MAKE_EXPRESSION(ListTypeNode(move(types.front())));
}

Expression_ptr Parser::parse_set_type(bool is_optional)
{
	vector<Expression_ptr> types;

	while (true)
	{
		auto type = parse_type();
		types.push_back(type);

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		token_pipe->require(WTokenType::CLOSE_CURLY_BRACE);
		break;
	}

	if (is_optional)
	{
		return MAKE_OPTIONAL_EXPRESSION(ListTypeNode(move(types.front())));
	}

	return MAKE_EXPRESSION(ListTypeNode(move(types.front())));
}

Expression_ptr Parser::parse_tuple_type(bool is_optional)
{
	vector<Expression_ptr> types;

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
		return MAKE_OPTIONAL_EXPRESSION(TupleTypeNode(types));
	}

	return MAKE_EXPRESSION(TupleTypeNode(types));
}

Expression_ptr Parser::parse_map_type(bool is_optional)
{
	auto key_type = parse_type();
	token_pipe->require(WTokenType::ARROW);

	auto value_type = parse_type();
	token_pipe->require(WTokenType::CLOSE_ANGLE_BRACKET);

	if (is_optional)
	{
		return MAKE_OPTIONAL_EXPRESSION(MapTypeNode(move(key_type), move(value_type)));
	}

	return MAKE_EXPRESSION(MapTypeNode(move(key_type), move(value_type)));
}

tuple<ExpressionVector, optional<Expression_ptr>> Parser::parse_callable_type()
{
	ExpressionVector input_types;

	if (!token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
	{
		while (true)
		{
			auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
			NULL_CHECK(identifier);

			token_pipe->require(WTokenType::COLON);

			auto type = parse_type();
			NULL_CHECK(type);

			input_types.push_back(type);

			if (token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
				break;

			token_pipe->require(WTokenType::COMMA);
		}
	}

	std::optional<Expression_ptr> return_type = std::nullopt;

	if (token_pipe->optional(WTokenType::ARROW))
	{
		return_type = parse_type();
		NULL_CHECK(return_type);
	}

	return make_tuple(input_types, return_type);
}

Expression_ptr Parser::parse_function_type(bool is_optional)
{
	auto [input_types, return_type] = parse_callable_type();

	if (is_optional)
	{
		return MAKE_OPTIONAL_EXPRESSION(FunctionTypeNode(input_types, return_type));
	}

	return MAKE_EXPRESSION(FunctionTypeNode(input_types, return_type));
}

Expression_ptr Parser::parse_generator_type(bool is_optional)
{
	auto [input_types, return_type] = parse_callable_type();

	if (is_optional)
	{
		return MAKE_OPTIONAL_EXPRESSION(GeneratorTypeNode(input_types, return_type));
	}

	return MAKE_EXPRESSION(GeneratorTypeNode(input_types, return_type));
}

Expression_ptr Parser::consume_datatype_word(bool is_optional)
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
				? MAKE_OPTIONAL_EXPRESSION(IntLiteralTypeNode((int)value))
				: MAKE_EXPRESSION(IntLiteralTypeNode((int)value));
		}

		return is_optional
			? MAKE_OPTIONAL_EXPRESSION(FloatLiteralTypeNode(value))
			: MAKE_EXPRESSION(FloatLiteralTypeNode(value));
	}
	case WTokenType::STRING_LITERAL:
	{
		ADVANCE_PTR;
		return is_optional
			? MAKE_OPTIONAL_EXPRESSION(StringLiteralTypeNode(token.value()->value))
			: MAKE_EXPRESSION(StringLiteralTypeNode(token.value()->value));
	}
	case WTokenType::TRUE_KEYWORD:
	{
		ADVANCE_PTR;
		return is_optional
			? MAKE_OPTIONAL_EXPRESSION(BooleanLiteralTypeNode(true))
			: MAKE_EXPRESSION(BooleanLiteralTypeNode(true));
	}
	case WTokenType::FALSE_KEYWORD:
	{
		ADVANCE_PTR;
		return is_optional
			? MAKE_OPTIONAL_EXPRESSION(BooleanLiteralTypeNode(false))
			: MAKE_EXPRESSION(BooleanLiteralTypeNode(false));
	}
	case WTokenType::INT:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_EXPRESSION(IntTypeNode()) : MAKE_EXPRESSION(IntTypeNode());
	}
	case WTokenType::FLOAT:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_EXPRESSION(FloatTypeNode()) : MAKE_EXPRESSION(FloatTypeNode());
	}
	case WTokenType::STRING_KEYWORD:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_EXPRESSION(StringTypeNode()) : MAKE_EXPRESSION(StringTypeNode());
	}
	case WTokenType::BOOL:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_EXPRESSION(BooleanTypeNode()) : MAKE_EXPRESSION(BooleanTypeNode());
	}
	case WTokenType::ANY:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_EXPRESSION(AnyTypeNode()) : MAKE_EXPRESSION(AnyTypeNode());
	}
	case WTokenType::IDENTIFIER:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_EXPRESSION(TypeIdentifierTypeNode(token.value()->value)) : MAKE_EXPRESSION(TypeIdentifierTypeNode(token.value()->value));
	}
	}

	FATAL("EXPECTED_DATATYPE");
}

pair<wstring, Expression_ptr> Parser::consume_identifier_type_pair()
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	token_pipe->require(WTokenType::COLON);
	auto type = parse_type();

	return make_pair(identifier->value, move(type));
}

