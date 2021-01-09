#pragma once
#include "pch.h"
#include "ExpressionParselets.h"
#include "Parser.h"
#include "Assertion.h"
#include "utils.h"

#include <cmath>
#include <vector>
#include <tuple>
#include <stack>
#include <map>
#include <memory>
#include <utility>
#include <optional>
#include <variant>

#define ADVANCE_PTR parser->token_pipe->advance_pointer()
#define RETREAT_PTR parser->token_pipe->retreat_pointer()
#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
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

Expression_ptr IdentifierParselet::parse(Parser_ptr parser, Token_ptr token)
{
	ADVANCE_PTR;
	return MAKE_EXPRESSION(Identifier(token->value));
}

Expression_ptr LiteralParselet::parse(Parser_ptr parser, Token_ptr token)
{
	ADVANCE_PTR;

	switch (token->type)
	{
	case WTokenType::TRUE_KEYWORD:
	{
		return MAKE_EXPRESSION(true);
	}
	case WTokenType::FALSE_KEYWORD:
	{
		return MAKE_EXPRESSION(false);
	}
	case WTokenType::STRING_LITERAL:
	{
		return MAKE_EXPRESSION(token->value);
	}
	case WTokenType::NUMBER_LITERAL:
	{
		auto value = stod(token->value);

		if (std::fmod(value, 1.0) == 0.0)
		{
			return MAKE_EXPRESSION((int)value);
		}

		return MAKE_EXPRESSION(value);
	}
	default:
	{
		FATAL("Token has to be boolean");
	}
	}
}

Expression_ptr PrefixOperatorParselet::parse(Parser_ptr parser, Token_ptr token)
{
	ADVANCE_PTR;
	Expression_ptr right = parser->parse_expression();

	if (token->type == WTokenType::DOT_DOT_DOT)
	{
		return MAKE_EXPRESSION(Spread(right));
	}

	return MAKE_EXPRESSION(Prefix(token, right));
}

Expression_ptr InfixOperatorParselet::parse(Parser_ptr parser, Expression_ptr left, Token_ptr token)
{
	auto right = parser->parse_expression(precedence - (is_right_associative ? 1 : 0));
	return MAKE_EXPRESSION(Infix(left, token, right));
}

Expression_ptr PostfixOperatorParselet::parse(Parser_ptr parser, Expression_ptr left, Token_ptr token)
{
	ADVANCE_PTR;
	return MAKE_EXPRESSION(Postfix(left, token));
}

Expression_ptr GroupParselet::parse(Parser_ptr parser, Token_ptr token)
{
	ADVANCE_PTR;
	Expression_ptr expression = parser->parse_expression();
	parser->token_pipe->require(WTokenType::CLOSE_PARENTHESIS);
	return expression;
}

Expression_ptr ListParselet::parse(Parser_ptr parser, Token_ptr token)
{
	ADVANCE_PTR;
	ExpressionVector expressions = parser->parse_expressions();
	parser->token_pipe->require(WTokenType::CLOSE_SQUARE_BRACKET);
	return MAKE_EXPRESSION(ListLiteral(expressions));
}

Expression_ptr TupleParselet::parse(Parser_ptr parser, Token_ptr token)
{
	ADVANCE_PTR;
	ExpressionVector expressions = parser->parse_expressions();
	parser->token_pipe->require(WTokenType::CLOSE_FLOOR_BRACKET);
	return MAKE_EXPRESSION(TupleLiteral(expressions));
}

Expression_ptr SetParselet::parse(Parser_ptr parser, Token_ptr token)
{
	ADVANCE_PTR;
	ExpressionVector expressions = parser->parse_expressions();
	parser->token_pipe->require(WTokenType::CLOSE_CURLY_BRACE);
	return MAKE_EXPRESSION(SetLiteral(expressions));
}

