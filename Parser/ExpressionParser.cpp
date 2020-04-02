#include "pch.h"
#include "ExpressionParser.h"

#include <vector>
#include <string>
#include <memory>
#include <map>

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
			return nullptr;

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
			ast.push(make_shared<NumberLiteral>(stod(current_token->value)));
			ADVANCE_PTR;
			break;
		}
		case WTokenType::StringLiteral:
		{
			ast.push(make_shared<StringLiteral>(current_token->value));
			ADVANCE_PTR;
			break;
		}
		case WTokenType::TRUE_KEYWORD:
		case WTokenType::FALSE_KEYWORD:
		{
			bool bool_value = current_token->value == "true" ? true : false;
			ast.push(make_shared<BooleanLiteral>(bool_value));
			ADVANCE_PTR;
			break;
		}
		case WTokenType::Identifier:
		{
			ADVANCE_PTR;

			auto member_access_expression = consume_member_access(current_token);

			if (member_access_expression)
				ast.push(member_access_expression);
			else
				ast.push(make_shared<Identifier>(current_token->value));

			break;
		}
		case WTokenType::OPEN_BRACKET:
		{
			ADVANCE_PTR;
			auto vector_literal = parse_vector_literal();
			RETURN_NULLPTR_IF_NULLPTR(vector_literal);

			ast.push(move(vector_literal));
			break;
		}
		case WTokenType::OPEN_CURLY_BRACE:
		{
			ADVANCE_PTR;
			auto UDT_literal = parse_UDT_literal();
			RETURN_NULLPTR_IF_NULLPTR(UDT_literal);

			ast.push(move(UDT_literal));
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
		case WTokenType::FunctionIdentifier:
		{
			ADVANCE_PTR;
			auto arguments = parse_function_call_arguments();
			RETURN_NULLPTR_IF_NULLPTR(arguments);
		}
		}
	}

	return finish_parsing();
}

Expression_ptr ExpressionParser::finish_parsing()
{
	operator_stack->drain_into_ast(ast);

	if (ast.size() > 1) {
		return nullptr;
	}

	auto result = move(ast.top());
	ast.pop();

	return result;
}

shared_ptr<string> ExpressionParser::consume_valid_record_key()
{
	auto token = token_pipe->get_current_token();
	RETURN_NULLPTR_IF_NULLPTR(token);

	switch (token->type)
	{
	case WTokenType::Identifier:
	{
		ADVANCE_PTR;
		return make_shared<string>(token->value);
	}
	}
	return nullptr;
}

// Literal Parsers

Expression_ptr ExpressionParser::parse_vector_literal()
{
	vector<Expression_ptr> elements;

	if (token_pipe->expect_current_token_to_be(WTokenType::CLOSE_BRACKET))
		return make_shared<VectorLiteral>(elements);

	while (true)
	{
		auto element = parse_expression();
		RETURN_NULLPTR_IF_NULLPTR(element);

		elements.push_back(move(element));

		if (token_pipe->expect_current_token_to_be(WTokenType::CLOSE_BRACKET))
			return make_shared<VectorLiteral>(elements);

		RETURN_NULLPTR_IF_FALSE(token_pipe->expect_current_token_to_be(WTokenType::COMMA));
	}
}

Expression_ptr ExpressionParser::parse_UDT_literal()
{
	map<string, Expression_ptr> pairs;

	if (token_pipe->expect_current_token_to_be(WTokenType::CLOSE_CURLY_BRACE))
		return make_shared<UDTLiteral>(pairs);

	while (true)
	{
		token_pipe->ignore(WTokenType::EOL);

		auto key = consume_valid_record_key();
		RETURN_NULLPTR_IF_NULLPTR(key);

		RETURN_NULLPTR_IF_FALSE(token_pipe->expect_current_token_to_be(WTokenType::COLON));

		auto value = parse_expression();
		RETURN_NULLPTR_IF_NULLPTR(value);

		pairs.insert_or_assign(*key.get(), value);

		token_pipe->ignore(WTokenType::EOL);

		if (token_pipe->expect_current_token_to_be(WTokenType::CLOSE_CURLY_BRACE))
			return make_shared<UDTLiteral>(pairs);

		RETURN_NULLPTR_IF_FALSE(token_pipe->expect_current_token_to_be(WTokenType::COMMA));
	}
}

Expression_ptr ExpressionParser::consume_member_access(Token_ptr identifier_token)
{
	if (token_pipe->expect_current_token_to_be(WTokenType::OPEN_BRACKET))
	{
		auto expression = parse_expression();
		RETURN_NULLPTR_IF_NULLPTR(expression);
		RETURN_NULLPTR_IF_FALSE(token_pipe->expect_current_token_to_be(WTokenType::CLOSE_BRACKET));

		return make_shared<VectorMemberAccess>(identifier_token->value, move(expression));
	}
	else if (token_pipe->expect_current_token_to_be(WTokenType::DOT))
	{
		auto identifier = token_pipe->consume_token(WTokenType::Identifier);
		RETURN_NULLPTR_IF_NULLPTR(identifier);

		return make_shared<UDTMemberAccess>(identifier_token->value, identifier->value);
	}

	return nullptr;
}

ExpressionVector_ptr ExpressionParser::parse_function_call_arguments()
{
	ExpressionVector_ptr expressions;

	RETURN_NULLPTR_IF_FALSE(token_pipe->expect_current_token_to_be(WTokenType::OPEN_PARENTHESIS));

	inside_function_call.push(true);

	if (token_pipe->expect_current_token_to_be(WTokenType::CLOSE_PARENTHESIS))
	{
		inside_function_call.pop();
		return expressions;
	}

	while (true)
	{
		auto expression = parse_expression();
		RETURN_NULLPTR_IF_NULLPTR(expression);

		expressions->push_back(move(expression));

		if (token_pipe->expect_current_token_to_be(WTokenType::COMMA))
			continue;

		if (token_pipe->expect_current_token_to_be(WTokenType::CLOSE_PARENTHESIS))
		{
			inside_function_call.pop();
			return expressions;
		}
		else
		{
			return nullptr;
		}
	}
}