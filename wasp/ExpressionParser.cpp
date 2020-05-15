#pragma once
#include "ExpressionParser.h"
#include "Assertion.h"

#include <vector>
#include <string>
#include <memory>
#include <map>

#define ADVANCE_PTR token_pipe->advance_pointer()
#define RETREAT_PTR token_pipe->retreat_pointer()
#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)

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

		case WTokenType::NUMBER_LITERAL:
		{
			ast.push(MAKE_EXPRESSION(stod(current.value()->value)));
			ADVANCE_PTR;
			break;
		}
		case WTokenType::STRING_LITERAL:
		{
			ast.push(MAKE_EXPRESSION(current.value()->value));
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
		case WTokenType::IDENTIFIER:
		{
			ADVANCE_PTR;
			auto expression = parse_identifier(current.value());
			ast.push(expression);
			break;
		}
		case WTokenType::DOT:
		{
			ADVANCE_PTR;
			auto expression = parse_member_access();
			ast.push(expression);
			break;
		}

		// DICTIONARY_LITERAL

		case WTokenType::OPEN_CURLY_BRACE:
		{
			push_context(ExpressionContext::DICTIONARY_LITERAL);
			ADVANCE_PTR;
			auto value = parse_dictionary_literal();
			ast.push(move(value));
			break;
		}
		case WTokenType::CLOSE_CURLY_BRACE:
		{
			push_context(ExpressionContext::DICTIONARY_LITERAL);
			return finish_parsing();
		}

		// SEQUENCE_LITERAL

		case WTokenType::OPEN_SQUARE_BRACKET:
		{
			push_context(ExpressionContext::SEQUENCE_LITERAL);
			ADVANCE_PTR;
			auto value = parse_sequence_literal();
			ast.push(move(value));
			break;
		}
		case WTokenType::CLOSE_SQUARE_BRACKET:
		{
			pop_context(ExpressionContext::SEQUENCE_LITERAL);
			return finish_parsing();
		}

		// FUNCTION CALL

		case WTokenType::CALLABLE_IDENTIFIER:
		{
			push_context(ExpressionContext::FUNCTION_CALL);
			ADVANCE_PTR;
			auto arguments = parse_function_call_arguments();
			ast.push(MAKE_EXPRESSION(FunctionCall(current.value()->value, arguments)));
			break;
		}
		case WTokenType::CLOSE_PARENTHESIS:
		{
			if (context_stack.top() == ExpressionContext::FUNCTION_CALL)
			{
				pop_context(ExpressionContext::FUNCTION_CALL);
				return finish_parsing();
			}

			operator_stack->drain_into_ast_until_open_parenthesis(ast);
			pop_context(ExpressionContext::PARENTHESIS);
			return finish_parsing();
		}

		// OPERATORS

		case WTokenType::OPEN_PARENTHESIS:
		{
			push_context(ExpressionContext::PARENTHESIS);
			operator_stack->dumb_push(move(current.value()));
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
			operator_stack->smart_push(move(current.value()), ast);
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

Expression_ptr ExpressionParser::parse_sequence_literal()
{
	ExpressionVector elements;

	if (token_pipe->optional(WTokenType::CLOSE_SQUARE_BRACKET))
		return MAKE_EXPRESSION(SequenceLiteral(elements));

	elements = parse_expressions();

	token_pipe->expect(WTokenType::CLOSE_SQUARE_BRACKET);

	return MAKE_EXPRESSION(SequenceLiteral(elements));
}

Expression_ptr ExpressionParser::parse_dictionary_literal()
{
	map<Token_ptr, Expression_ptr> pairs;

	if (token_pipe->optional(WTokenType::CLOSE_CURLY_BRACE))
		return MAKE_EXPRESSION(DictionaryLiteral(pairs));

	while (true)
	{
		token_pipe->ignore({ WTokenType::SPACE, WTokenType::EOL });

		auto key = consume_valid_dictionary_key();

		if (token_pipe->optional(WTokenType::COLON))
		{
			auto value = parse_expression();
			pairs.insert_or_assign(key, value);
		}
		else
		{
			auto value = MAKE_EXPRESSION(Identifier(key->value));
			pairs.insert_or_assign(key, value);
		}

		token_pipe->ignore({ WTokenType::SPACE, WTokenType::EOL });

		if (token_pipe->optional(WTokenType::CLOSE_CURLY_BRACE))
			return MAKE_EXPRESSION(DictionaryLiteral(pairs));

		token_pipe->expect(WTokenType::COMMA);
	}
}

Expression_ptr ExpressionParser::parse_identifier(Token_ptr identifier)
{
	if (token_pipe->optional(WTokenType::COLON_COLON))
	{
		auto member_identifier = token_pipe->required(WTokenType::IDENTIFIER);
		return MAKE_EXPRESSION(EnumMember(identifier->value, member_identifier->value));
	}

	return MAKE_EXPRESSION(Identifier(identifier->value));
}

Expression_ptr ExpressionParser::parse_member_access()
{
	ASSERT(ast.size() > 0, "Cannot use dot operator without a container before it");

	push_context(ExpressionContext::MEMBER_ACCESS);

	auto container = move(ast.top());
	ast.pop();

	auto access_expression = parse_expression();

	pop_context(ExpressionContext::MEMBER_ACCESS);

	return MAKE_EXPRESSION(MemberAccess(container, access_expression));
}

ExpressionVector ExpressionParser::parse_function_call_arguments()
{
	token_pipe->expect(WTokenType::OPEN_PARENTHESIS);

	ExpressionVector expressions;

	if (token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
	{
		pop_context(ExpressionContext::FUNCTION_CALL);
		return expressions;
	}

	expressions = parse_expressions();

	token_pipe->expect(WTokenType::CLOSE_PARENTHESIS);

	return expressions;
}

// Utils

Expression_ptr ExpressionParser::finish_parsing()
{
	operator_stack->drain_into_ast(ast);

	ASSERT(ast.size() == 1, "Malformed Expression. AST must have a single node after parsing.");

	auto result = move(ast.top());
	ast.pop();

	return move(result);
}

Token_ptr ExpressionParser::consume_valid_dictionary_key()
{
	auto token = token_pipe->current();
	OPT_CHECK(token);

	switch (token.value()->type)
	{
	case WTokenType::IDENTIFIER:
	case WTokenType::STRING_LITERAL:
	case WTokenType::NUMBER_LITERAL:
	case WTokenType::TRUE_KEYWORD:
	case WTokenType::FALSE_KEYWORD:
	{
		ADVANCE_PTR;
		return move(token.value());
	}
	}

	FATAL("Expected a valid map key");
}

void ExpressionParser::push_context(ExpressionContext context)
{
	context_stack.push(context);
}

void ExpressionParser::pop_context(ExpressionContext context)
{
	ASSERT(context_stack.top() == context, "Context Mismatch");
	context_stack.pop();
}