Expression_ptr MapParselet::parse(Parser_ptr parser, Token_ptr token)
{
	ADVANCE_PTR;

	map<Expression_ptr, Expression_ptr> pairs;

	if (parser->token_pipe->optional(WTokenType::CLOSE_ANGLE_BRACKET))
		return MAKE_EXPRESSION(MapLiteral(pairs));

	while (true)
	{
		parser->token_pipe->ignore_whitespace();

		auto key = parser->parse_expression();
		auto value = (parser->token_pipe->optional(WTokenType::ARROW)) ? parser->parse_expression() : key;
		pairs.insert_or_assign(key, value);


		if (parser->token_pipe->optional(WTokenType::CLOSE_ANGLE_BRACKET))
			return MAKE_EXPRESSION(MapLiteral(pairs));

		parser->token_pipe->require(WTokenType::COMMA);		
	}
}

Expression_ptr AssignmentParselet::parse(Parser_ptr parser, Expression_ptr left, Token_ptr token)
{
	Expression_ptr right = parser->parse_expression((int)Precedence::ASSIGNMENT - 1);

	if (holds_alternative<TypePattern>(*left))
	{
		auto type_pattern = get_if<TypePattern>(&*left);
		auto lhs = type_pattern->expression;
		auto type_node = type_pattern->type_node;

		return MAKE_EXPRESSION(TypedAssignment(lhs, right, type_node));
	}

	return MAKE_EXPRESSION(UntypedAssignment(left, right));
}

Expression_ptr TypePatternParselet::parse(Parser_ptr parser, Expression_ptr left, Token_ptr token)
{
	TypeNode_ptr type = parser->parse_type();
	return MAKE_EXPRESSION(TypePattern(left, type));
}

Expression_ptr TernaryConditionParselet::parse(Parser_ptr parser, Token_ptr token)
{
	ADVANCE_PTR;

	Expression_ptr condition = parser->parse_expression();

	parser->token_pipe->require(WTokenType::THEN);
	Expression_ptr then_arm = parser->parse_expression();

	parser->token_pipe->require(WTokenType::ELSE);
	Expression_ptr else_arm = parser->parse_expression((int)Precedence::TERNARY_CONDITION - 1);

	return MAKE_EXPRESSION(TernaryCondition(condition, then_arm, else_arm));
}

Expression_ptr CallParselet::parse(Parser_ptr parser, Identifier* identifier)
{
	ExpressionVector arguments;

	if (parser->token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
	{
		return MAKE_EXPRESSION(Call(identifier->name));
	}

	arguments = parser->parse_expressions();
	parser->token_pipe->require(WTokenType::CLOSE_PARENTHESIS);

	return MAKE_EXPRESSION(Call(identifier->name, arguments));
}

Expression_ptr CallParselet::parse(Parser_ptr parser, Expression_ptr left, Token_ptr token)
{
	Expression_ptr result;

	std::visit(overloaded{
		[&](Identifier& expr)
		{
			result = parse(parser, &expr);
		},
		[&](auto)
		{
			FATAL("Function Call syntax error");
		}
		}, *left);

	return result;
}

Expression_ptr EnumMemberParselet::parse(Parser_ptr parser, Expression_ptr left, Token_ptr token)
{
	ASSERT(holds_alternative<Identifier>(*left), "Expect enum member name");
	auto identifier = get_if<Identifier>(&*left);

	std::vector<std::wstring> chain = { identifier->name };

	while (auto member_identifier = parser->token_pipe->require(WTokenType::IDENTIFIER))
	{
		chain.push_back(member_identifier->value);

		if (!parser->token_pipe->optional(WTokenType::COLON_COLON))
			break;
	}

	return MAKE_EXPRESSION(EnumMember(chain, concat(chain, L"::")));
}

// get_precedence

int PrefixOperatorParselet::get_precedence()
{
	return precedence;
}

int InfixOperatorParselet::get_precedence()
{
	return precedence;
}

int PostfixOperatorParselet::get_precedence()
{
	return precedence;
}

int AssignmentParselet::get_precedence()
{
	return (int)Precedence::ASSIGNMENT;
}

int TypePatternParselet::get_precedence()
{
	return (int)Precedence::TYPE_PATTERN;
}

int TernaryConditionParselet::get_precedence()
{
	return (int)Precedence::TERNARY_CONDITION;
}

int CallParselet::get_precedence()
{
	return (int)Precedence::CALL;
}

int EnumMemberParselet::get_precedence()
{
	return (int)Precedence::MEMBER_ACCESS;
}
