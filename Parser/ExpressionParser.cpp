#pragma once
#include "pch.h"
#include "ExpressionParser.h"
#include "CommonAssertion.h"

#include <vector>
#include <string>
#include <memory>
#include <map>

#define ADVANCE_PTR token_pipe->advance_pointer()
#define RETREAT_PTR token_pipe->retreat_pointer()
#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr");
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)
#define IGNORABLES { WTokenType::EOL }
#define PUSH_CONTEXT(x) context_stack.push(x);
#define POP_CONTEXT(x) ASSERT(context_stack.top() == x, "Context Mismatch"); context_stack.pop();

using std::vector;
using std::string;
using std::map;
using std::make_shared;
using std::shared_ptr;
using std::move;

Expression_ptr ExpressionParser::parse_expression()
{
	PUSH_CONTEXT(ExpressionContext::GLOBAL_EXPRESSION);

	while (true)
	{
		Token_ptr current = token_pipe->current();

		if (current == nullptr)
			break;

		switch (current->type)
		{
			// SIMPLE

		case WTokenType::NumberLiteral:
		{
			ast.push(MAKE_EXPRESSION(stod(current->value)));
			ADVANCE_PTR;
			break;
		}
		case WTokenType::StringLiteral:
		{
			ast.push(MAKE_EXPRESSION(current->value));
			ADVANCE_PTR;
			break;
		}
		case WTokenType::TRUE_KEYWORD:
		{
			ast.push(MAKE_EXPRESSION(true));
			ADVANCE_PTR;
			break;
		}
		case WTokenType::FALSE_KEYWORD:
		{
			ast.push(MAKE_EXPRESSION(false));
			ADVANCE_PTR;
			break;
		}
		case WTokenType::Identifier:
		{
			ADVANCE_PTR;
			auto expression = parse_identifier(current);
			ast.push(expression);
			break;
		}

		// DICTIONARY_LITERAL

		case WTokenType::OPEN_CURLY_BRACE:
		{
			PUSH_CONTEXT(ExpressionContext::DICTIONARY_LITERAL);
			ADVANCE_PTR;
			auto literal = parse_dictionary_literal();
			ast.push(move(literal));
			break;
		}
		case WTokenType::CLOSE_CURLY_BRACE:
		{
			POP_CONTEXT(ExpressionContext::DICTIONARY_LITERAL);
			ADVANCE_PTR;
			return finish_parsing();
		}

		// SEQUENCE_LITERAL

		case WTokenType::OPEN_SQUARE_BRACKET:
		{
			PUSH_CONTEXT(ExpressionContext::SEQUENCE_LITERAL);
			ADVANCE_PTR;
			auto literal = parse_list_literal();
			ast.push(move(literal));
			break;
		}
		case WTokenType::OPEN_TUPLE_PARENTHESIS:
		{
			PUSH_CONTEXT(ExpressionContext::SEQUENCE_LITERAL);
			ADVANCE_PTR;
			auto literal = parse_tuple_literal();
			ast.push(move(literal));
			break;
		}
		case WTokenType::CLOSE_SQUARE_BRACKET:
		{
			POP_CONTEXT(ExpressionContext::SEQUENCE_LITERAL);
			ADVANCE_PTR;
			return finish_parsing();
		}

		// FUNCTION CALL

		case WTokenType::FunctionIdentifier:
		{
			PUSH_CONTEXT(ExpressionContext::FUNCTION_CALL);
			ADVANCE_PTR;
			auto arguments = parse_function_call_arguments();
			ast.push(MAKE_EXPRESSION(FunctionCall(current->value, arguments)));
			break;
		}
		case WTokenType::CLOSE_PARENTHESIS:
		{
			if (context_stack.top() == ExpressionContext::FUNCTION_CALL)
				return finish_parsing();

			operator_stack->drain_into_ast_until_open_parenthesis(ast);
			POP_CONTEXT(ExpressionContext::PARENTHESIS);
			ADVANCE_PTR;
			return finish_parsing();
		}

		// OPERATORS

		case WTokenType::OPEN_PARENTHESIS:
		{
			PUSH_CONTEXT(ExpressionContext::PARENTHESIS);
			operator_stack->dumb_push(move(current));
			ADVANCE_PTR;
			break;
		}
		case WTokenType::BANG:
		case WTokenType::UNARY_MINUS:
		case WTokenType::UNARY_PLUS:
		case WTokenType::PLUS:
		case WTokenType::MINUS:
		case WTokenType::DIVISION:
		case WTokenType::REMINDER:
		case WTokenType::STAR:
		case WTokenType::POWER:
		case WTokenType::GREATER_THAN:
		case WTokenType::GREATER_THAN_EQUAL:
		case WTokenType::LESSER_THAN:
		case WTokenType::LESSER_THAN_EQUAL:
		case WTokenType::EQUAL_EQUAL:
		case WTokenType::BANG_EQUAL:
		case WTokenType::AND:
		case WTokenType::OR:
		{
			operator_stack->smart_push(move(current), ast);
			ADVANCE_PTR;
			break;
		}

		// OTHER

		case WTokenType::SPACE:
		{
			ADVANCE_PTR;
			break;
		}
		case WTokenType::EOL:
		case WTokenType::COLON:
		case WTokenType::COMMA:
		case WTokenType::EQUAL:
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

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		break;
	}

	return elements;
}

Expression_ptr ExpressionParser::parse_tuple_literal()
{
	ExpressionVector elements;

	if (token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
		return MAKE_EXPRESSION(TupleLiteral(elements));

	elements = parse_expressions();
	token_pipe->expect(WTokenType::CLOSE_PARENTHESIS);

	return MAKE_EXPRESSION(TupleLiteral(elements));
}

Expression_ptr ExpressionParser::parse_list_literal()
{
	ExpressionVector elements;

	if (token_pipe->optional(WTokenType::CLOSE_SQUARE_BRACKET))
		return MAKE_EXPRESSION(ListLiteral(elements));

	elements = parse_expressions();
	token_pipe->expect(WTokenType::CLOSE_SQUARE_BRACKET);

	return MAKE_EXPRESSION(ListLiteral(elements));
}

Expression_ptr ExpressionParser::parse_dictionary_literal()
{
	map<Token_ptr, Expression_ptr> pairs;

	if (token_pipe->optional(WTokenType::CLOSE_CURLY_BRACE))
		return MAKE_EXPRESSION(DictionaryLiteral(pairs));

	while (true)
	{
		token_pipe->skip_empty_lines();

		auto key = consume_valid_dictionary_key();
		token_pipe->expect(WTokenType::COLON);

		auto value = parse_expression();
		pairs.insert_or_assign(key, value);

		if (token_pipe->optional(WTokenType::CLOSE_CURLY_BRACE, IGNORABLES))
			return MAKE_EXPRESSION(DictionaryLiteral(pairs));

		token_pipe->expect(WTokenType::COMMA);
	}
}

Expression_ptr ExpressionParser::parse_identifier(Token_ptr identifier)
{
	if (token_pipe->optional(WTokenType::OPEN_SQUARE_BRACKET))
	{
		PUSH_CONTEXT(ExpressionContext::SEQUENCE_MEMBER_ACCESS);

		auto expression = parse_expression();
		token_pipe->expect(WTokenType::CLOSE_SQUARE_BRACKET);

		POP_CONTEXT(ExpressionContext::SEQUENCE_MEMBER_ACCESS);

		return MAKE_EXPRESSION(MemberAccess(identifier->value, move(expression)));
	}
	else if (token_pipe->optional(WTokenType::DOT))
	{
		auto member_identifier = token_pipe->required(WTokenType::Identifier);

		return MAKE_EXPRESSION(MemberAccess(
			identifier->value,
			MAKE_EXPRESSION(Identifier(member_identifier->value))
		));
	}
	else if (token_pipe->optional(WTokenType::COLON_COLON))
	{
		auto member_identifier = token_pipe->required(WTokenType::Identifier);
		return MAKE_EXPRESSION(EnumMember(identifier->value, member_identifier->value));
	}

	return MAKE_EXPRESSION(Identifier(identifier->value));
}

ExpressionVector ExpressionParser::parse_function_call_arguments()
{
	token_pipe->expect(WTokenType::OPEN_PARENTHESIS);

	ExpressionVector expressions;

	if (token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
	{
		POP_CONTEXT(ExpressionContext::FUNCTION_CALL);
		return expressions;
	}

	expressions = parse_expressions();

	token_pipe->expect(WTokenType::CLOSE_PARENTHESIS);
	POP_CONTEXT(ExpressionContext::FUNCTION_CALL);

	return expressions;
}

// Utils

Expression_ptr ExpressionParser::finish_parsing()
{
	operator_stack->drain_into_ast(ast);

	ASSERT(ast.size() == 1, "Malformed Expression. AST must have a single node after parsing.");
	POP_CONTEXT(ExpressionContext::GLOBAL_EXPRESSION);

	auto result = move(ast.top());
	ast.pop();

	return move(result);
}

Token_ptr ExpressionParser::consume_valid_dictionary_key()
{
	auto token = token_pipe->current();
	NULL_CHECK(token);

	switch (token->type)
	{
	case WTokenType::Identifier:
	case WTokenType::StringLiteral:
	case WTokenType::NumberLiteral:
	case WTokenType::TRUE_KEYWORD:
	case WTokenType::FALSE_KEYWORD:
	{
		ADVANCE_PTR;
		return move(token);
	}
	}

	FATAL("Expected a valid map key");
}