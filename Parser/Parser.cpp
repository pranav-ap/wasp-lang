#pragma once
#include "pch.h"
#include "Parser.h"
#include "TokenType.h"
#include "TokenPipe.h"
#include "Assertion.h"

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
using namespace Assertion;

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
	ASSERT(!token, "Pub keyword followed by a nullptr.");

	ADVANCE_PTR;

	const bool is_public = true;

	switch (token->type)
	{
		CASE(WTokenType::LET, parse_variable_declaration(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, parse_variable_declaration(is_public, false));
		CASE(WTokenType::TYPE, parse_UDT_declaration(is_public));
		CASE(WTokenType::FN, parse_function_definition(is_public));
		CASE(WTokenType::ENUM, parse_enum_statement(is_public));
	default:
	{
		FATAL(token->value + " cannot be made public");
	}
	}
}

Statement_ptr Parser::parse_return_statement()
{
	auto expression = expr_parser->parse_expression();

	ASSERT(
		!token_pipe->expect_current_token(WTokenType::EOL),
		"Expected an EOL"
	);

	if (expression)
	{
		return make_shared<Return>(make_optional(move(expression)));
	}

	return make_shared<Return>();
}

Statement_ptr Parser::parse_break_statement()
{
	ASSERT(
		!token_pipe->expect_current_token(WTokenType::EOL),
		"Expected an EOL"
	);

	return make_shared<Break>();
}

Statement_ptr Parser::parse_continue_statement()
{
	ASSERT(
		!token_pipe->expect_current_token(WTokenType::EOL),
		"Expected an EOL"
	);

	return make_shared<Continue>();
}

Statement_ptr Parser::parse_variable_declaration(bool is_public, bool is_mutable)
{
	auto identifier = token_pipe->consume_token(WTokenType::Identifier);
	ASSERT(!identifier, "Malformed Identifier declared");

	ASSERT(
		!token_pipe->expect_current_token(WTokenType::COLON),
		"Expected a COLON"
	);

	auto type = parse_type();

	ASSERT(
		!token_pipe->expect_current_token(WTokenType::EQUAL),
		"Expected a EQUAL"
	);

	Expression_ptr expression = expr_parser->parse_expression();

	ASSERT(
		!token_pipe->expect_current_token(WTokenType::EOL),
		"Expected an EOL"
	);

	return make_shared<VariableDeclaration>(is_public, is_mutable, identifier->value, move(type), move(expression));
}

// Expression Parsers

Statement_ptr Parser::parse_expression_statement()
{
	auto expression = expr_parser->parse_expression();

	ASSERT(
		!token_pipe->expect_current_token(WTokenType::EOL),
		"Expected an EOL"
	);

	return make_shared<ExpressionStatement>(move(expression));
}

