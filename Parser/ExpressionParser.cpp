#pragma once
#include "pch.h"
#include "ExpressionParser.h"
#include "CommonAssertion.h"

#include <vector>
#include <string>
#include <memory>
#include <map>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr");
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)

using std::vector;
using std::string;
using std::make_shared;
using std::shared_ptr;
using std::map;
using std::move;

Expression_ptr ExpressionParser::parse_expression()
{
	while (true)
	{
		Token_ptr current_token = token_pipe->get_current_token();

		if (current_token == nullptr)
			break;

		switch (current_token->type)
		{
		case WTokenType::EOL:
		case WTokenType::COMMA:
		case WTokenType::CLOSE_CURLY_BRACE:
		case WTokenType::CLOSE_BRACKET:
		{
			return finish_parsing();
		}
		case WTokenType::NumberLiteral:
		{
			ast.push(MAKE_EXPRESSION(stod(current_token->value)));
			ADVANCE_PTR;
			break;
		}
		case WTokenType::StringLiteral:
		{
			ast.push(MAKE_EXPRESSION(current_token->value));
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

			auto member_access_expression = consume_member_access(current_token);

			if (member_access_expression)
			{
				ast.push(member_access_expression);
			}
			else
			{
				ast.push(MAKE_EXPRESSION(current_token->value));
			}

			break;
		}
		case WTokenType::OPEN_BRACKET:
		{
			ADVANCE_PTR;
			auto vector_literal = parse_vector_literal();
			ast.push(move(vector_literal));
			break;
		}
		case WTokenType::OPEN_CURLY_BRACE:
		{
			ADVANCE_PTR;
			auto literal = parse_dictionary_literal();
			ast.push(move(literal));
			break;
		}
		case WTokenType::OPEN_PARENTHESIS:
		{
			inside_function_call.push(false);
			operator_stack->dumb_push(move(current_token));
			ADVANCE_PTR;
			break;
		}
		case WTokenType::CLOSE_PARENTHESIS:
		{
			if (this->inside_function_call.top() == true)
				return finish_parsing();

			operator_stack->drain_into_ast_until_open_parenthesis(ast);
			ADVANCE_PTR;
			return finish_parsing();
		}
		case WTokenType::FunctionIdentifier:
		{
			ADVANCE_PTR;
			auto arguments = parse_function_call_arguments();
			ast.push(MAKE_EXPRESSION(FunctionCall(current_token->value, arguments)));
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
			operator_stack->smart_push(move(current_token), ast);
			ADVANCE_PTR;
			break;
		}
		}
	}

	return finish_parsing();
}

// Literal Parsers

Expression_ptr ExpressionParser::parse_vector_literal()
{
	vector<Expression_ptr> elements;

	if (token_pipe->next_significant_token_is(WTokenType::CLOSE_BRACKET))
		return MAKE_EXPRESSION(VectorLiteral(elements));

	while (true)
	{
		auto element = parse_expression();
		elements.push_back(move(element));

		if (token_pipe->next_significant_token_is(WTokenType::CLOSE_BRACKET))
			return MAKE_EXPRESSION(VectorLiteral(elements));

		ASSERT(
			token_pipe->next_significant_token_is(WTokenType::COMMA),
			"Expected a COMMA"
		);
	}
}

Expression_ptr ExpressionParser::parse_dictionary_literal()
{
	map<Token_ptr, Expression_ptr> pairs;

	if (token_pipe->next_significant_token_is(WTokenType::CLOSE_CURLY_BRACE))
		return MAKE_EXPRESSION(DictionaryLiteral(pairs));

	while (true)
	{
		auto key = consume_valid_dictionary_key();

		ASSERT(
			token_pipe->next_significant_token_is(WTokenType::COLON),
			"Expected a COLON"
		);

		auto value = parse_expression();
		pairs.insert_or_assign(key, value);

		if (token_pipe->next_significant_token_is(WTokenType::CLOSE_CURLY_BRACE))
			return MAKE_EXPRESSION(DictionaryLiteral(pairs));

		ASSERT(
			token_pipe->next_significant_token_is(WTokenType::COMMA),
			"Expected a COMMA"
		);
	}
}

Expression_ptr ExpressionParser::consume_member_access(Token_ptr identifier)
{
	if (token_pipe->next_significant_token_is(WTokenType::OPEN_BRACKET))
	{
		auto expression = parse_expression();

		ASSERT(
			token_pipe->next_significant_token_is(WTokenType::CLOSE_BRACKET),
			"Expected a CLOSE_BRACKET"
		);

		return MAKE_EXPRESSION(MemberAccess(identifier->value, move(expression)));
	}
	else if (
		token_pipe->next_significant_token_is(WTokenType::DOT) ||
		token_pipe->next_significant_token_is(WTokenType::COLON_COLON)
		)
	{
		auto member_identifier = token_pipe->consume_required_token(WTokenType::Identifier);
		return MAKE_EXPRESSION(MemberAccess(
			identifier->value,
			MAKE_EXPRESSION(Identifier(member_identifier->value))
		));
	}

	return nullptr;
}

ExpressionVector ExpressionParser::parse_function_call_arguments()
{
	ExpressionVector expressions;

	ASSERT(
		token_pipe->next_significant_token_is(WTokenType::OPEN_PARENTHESIS),
		"Expected a OPEN_PARENTHESIS"
	);

	inside_function_call.push(true);

	if (token_pipe->next_significant_token_is(WTokenType::CLOSE_PARENTHESIS))
	{
		inside_function_call.pop();
		return expressions;
	}

	while (true)
	{
		auto expression = parse_expression();
		expressions.push_back(move(expression));

		if (token_pipe->next_significant_token_is(WTokenType::COMMA))
			continue;

		ASSERT(
			token_pipe->next_significant_token_is(WTokenType::CLOSE_PARENTHESIS),
			"Expected a CLOSE_PARENTHESIS"
		);

		inside_function_call.pop();
		return expressions;
	}
}

// Utils

Expression_ptr ExpressionParser::finish_parsing()
{
	operator_stack->drain_into_ast(ast);

	ASSERT(ast.size() != 0, "Malformed Expression. AST is empty.");

	auto result = move(ast.top());
	ast.pop();

	return move(result);
}

Token_ptr ExpressionParser::consume_valid_dictionary_key()
{
	auto token = token_pipe->get_significant_token();
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