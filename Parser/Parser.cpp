#pragma once
#include "pch.h"
#include "logger.h"
#include "Parser.h"
#include "TokenType.h"
#include "TokenPipe.h"

#include <iostream>
#include <vector>
#include <stack>
#include <map>
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

		if (node)
		{
			mod.add(move(node));
		}
	}

	return mod;
}

// Statement Parsers

Statement_ptr Parser::parse_statement(bool is_public)
{
	token_pipe->ignore(WTokenType::EOL);

	auto token = token_pipe->get_current_token();

	if (token == nullptr)
		return nullptr;

	ADVANCE_PTR;

	switch (token->type)
	{
		CASE(WTokenType::LET, parse_variable_declaration(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, parse_variable_declaration(is_public, false));
		CASE(WTokenType::Identifier, consume_assignment_or_expression_statement(move(token)));
		CASE(WTokenType::BREAK, parse_break_statement());
		CASE(WTokenType::CONTINUE, parse_continue_statement());
		CASE(WTokenType::RETURN, parse_return_statement());
		CASE(WTokenType::PUB, parse_public_statement());
		CASE(WTokenType::IF, parse_branching_statement());
		CASE(WTokenType::LOOP, parse_loop_statement());
		CASE(WTokenType::FOREACH, parse_foreach_loop_statement());
		CASE(WTokenType::TYPE, parse_UDT_declaration(is_public));
		CASE(WTokenType::FN, parse_function_definition(is_public));
		CASE(WTokenType::IMPORT, parse_import_statement());
		CASE(WTokenType::ENUM, parse_enum_statement(is_public));
	default: {
		RETREAT_PTR;
		return parse_expression_statement();
	}
	}
}

Statement_ptr Parser::parse_public_statement()
{
	auto token = token_pipe->get_current_token();
	FATAL_IF_NULLPTR(token, "Pub keyword followed by a nullptr.");

	ADVANCE_PTR;

	const bool is_public = true;

	switch (token->type)
	{
		CASE(WTokenType::LET, parse_variable_declaration(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, parse_variable_declaration(is_public, false));
		CASE(WTokenType::TYPE, parse_UDT_declaration(is_public));
		CASE(WTokenType::FN, parse_function_definition(is_public));
		CASE(WTokenType::ENUM, parse_enum_statement(is_public));
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

	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::EOL),
		"Expected an EOL"
	);

	if (expression)
	{
		return make_shared<Return>(make_optional(move(expression)));
	}

	return make_shared<Return>(std::nullopt);
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
	FATAL_IF_NULLPTR(identifier, "Malformed Identifier declared");

	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::COLON),
		"Expected a COLON"
	);

	// Type
	auto type = parse_type();
	FATAL_IF_NULLPTR(type, "Malformed Type in variable declaration");

	// Equal
	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::EQUAL),
		"Expected a EQUAL"
	);

	// Expression
	Expression_ptr expression = expr_parser->parse_expression();
	FATAL_IF_NULLPTR(expression, "Malformed Expression assigned in variable declaration");

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

	WTokenType current_token_type = WTokenType::UNKNOWN;

	if (current_token)
	{
		current_token_type = current_token->type;
	}

	switch (current_token_type)
	{
	case WTokenType::EQUAL:
	{
		ADVANCE_PTR;

		auto expression = expr_parser->parse_expression();
		FATAL_IF_NULLPTR(expression, "Cannot assign a malformed expression to a variable");

		FATAL_IF_FALSE(
			token_pipe->expect_current_token(WTokenType::EOL),
			"Expected an EOL"
		);

		return make_shared<Assignment>(identifier->value, move(expression));
	}
	case WTokenType::PLUS_EQUAL:
	case WTokenType::MINUS_EQUAL:
	case WTokenType::STAR_EQUAL:
	case WTokenType::DIVISION_EQUAL:
	case WTokenType::REMINDER_EQUAL:
	case WTokenType::POWER_EQUAL:
	{
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
	default:
	{
		RETREAT_PTR;
		return parse_expression_statement();
	}
	}
}

Statement_ptr Parser::parse_import_statement()
{
	auto current_token = token_pipe->consume_token(WTokenType::Identifier);

	if (current_token)
	{
		return make_shared<ImportSTD>(current_token->value);
	}

	FATAL_IF_NULLPTR(
		token_pipe->consume_token(WTokenType::OPEN_CURLY_BRACE),
		"Expected a OPEN_CURLY_BRACE"
	);

	vector<string> goods;

	while (true)
	{
		auto identifier = token_pipe->consume_token(WTokenType::Identifier);
		FATAL_IF_NULLPTR(identifier, "Expected an Identifier");

		goods.push_back(identifier->value);

		if (token_pipe->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
			break;

		FATAL_IF_FALSE(
			token_pipe->expect_current_token(WTokenType::COMMA),
			"Expected a COMMA or a CLOSE_CURLY_BRACE"
		);
	}

	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::FROM),
		"Expected the FROM keyword"
	);

	auto path_token = token_pipe->consume_token(WTokenType::StringLiteral);
	FATAL_IF_NULLPTR(path_token, "Expected the path StringLiteral");

	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::EOL),
		"Expected an EOL"
	);

	return make_shared<Import>(goods, path_token->value);
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

		auto statement = parse_statement(false);
		FATAL_IF_NULLPTR(statement, "Malformed Statement");

		statements->push_back(move(statement));
	}
}

