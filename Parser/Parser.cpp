#pragma once
#include "pch.h"
#include "logger.h"
#include "Parser.h"
#include "TokenType.h"
#include "TokenPipe.h"

#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <utility>

using std::string;
using std::vector;
using std::stack;
using std::map;
using std::shared_ptr;
using std::make_shared;
using std::pair;
using std::make_pair;
using std::optional;
using std::make_optional;
using std::move;

// API

Module Parser::execute()
{
	Module mod;
	size_t length = token_pipe->get_size();

	while (true)
	{
		if ((size_t)token_pipe->get_pointer_index() >= length)
			break;

		Statement_ptr node = parse_statement(false);
		FATAL_IF_NULLPTR(node, "Malformed Statement");
		mod.add(move(node));
	}

	return mod;
}

// Statement Parsers

Statement_ptr Parser::parse_statement(bool is_public)
{
	token_pipe->ignore(WTokenType::EOL);

	auto token = token_pipe->get_current_token();
	ADVANCE_PTR;

	if (token == nullptr)
		return nullptr;

	switch (token->type)
	{
		CASE(WTokenType::LET, this->parse_variable_declaration(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, this->parse_variable_declaration(is_public, false));
		CASE(WTokenType::Identifier, this->consume_assignment_or_expression_statement(move(token)));
		CASE(WTokenType::BREAK, this->parse_break_statement());
		CASE(WTokenType::CONTINUE, this->parse_continue_statement());
		CASE(WTokenType::RETURN, this->parse_return_statement());
		CASE(WTokenType::PUB, this->parse_public_statement());
		CASE(WTokenType::IF, this->parse_branching_statement());
		CASE(WTokenType::LOOP, this->parse_loop_statement());
		CASE(WTokenType::TYPE, this->parse_UDT_declaration(is_public));
		CASE(WTokenType::FN, this->parse_function_definition(is_public));
	default: {
		RETREAT_PTR;
		return this->parse_expression_statement();
	}
	}
}

Statement_ptr Parser::parse_public_statement()
{
	auto token = token_pipe->get_current_token();
	ADVANCE_PTR;

	if (token == nullptr)
		return nullptr;

	const bool is_public = true;

	switch (token->type)
	{
		CASE(WTokenType::LET, this->parse_variable_declaration(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, this->parse_variable_declaration(is_public, false));
		CASE(WTokenType::TYPE, this->parse_UDT_declaration(is_public));
		CASE(WTokenType::FN, this->parse_function_definition(is_public));
	default: {
		std::stringstream message;
		message << token->type << " cannot be made public";
		FATAL(message.str());
	}
	}
}

Statement_ptr Parser::parse_return_statement()
{
	auto expression = expr_parser->parse_expression();
	FATAL_IF_NULLPTR(expression, "Malformed Expression");

	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::EOL),
		"Expected an EOL"
	);

	return make_shared<Return>(make_optional(move(expression)));
}

Statement_ptr Parser::parse_break_statement()
{
	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::EOL),
		"Expected an EOL"
	);

	return make_shared<Break>();
}

Statement_ptr Parser::parse_continue_statement()
{
	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::EOL),
		"Expected an EOL"
	);

	return make_shared<Continue>();
}

Statement_ptr Parser::parse_variable_declaration(bool is_public, bool is_mutable)
{
	// Identifier
	auto identifier = token_pipe->consume_token(WTokenType::Identifier);
	FATAL_IF_NULLPTR(identifier, "Malformed Identifier");

	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::COLON),
		"Expected a COLON"
	);

	// Type
	auto type = parse_type();
	FATAL_IF_NULLPTR(type, "Malformed Type");

	// Equal
	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::EQUAL),
		"Expected a EQUAL"
	);

	// Expression
	Expression_ptr expression = expr_parser->parse_expression();
	FATAL_IF_NULLPTR(expression, "Malformed Expression");

	// EOL
	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::EOL),
		"Expected an EOL"
	);

	return make_shared<VariableDeclaration>(is_public, is_mutable, identifier->value, move(type), move(expression));
}

// Expression Parsers

Statement_ptr Parser::parse_expression_statement()
{
	auto expression = expr_parser->parse_expression();
	FATAL_IF_NULLPTR(expression, "Malformed Expression");

	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::EOL),
		"Expected an EOL"
	);

	return make_shared<ExpressionStatement>(move(expression));
}

