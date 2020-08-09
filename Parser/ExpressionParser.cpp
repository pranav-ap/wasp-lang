#pragma once
#include "pch.h"
#include "ExpressionParser.h"
#include "Assertion.h"

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <variant>

#define ADVANCE_PTR token_pipe->advance_pointer()
#define RETREAT_PTR token_pipe->retreat_pointer()
#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::vector;
using std::string;
using std::map;
using std::make_shared;
using std::shared_ptr;
using std::move;

Expression_ptr ExpressionParser::parse_expression()
{
	while (true)
	{
		auto current = token_pipe->current();

		if (!current.has_value())
			break;

		switch (current.value()->type)
		{
			// SIMPLE

		case TokenType::NUMBER_LITERAL:
		{
			ast.push(MAKE_EXPRESSION(stod(current.value()->value)));
			ADVANCE_PTR;
			break;
		}
		case TokenType::STRING_LITERAL:
		{
			ast.push(MAKE_EXPRESSION(current.value()->value));
			ADVANCE_PTR;
			break;
		}
		case TokenType::TRUE_KEYWORD:
		{
			ast.push(MAKE_EXPRESSION(true));
			ADVANCE_PTR;
			break;
		}
		case TokenType::FALSE_KEYWORD:
		{
			ast.push(MAKE_EXPRESSION(false));
			ADVANCE_PTR;
			break;
		}
		case TokenType::IDENTIFIER:
		{
			ADVANCE_PTR;
			auto expression = parse_identifier(current.value());
			ast.push(expression);
			break;
		}
		case TokenType::DOT:
		{
			if (context_stack.size() == 0 || context_stack.top() != ExpressionContext::MEMBER_ACCESS)
			{
				push_context(ExpressionContext::MEMBER_ACCESS);
				ADVANCE_PTR;
				auto expression = parse_member_access();
				ast.push(expression);
				break;
			}

			return finish_parsing();
		}

		// UDT LITERAL

		case TokenType::NEW:
		{
			push_context(ExpressionContext::UDT_CREATION);
			ADVANCE_PTR;
			auto value = parse_UDT_creation();
			ast.push(move(value));
			break;
		}

		// MAP_LITERAL

		case TokenType::OPEN_ANGLE_BRACKET:
		{
			push_context(ExpressionContext::MAP_LITERAL);
			ADVANCE_PTR;
			auto value = parse_map_literal();
			ast.push(move(value));
			break;
		}
		case TokenType::CLOSE_ANGLE_BRACKET:
		{
			pop_context(ExpressionContext::MAP_LITERAL);
			return finish_parsing();
		}

		// LIST LITERAL

		case TokenType::OPEN_SQUARE_BRACKET:
		{
			push_context(ExpressionContext::LIST_LITERAL);
			ADVANCE_PTR;
			auto value = parse_list_literal();
			ast.push(move(value));
			break;
		}
		case TokenType::CLOSE_SQUARE_BRACKET:
		{
			pop_context(ExpressionContext::LIST_LITERAL);
			return finish_parsing();
		}

		// TUPLE LITERAL

		case TokenType::OPEN_FLOOR_BRACKET:
		{
			push_context(ExpressionContext::TUPLE_LITERAL);
			ADVANCE_PTR;
			auto value = parse_tuple_literal();
			ast.push(move(value));
			break;
		}
		case TokenType::CLOSE_FLOOR_BRACKET:
		{
			pop_context(ExpressionContext::TUPLE_LITERAL);
			return finish_parsing();
		}

		// FUNCTION CALL

		case TokenType::CALLABLE_IDENTIFIER:
		{
			push_context(ExpressionContext::FUNCTION_CALL);
			ADVANCE_PTR;
			auto arguments = parse_function_call_arguments();
			ast.push(MAKE_EXPRESSION(Call(current.value()->value, arguments)));
			break;
		}
		case TokenType::CLOSE_PARENTHESIS:
		{
			if (context_stack.top() == ExpressionContext::FUNCTION_CALL)
			{
				pop_context(ExpressionContext::FUNCTION_CALL);
				return finish_parsing();
			}
			else if (context_stack.top() == ExpressionContext::UDT_CREATION)
			{
				pop_context(ExpressionContext::UDT_CREATION);
				return finish_parsing();
			}

			operator_stack->drain_into_ast_until_open_parenthesis(ast);
			pop_context(ExpressionContext::PARENTHESIS);
			return finish_parsing();
		}

		// OPERATORS

		case TokenType::OPEN_PARENTHESIS:
		{
			push_context(ExpressionContext::PARENTHESIS);
			operator_stack->dumb_push(move(current.value()));
			ADVANCE_PTR;
			break;
		}
		case TokenType::BANG:
		case TokenType::UNARY_MINUS:
		case TokenType::UNARY_PLUS:
		case TokenType::PLUS:
		case TokenType::MINUS:
		case TokenType::DIVISION:
		case TokenType::REMINDER:
		case TokenType::STAR:
		case TokenType::POWER:
		case TokenType::GREATER_THAN:
		case TokenType::GREATER_THAN_EQUAL:
		case TokenType::LESSER_THAN:
		case TokenType::LESSER_THAN_EQUAL:
		case TokenType::EQUAL_EQUAL:
		case TokenType::BANG_EQUAL:
		case TokenType::AND:
		case TokenType::OR:
		{
			operator_stack->smart_push(move(current.value()), ast);
			ADVANCE_PTR;
			break;
		}

		// OTHER

		case TokenType::SPACE:
		{
			ADVANCE_PTR;
			break;
		}
		case TokenType::EOL:
		case TokenType::COLON:
		case TokenType::COMMA:
		case TokenType::EQUAL:
		{
			return finish_parsing();
		}
		}
	}

	return finish_parsing();
}