Statement_ptr Parser::consume_assignment_or_expression_statement(Token_ptr identifier)
{
	auto current_token = token_pipe->get_current_token();

	WTokenType current_token_type = current_token ? current_token->type : WTokenType::UNKNOWN;

	switch (current_token_type)
	{
	case WTokenType::EQUAL:
	{
		ADVANCE_PTR;

		auto expression = expr_parser->parse_expression();

		ASSERT(
			!token_pipe->expect_current_token(WTokenType::EOL),
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

		ASSERT(
			!token_pipe->expect_current_token(WTokenType::EOL),
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
	ASSERT(
		!token_pipe->consume_token(WTokenType::OPEN_CURLY_BRACE),
		"Expected a OPEN_CURLY_BRACE"
	);

	vector<string> goods;

	while (true)
	{
		auto identifier = token_pipe->consume_token(WTokenType::Identifier);
		ASSERT(!identifier, "Expected an Identifier");

		goods.push_back(identifier->value);

		if (token_pipe->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
			break;

		ASSERT(
			!token_pipe->expect_current_token(WTokenType::COMMA),
			"Expected a COMMA or a CLOSE_CURLY_BRACE"
		);
	}

	ASSERT(
		!token_pipe->expect_current_token(WTokenType::FROM),
		"Expected the FROM keyword"
	);

	auto current_token = token_pipe->consume_token(WTokenType::Identifier);

	if (current_token)
	{
		return make_shared<Import>(current_token->value, goods, true);
	}

	auto path_token = token_pipe->consume_token(WTokenType::StringLiteral);
	ASSERT(!path_token, "Expected a path");

	return make_shared<Import>(path_token->value, goods, false);
}

// Block Statements Parsers

Block_ptr Parser::parse_block()
{
	Block_ptr statements = make_shared<Block>();

	token_pipe->ignore(WTokenType::EOL);
	ASSERT(
		!token_pipe->expect_current_token(WTokenType::OPEN_CURLY_BRACE),
		"Expected a OPEN_CURLY_BRACE"
	);

	while (true)
	{
		if (token_pipe->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
			return statements;

		token_pipe->ignore(WTokenType::EOL);

		auto statement = parse_statement(false);
		statements->push_back(move(statement));
	}
}

Statement_ptr Parser::parse_loop_statement()
{
	auto block = parse_block();
	return make_shared<Loop>(block);
}

Statement_ptr Parser::parse_foreach_loop_statement()
{
	auto identifier = token_pipe->consume_token(WTokenType::Identifier);
	ASSERT(!identifier, "Expected an identifier");

	ASSERT(
		!token_pipe->consume_token(WTokenType::IN_KEYWORD),
		"Expected the IN keyword"
	);

	auto iterable_identifier = token_pipe->consume_token(WTokenType::Identifier);
	ASSERT(!iterable_identifier, "Expected an identifier");

	auto block = parse_block();

	return make_shared<ForEachLoop>(identifier->value, iterable_identifier->value, block);
}

Statement_ptr Parser::parse_branching_statement()
{
	auto condition = expr_parser->parse_expression();
	auto consequence = parse_block();

	// empty alternative
	auto alternative = make_shared<vector<Statement_ptr>>();

	if (token_pipe->expect_current_token(WTokenType::ELSE))
	{
		if (token_pipe->expect_current_token(WTokenType::IF))
		{
			auto alternative_stat = parse_branching_statement();
			alternative->push_back(move(alternative_stat));
		}
		else
		{
			alternative = parse_block();
		}
	}

	return make_shared<Branch>(move(condition), consequence, alternative);
}

// Definition Parsers

Statement_ptr Parser::parse_UDT_declaration(bool is_public)
{
	auto name = token_pipe->consume_token(WTokenType::Identifier);
	ASSERT(!name, "Malformed Identifier");

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
			ASSERT(!identifier, "Malformed Identifier");

			ASSERT(
				!token_pipe->expect_current_token(WTokenType::COLON),
				"Expected a COLON"
			);

			auto type = parse_type();
			member_types.insert_or_assign(identifier->value, type);

			token_pipe->ignore(WTokenType::EOL);

			if (token_pipe->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
				return make_shared<UDTDefinition>(is_public, name->value, member_types);

			ASSERT(
				!token_pipe->expect_current_token(WTokenType::COMMA),
				"Expected a COMMA"
			);
		}
	}

	return nullptr;
}

Statement_ptr Parser::parse_function_definition(bool is_public)
{
	auto identifier = token_pipe->consume_token(WTokenType::FunctionIdentifier);
	ASSERT(!identifier, "Malformed Identifier");

	ASSERT(
		!token_pipe->expect_current_token(WTokenType::OPEN_PARENTHESIS),
		"Expected a OPEN_PARENTHESIS"
	);

	vector<pair<string, Type_ptr>> arguments;

	while (true)
	{
		if (token_pipe->expect_current_token(WTokenType::CLOSE_PARENTHESIS))
			break;

		auto identifier = token_pipe->consume_token(WTokenType::Identifier);
		ASSERT(!identifier, "Malformed Identifier");

		ASSERT(
			!token_pipe->expect_current_token(WTokenType::COLON),
			"Expected a COLON"
		);

		auto type = parse_type();
		arguments.push_back(pair(identifier->value, type));
	}

	optional<Type_ptr> optional_return_type = std::nullopt;

	if (token_pipe->consume_token(WTokenType::ARROW))
	{
		auto return_type = parse_type();
		optional_return_type = std::make_optional(return_type);
	}

	token_pipe->ignore(WTokenType::EOL);

	auto block = parse_block();

	return make_shared<FunctionDefinition>(is_public, identifier->value, arguments, move(optional_return_type), block);
}

Statement_ptr Parser::parse_enum_statement(bool is_public)
{
	auto identifier = token_pipe->consume_token(WTokenType::Identifier);
	ASSERT(!identifier, "Expected an Identifier");

	token_pipe->ignore(WTokenType::EOL);

	ASSERT(
		!token_pipe->consume_token(WTokenType::OPEN_CURLY_BRACE),
		"Expected a OPEN_CURLY_BRACE"
	);

	vector<string> members;
	vector<string>::iterator it;

	while (true)
	{
		token_pipe->ignore(WTokenType::EOL);

		auto identifier = token_pipe->consume_token(WTokenType::Identifier);
		ASSERT(!identifier, "Expected an Identifier");

		it = std::find(members.begin(), members.end(), identifier->value);
		ASSERT(it != members.end(), "Duplicate Enum members are present");
		members.push_back(identifier->value);

		token_pipe->ignore(WTokenType::EOL);

		if (token_pipe->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
			break;

		ASSERT(
			!token_pipe->expect_current_token(WTokenType::COMMA),
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
	ASSERT(!type, "Expected a datatype");

	return type;
}

Type_ptr Parser::parse_vector_type()
{
	auto type = parse_type();

	ASSERT(
		!token_pipe->expect_current_token(WTokenType::CLOSE_BRACKET),
		"Expected a CLOSE_BRACKET"
	);

	return make_shared<VectorType>(move(type));
}

Type_ptr Parser::consume_datatype_word()
{
	auto token = token_pipe->get_current_token();
	ASSERT(!token, "Expected a datatype word");

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
		FATAL("Expected a datatype");
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