Statement_ptr Parser::consume_assignment_or_expression_statement(Token_ptr identifier)
{
	auto current_token = token_pipe->get_current_token();
	WTokenType current_token_type = current_token->type;

	if (current_token_type == WTokenType::EQUAL)
	{
		ADVANCE_PTR;

		auto expression = expr_parser->parse_expression();
		FATAL_IF_NULLPTR(expression, "Malformed Expression");

		FATAL_IF_FALSE(
			token_pipe->expect_current_token(WTokenType::EOL),
			"Expected an EOL"
		);

		return make_shared<Assignment>(identifier->value, move(expression));
	}
	else if (
		current_token_type == WTokenType::PLUS_EQUAL ||
		current_token_type == WTokenType::MINUS_EQUAL ||
		current_token_type == WTokenType::STAR_EQUAL ||
		current_token_type == WTokenType::DIVISION_EQUAL ||
		current_token_type == WTokenType::REMINDER_EQUAL ||
		current_token_type == WTokenType::POWER_EQUAL
		) {
		ADVANCE_PTR;

		auto expression = expr_parser->parse_expression();
		FATAL_IF_NULLPTR(expression, "Malformed Expression");

		FATAL_IF_FALSE(
			token_pipe->expect_current_token(WTokenType::EOL),
			"Expected an EOL"
		);

		convert_to_equivalent_token(current_token);

		auto identifier_expression = make_shared<Identifier>(identifier->value);
		auto rhs = make_shared<Binary>(identifier_expression, current_token, move(expression));

		return make_shared<Assignment>(identifier->value, move(rhs));
	}

	RETREAT_PTR; // To point to identifier token
	return this->parse_expression_statement();
}

// Block Statements Parsers

Block_ptr Parser::parse_block()
{
	Block_ptr statements = make_shared<Block>();

	token_pipe->ignore(WTokenType::EOL);
	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::OPEN_CURLY_BRACE),
		"Expected a OPEN_CURLY_BRACE"
	);

	while (true)
	{
		if (token_pipe->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
			return statements;

		token_pipe->ignore(WTokenType::EOL);

		auto statement = this->parse_statement(false);
		FATAL_IF_NULLPTR(statement, "Malformed Statement");

		statements->push_back(move(statement));
	}
}

Statement_ptr Parser::parse_loop_statement()
{
	auto block = this->parse_block();
	FATAL_IF_NULLPTR(block, "Malformed Statement Block");

	return make_shared<Loop>(block);
}

Statement_ptr Parser::parse_branching_statement()
{
	auto condition = expr_parser->parse_expression();
	FATAL_IF_NULLPTR(condition, "Malformed Condition");

	auto consequence = this->parse_block();
	FATAL_IF_NULLPTR(consequence, "Malformed Statement Block");

	// empty alternative
	auto alternative = make_shared<vector<Statement_ptr>>();

	if (token_pipe->expect_current_token(WTokenType::ELSE))
	{
		if (token_pipe->expect_current_token(WTokenType::IF))
		{
			auto alternative_stat = this->parse_branching_statement();
			FATAL_IF_NULLPTR(alternative_stat, "Malformed Statement");

			auto alternative = make_shared<vector<Statement_ptr>>();
			alternative->push_back(move(alternative_stat));
		}
		else
		{
			auto alternative = this->parse_block();
			FATAL_IF_NULLPTR(alternative, "Malformed Statement Block");
		}
	}

	return make_shared<Branch>(move(condition), consequence, alternative);
}

// Definition Parsers