ExpressionVector ExpressionParser::parse_expressions()
{
	ExpressionVector elements;

	while (auto element = parse_expression())
	{
		elements.push_back(move(element));

		if (token_pipe->optional(TokenType::COMMA))
			continue;

		break;
	}

	return elements;
}

Expression_ptr ExpressionParser::parse_list_literal()
{
	ExpressionVector elements;

	if (token_pipe->optional(TokenType::CLOSE_SQUARE_BRACKET))
	{
		return MAKE_EXPRESSION(ListLiteral(elements));
	}

	elements = parse_expressions();

	token_pipe->expect(TokenType::CLOSE_SQUARE_BRACKET);
	return MAKE_EXPRESSION(ListLiteral(elements));
}

Expression_ptr ExpressionParser::parse_tuple_literal()
{
	ExpressionVector elements;

	if (token_pipe->optional(TokenType::CLOSE_FLOOR_BRACKET))
	{
		return MAKE_EXPRESSION(TupleLiteral(elements));
	}

	elements = parse_expressions();

	token_pipe->expect(TokenType::CLOSE_FLOOR_BRACKET);
	return MAKE_EXPRESSION(TupleLiteral(elements));
}

Expression_ptr ExpressionParser::parse_UDT_creation()
{
	ExpressionVector expressions;

	auto type_identifier = token_pipe->required(TokenType::CALLABLE_IDENTIFIER);

	token_pipe->expect(TokenType::OPEN_PARENTHESIS);

	if (token_pipe->optional(TokenType::CLOSE_PARENTHESIS))
		return MAKE_EXPRESSION(UDTConstruct(type_identifier->value, expressions));

	expressions = parse_expressions();

	token_pipe->expect(TokenType::CLOSE_PARENTHESIS);

	return MAKE_EXPRESSION(UDTConstruct(type_identifier->value, expressions));
}

Expression_ptr ExpressionParser::parse_map_literal()
{
	map<Expression_ptr, Expression_ptr> pairs;

	if (token_pipe->optional(TokenType::CLOSE_ANGLE_BRACKET))
		return MAKE_EXPRESSION(MapLiteral(pairs));

	while (true)
	{
		token_pipe->ignore({ TokenType::SPACE, TokenType::EOL });

		auto key = consume_valid_map_key();

		if (token_pipe->optional(TokenType::COLON))
		{
			auto value = parse_expression();
			pairs.insert_or_assign(key, value);
		}
		else
		{
			pairs.insert_or_assign(key, key);
		}

		token_pipe->ignore({ TokenType::SPACE, TokenType::EOL });

		if (token_pipe->optional(TokenType::CLOSE_ANGLE_BRACKET))
			break;

		token_pipe->expect(TokenType::COMMA);
	}

	return MAKE_EXPRESSION(MapLiteral(pairs));
}