Statement_ptr Parser::parse_loop_statement()
{
	auto block = parse_block();
	FATAL_IF_NULLPTR(block, "Malformed Statement Block");

	return make_shared<Loop>(block);
}

Statement_ptr Parser::parse_foreach_loop_statement()
{
	auto identifier = token_pipe->consume_token(WTokenType::Identifier);
	FATAL_IF_NULLPTR(identifier, "Expected an identifier");

	FATAL_IF_NULLPTR(
		token_pipe->consume_token(WTokenType::IN_KEYWORD),
		"Expected the IN keyword"
	);

	auto iterable_identifier = token_pipe->consume_token(WTokenType::Identifier);
	FATAL_IF_NULLPTR(iterable_identifier, "Expected an identifier");

	auto block = parse_block();
	FATAL_IF_NULLPTR(block, "Malformed Statement Block");

	return make_shared<ForEachLoop>(identifier->value, iterable_identifier->value, block);
}

Statement_ptr Parser::parse_branching_statement()
{
	auto condition = expr_parser->parse_expression();
	FATAL_IF_NULLPTR(condition, "Malformed Condition");

	auto consequence = parse_block();
	FATAL_IF_NULLPTR(consequence, "Malformed Statement Block");

	// empty alternative
	auto alternative = make_shared<vector<Statement_ptr>>();

	if (token_pipe->expect_current_token(WTokenType::ELSE))
	{
		if (token_pipe->expect_current_token(WTokenType::IF))
		{
			auto alternative_stat = parse_branching_statement();
			FATAL_IF_NULLPTR(alternative_stat, "Malformed Statement");

			auto alternative = make_shared<vector<Statement_ptr>>();
			alternative->push_back(move(alternative_stat));
		}
		else
		{
			auto alternative = parse_block();
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

	vector<pair<string, Type_ptr>> arguments;

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

		arguments.push_back(pair(identifier->value, type));
	}

	optional<Type_ptr> optional_return_type = std::nullopt;

	if (token_pipe->consume_token(WTokenType::ARROW))
	{
		auto return_type = parse_type();
		FATAL_IF_NULLPTR(return_type, "Malformed return type");

		optional_return_type = std::make_optional(return_type);
	}

	token_pipe->ignore(WTokenType::EOL);

	auto block = parse_block();
	FATAL_IF_NULLPTR(block, "Malformed Statement Block");

	return make_shared<FunctionDefinition>(is_public, identifier->value, arguments, move(optional_return_type), block);
}

Statement_ptr Parser::parse_enum_statement(bool is_public)
{
	auto identifier = token_pipe->consume_token(WTokenType::Identifier);
	FATAL_IF_NULLPTR(identifier, "Expected an Identifier");

	token_pipe->ignore(WTokenType::EOL);

	FATAL_IF_NULLPTR(
		token_pipe->consume_token(WTokenType::OPEN_CURLY_BRACE),
		"Expected a OPEN_CURLY_BRACE"
	);

	vector<string> members;
	vector<string>::iterator it;

	while (true)
	{
		token_pipe->ignore(WTokenType::EOL);

		auto identifier = token_pipe->consume_token(WTokenType::Identifier);
		FATAL_IF_NULLPTR(identifier, "Expected an Identifier");

		it = std::find(members.begin(), members.end(), identifier->value);
		FATAL_IF_TRUE(it != members.end(), "Duplicate Enum members are present");
		members.push_back(identifier->value);

		token_pipe->ignore(WTokenType::EOL);

		if (token_pipe->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
			break;

		FATAL_IF_FALSE(
			token_pipe->expect_current_token(WTokenType::COMMA),
			"Expected a COMMA"
		);
	}

	return make_shared<EnumDefinition>(is_public, identifier->value, members);
}

// Type Parsers

Type_ptr Parser::parse_type()
{
	if (token_pipe->expect_current_token(WTokenType::OPEN_BRACKET))
		return parse_vector_type();

	auto type = consume_datatype_word();
	FATAL_IF_NULLPTR(type, "Expected a datatype");

	return type;
}

Type_ptr Parser::parse_vector_type()
{
	auto type = parse_type();
	FATAL_IF_NULLPTR(type, "Malformed Type set for vector");

	FATAL_IF_FALSE(
		token_pipe->expect_current_token(WTokenType::CLOSE_BRACKET),
		"Expected a CLOSE_BRACKET"
	);

	return make_shared<VectorType>(move(type));
}

Type_ptr Parser::consume_datatype_word()
{
	auto token = token_pipe->get_current_token();
	FATAL_IF_NULLPTR(token, "Expected a datatype word");

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
		return make_shared<BooleanType>();
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