Statement_ptr Parser::parse_UDT_declaration(bool is_public)
{
	auto name = token_pipe->consume_token(WTokenType::Identifier);
	FATAL_IF_NULLPTR(name, "Malformed Identifier");

	if (token_pipe->expect_current_token(WTokenType::OPEN_CURLY_BRACE))
	{
		map<string, Type_ptr> member_types;

		token_pipe->ignore(WTokenType::EOL);

		if (token_pipe->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
			return make_shared<UDTDefinition>(is_public, name->value, member_types);

		while (true)
		{
			token_pipe->ignore(WTokenType::EOL);

			auto identifier = token_pipe->consume_token(WTokenType::Identifier);
			FATAL_IF_NULLPTR(identifier, "Malformed Identifier");

			FATAL_IF_FALSE(
				token_pipe->expect_current_token(WTokenType::COLON),
				"Expected a COLON"
			);

			auto type = parse_type();
			FATAL_IF_NULLPTR(type, "Malformed Type");

			member_types.insert_or_assign(identifier->value, type);

			token_pipe->ignore(WTokenType::EOL);

			if (token_pipe->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
				return make_shared<UDTDefinition>(is_public, name->value, member_types);

			FATAL_IF_FALSE(
				token_pipe->expect_current_token(WTokenType::COMMA),
				"Expected a COMMA"
			);
		}
	}

	return nullptr;
}

Statement_ptr Parser::parse_function_definition(bool is_public)
{
	auto identifier = token_pipe->consume_token(WTokenType::FunctionIdentifier);
	FATAL_IF_NULLPTR(identifier, "Malformed Identifier");

	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::OPEN_PARENTHESIS),
		"Expected a OPEN_PARENTHESIS"
	);

	map<string, Type_ptr> arguments;

	while (true)
	{
		if (token_pipe->expect_current_token(WTokenType::CLOSE_PARENTHESIS))
			break;

		auto identifier = token_pipe->consume_token(WTokenType::Identifier);
		FATAL_IF_NULLPTR(identifier, "Malformed Identifier");

		FATAL_IF_FALSE(
			token_pipe->expect_current_token(WTokenType::COLON),
			"Expected a COLON"
		);

		auto type = parse_type();
		FATAL_IF_NULLPTR(type, "Malformed Type");

		arguments.insert_or_assign(identifier->value, type);
	}

	optional<Type_ptr> optional_return_type = std::nullopt;

	if (token_pipe->consume_token(WTokenType::ARROW))
	{
		auto return_type = parse_type();
		FATAL_IF_NULLPTR(return_type, "Malformed Type");

		optional_return_type = std::make_optional(return_type);
	}

	token_pipe->ignore(WTokenType::EOL);

	auto block = this->parse_block();
	FATAL_IF_NULLPTR(block, "Malformed Statement Block");

	return make_shared<FunctionDefinition>(is_public, identifier->value, arguments, move(optional_return_type), block);
}

// Type Parsers

Type_ptr Parser::parse_type()
{
	if (token_pipe->expect_current_token(WTokenType::OPEN_BRACKET))
		return parse_vector_type();

	auto type = consume_datatype_word();
	FATAL_IF_NULLPTR(type, "Expected a datatype");

	return nullptr;
}

Type_ptr Parser::parse_vector_type()
{
	auto type = parse_type();
	FATAL_IF_NULLPTR(type, "Malformed Type");

	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::CLOSE_BRACKET),
		"Expected a CLOSE_BRACKET"
	);

	return make_shared<VectorType>(move(type));
}

Type_ptr Parser::consume_datatype_word()
{
	auto token = token_pipe->get_current_token();
	FATAL_IF_NULLPTR(token, "Expected a datatype");

	switch (token->type)
	{
	case WTokenType::NUM:
	{
		ADVANCE_PTR;
		return make_shared<NumberType>();
	}
	case WTokenType::STR:
	{
		ADVANCE_PTR;
		return make_shared<StringType>();
	}
	case WTokenType::BOOL:
	{
		ADVANCE_PTR;
		return make_shared<BoolType>();
	}
	case WTokenType::Identifier:
	{
		ADVANCE_PTR;
		return make_shared<UDTType>(token->value);
	}
	default:
	{
		FATAL_IF_NULLPTR(token, "Expected a datatype");
	}
	}
}

// Utils

void Parser::convert_to_equivalent_token(Token_ptr token)
{
	switch (token->type)
	{
	case WTokenType::PLUS_EQUAL:
	{
		token->type = WTokenType::PLUS;
		token->value = "+";
		break;
	}
	case WTokenType::MINUS_EQUAL: {
		token->type = WTokenType::MINUS;
		token->value = "-";
		break;
	}
	case WTokenType::STAR_EQUAL:
	{
		token->type = WTokenType::STAR;
		token->value = "*";
		break;
	}
	case WTokenType::DIVISION_EQUAL:
	{
		token->type = WTokenType::DIVISION;
		token->value = "/";
		break;
	}
	case WTokenType::REMINDER_EQUAL:
	{
		token->type = WTokenType::REMINDER;
		token->value = "%";
		break;
	}
	case WTokenType::POWER_EQUAL:
	{
		token->type = WTokenType::POWER;
		token->value = "^";
		break;
	}
	}
}