Expression_ptr ExpressionParser::parse_identifier(Token_ptr identifier)
{
	if (token_pipe->optional(TokenType::COLON_COLON))
	{
		auto member_chain = parse_enum_member_chain();
		return MAKE_EXPRESSION(EnumMember(identifier->value, member_chain));
	}

	return MAKE_EXPRESSION(Identifier(identifier->value));
}

std::vector<std::wstring> ExpressionParser::parse_enum_member_chain()
{
	std::vector<std::wstring> member_chain;

	do
	{
		auto member_identifier = token_pipe->required(TokenType::IDENTIFIER);
		member_chain.push_back(member_identifier->value);
	} while (token_pipe->optional(TokenType::COLON_COLON));

	return member_chain;
}

Expression_ptr ExpressionParser::parse_member_access()
{
	ASSERT(ast.size() > 0, ERROR_CODE::UNEXPECTED_TOKEN);

	auto container = move(ast.top());
	ast.pop();

	ExpressionVector chain = { container };

	do
	{
		auto member_expression = parse_expression();
		chain.push_back(member_expression);
	} while (token_pipe->optional(TokenType::DOT));

	pop_context(ExpressionContext::MEMBER_ACCESS);
	return MAKE_EXPRESSION(UDTMemberAccess(chain));
}

ExpressionVector ExpressionParser::parse_function_call_arguments()
{
	token_pipe->expect(TokenType::OPEN_PARENTHESIS);

	ExpressionVector expressions;

	if (token_pipe->optional(TokenType::CLOSE_PARENTHESIS))
	{
		pop_context(ExpressionContext::FUNCTION_CALL);
		return expressions;
	}

	expressions = parse_expressions();

	token_pipe->expect(TokenType::CLOSE_PARENTHESIS);

	return expressions;
}

// Utils

Expression_ptr ExpressionParser::finish_parsing()
{
	operator_stack->drain_into_ast(ast);

	ASSERT(ast.size() == 1, ERROR_CODE::MALFORMED_EXPRESSION);

	auto result = move(ast.top());
	ast.pop();

	return move(result);
}

Expression_ptr ExpressionParser::consume_valid_map_key()
{
	auto token = token_pipe->current();
	OPT_CHECK(token);

	switch (token.value()->type)
	{
	case TokenType::STRING_LITERAL:
	{
		ADVANCE_PTR;
		return MAKE_EXPRESSION(token.value()->value);
	}
	case TokenType::NUMBER_LITERAL:
	{
		ADVANCE_PTR;
		return MAKE_EXPRESSION(stod(token.value()->value));
	}
	case TokenType::TRUE_KEYWORD:
	{
		ADVANCE_PTR;
		return MAKE_EXPRESSION(true);
	}
	case TokenType::FALSE_KEYWORD:
	{
		ADVANCE_PTR;
		return MAKE_EXPRESSION(false);
	}
	}

	FATAL(ERROR_CODE::INVALID_MAP_KEY);
}

Expression_ptr ExpressionParser::consume_valid_UDT_key()
{
	auto token = token_pipe->current();
	OPT_CHECK(token);

	switch (token.value()->type)
	{
	case TokenType::IDENTIFIER:
	{
		ADVANCE_PTR;
		return MAKE_EXPRESSION(Identifier(token.value()->value));
	}
	}

	FATAL(ERROR_CODE::INVALID_MAP_KEY);
}

void ExpressionParser::push_context(ExpressionContext context)
{
	context_stack.push(context);
}

void ExpressionParser::pop_context(ExpressionContext context)
{
	ASSERT(context_stack.top() == context, ERROR_CODE::EXPRESSION_PARSER_CONTEXT_MISMATCH);
	context_stack.pop();
}