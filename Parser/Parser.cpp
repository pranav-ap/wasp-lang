#pragma once
#include "pch.h"
#include "Parser.h"
#include "TokenType.h"
#include "TokenPipe.h"
#include "CommonAssertion.h"

#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include <memory>
#include <utility>
#include <variant>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define CASE(token_type, call) case token_type: { return call; }
#define MAKE_STATEMENT(x) std::make_shared<Statement>(x)
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)
#define MAKE_TYPE(x) std::make_shared<Type>(x)

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
using std::holds_alternative;

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
	auto token = token_pipe->get_significant_token();

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
	default:
	{
		RETREAT_PTR;
		return parse_expression_statement();
	}
	}
}

Statement_ptr Parser::parse_public_statement()
{
	auto token = token_pipe->get_significant_token();
	NULL_CHECK(token);

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

	if (expression)
	{
		return MAKE_STATEMENT(Return(move(expression)));
	}

	return MAKE_STATEMENT(Return());
}

Statement_ptr Parser::parse_break_statement()
{
	return MAKE_STATEMENT(Break());
}

Statement_ptr Parser::parse_continue_statement()
{
	return MAKE_STATEMENT(Continue());
}

Statement_ptr Parser::parse_variable_declaration(bool is_public, bool is_mutable)
{
	auto identifier = token_pipe->consume_required_token(WTokenType::Identifier);

	ASSERT(
		token_pipe->next_significant_token_is(WTokenType::COLON),
		"Expected a COLON"
	);

	auto type = parse_type();

	ASSERT(
		token_pipe->next_significant_token_is(WTokenType::EQUAL),
		"Expected a EQUAL"
	);

	Expression_ptr expression = expr_parser->parse_expression();

	return MAKE_STATEMENT(VariableDefinition(is_public, is_mutable, identifier->value, move(type), move(expression)));
}

// Expression Parsers

Statement_ptr Parser::parse_expression_statement()
{
	auto expression = expr_parser->parse_expression();
	return MAKE_STATEMENT(ExpressionStatement(move(expression)));
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
		return MAKE_STATEMENT(Assignment(identifier->value, move(expression)));
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
		convert_shortcut_token(current_token);

		auto identifier_expression = MAKE_EXPRESSION(Identifier(identifier->value));
		auto rhs = MAKE_EXPRESSION(Binary(identifier_expression, current_token, move(expression)));

		return MAKE_STATEMENT(Assignment(identifier->value, move(rhs)));
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
		token_pipe->next_significant_token_is(WTokenType::OPEN_CURLY_BRACE),
		"Expected a OPEN_CURLY_BRACE"
	);

	vector<string> goods;

	while (true)
	{
		auto identifier = token_pipe->consume_required_token(WTokenType::Identifier);

		goods.push_back(identifier->value);

		if (token_pipe->next_significant_token_is(WTokenType::CLOSE_CURLY_BRACE))
			break;

		ASSERT(
			token_pipe->next_significant_token_is(WTokenType::COMMA),
			"Expected a COMMA or a CLOSE_CURLY_BRACE"
		);
	}

	ASSERT(
		token_pipe->next_significant_token_is(WTokenType::FROM),
		"Expected the FROM keyword"
	);

	auto current_token = token_pipe->consume_optional_token(WTokenType::Identifier);

	if (current_token)
	{
		return MAKE_STATEMENT(ImportInBuilt(current_token->value, goods));
	}

	auto path_token = token_pipe->consume_required_token(WTokenType::StringLiteral);
	return MAKE_STATEMENT(ImportCustom(path_token->value, goods));
}

// Block Statements Parsers

Block Parser::parse_block()
{
	ASSERT(
		token_pipe->next_significant_token_is(WTokenType::OPEN_CURLY_BRACE),
		"Expected a OPEN_CURLY_BRACE"
	);

	Block statements;

	while (true)
	{
		if (token_pipe->next_significant_token_is(WTokenType::CLOSE_CURLY_BRACE))
			return statements;

		auto statement = parse_statement(false);
		statements.push_back(move(statement));
	}
}

Statement_ptr Parser::parse_loop_statement()
{
	auto block = parse_block();
	return MAKE_STATEMENT(Loop(block));
}

Statement_ptr Parser::parse_foreach_loop_statement()
{
	auto identifier = token_pipe->consume_required_token(WTokenType::Identifier);

	ASSERT(
		token_pipe->next_significant_token_is(WTokenType::IN_KEYWORD),
		"Expected the IN keyword"
	);

	auto iterable_expression = expr_parser->parse_expression();
	NULL_CHECK(iterable_expression);

	auto block = parse_block();

	return MAKE_STATEMENT(ForEachLoop(identifier->value, iterable_expression, block));
}

Statement_ptr Parser::parse_branching_statement()
{
	auto condition = expr_parser->parse_expression();
	auto consequence = parse_block();

	Block alternative;

	if (token_pipe->next_significant_token_is(WTokenType::ELSE))
	{
		if (token_pipe->next_significant_token_is(WTokenType::IF))
		{
			auto alternative_stat = parse_branching_statement();
			alternative.push_back(move(alternative_stat));
		}
		else
		{
			alternative = parse_block();
		}
	}

	return MAKE_STATEMENT(ConditionalBranch(condition, consequence, alternative));
}

// Definition Parsers

Statement_ptr Parser::parse_UDT_declaration(bool is_public)
{
	auto name = token_pipe->consume_required_token(WTokenType::Identifier);

	if (token_pipe->next_significant_token_is(WTokenType::OPEN_CURLY_BRACE))
	{
		map<string, Type_ptr> member_types;

		if (token_pipe->next_significant_token_is(WTokenType::CLOSE_CURLY_BRACE))
			return MAKE_STATEMENT(UDTDefinition(is_public, name->value, member_types));

		while (true)
		{
			auto identifier = token_pipe->consume_required_token(WTokenType::Identifier);

			ASSERT(
				token_pipe->next_significant_token_is(WTokenType::COLON),
				"Expected a COLON"
			);

			auto type = parse_type();
			member_types.insert_or_assign(identifier->value, type);

			if (token_pipe->next_significant_token_is(WTokenType::CLOSE_CURLY_BRACE))
				return MAKE_STATEMENT(UDTDefinition(is_public, name->value, member_types));

			ASSERT(
				token_pipe->next_significant_token_is(WTokenType::COMMA),
				"Expected a COMMA"
			);
		}
	}

	return nullptr;
}

Statement_ptr Parser::parse_function_definition(bool is_public)
{
	auto identifier = token_pipe->consume_required_token(WTokenType::FunctionIdentifier);

	ASSERT(
		token_pipe->next_significant_token_is(WTokenType::OPEN_PARENTHESIS),
		"Expected a OPEN_PARENTHESIS"
	);

	vector<pair<string, Type_ptr>> arguments;

	while (true)
	{
		if (token_pipe->next_significant_token_is(WTokenType::CLOSE_PARENTHESIS))
			break;

		auto identifier = token_pipe->consume_required_token(WTokenType::Identifier);

		ASSERT(
			token_pipe->next_significant_token_is(WTokenType::COLON),
			"Expected a COLON"
		);

		auto type = parse_type();
		arguments.push_back(pair(identifier->value, type));
	}

	optional<Type_ptr> optional_return_type = std::nullopt;

	if (token_pipe->consume_optional_token(WTokenType::ARROW))
	{
		auto return_type = parse_type();
		optional_return_type = std::make_optional(return_type);
	}

	auto block = parse_block();

	return MAKE_STATEMENT(FunctionDefinition(is_public, identifier->value, arguments, move(optional_return_type), block));
}

Statement_ptr Parser::parse_enum_statement(bool is_public)
{
	auto identifier = token_pipe->consume_required_token(WTokenType::Identifier);

	ASSERT(
		token_pipe->next_significant_token_is(WTokenType::OPEN_CURLY_BRACE),
		"Expected a OPEN_CURLY_BRACE"
	);

	vector<string> members;

	while (true)
	{
		auto identifier = token_pipe->consume_required_token(WTokenType::Identifier);
		members.push_back(identifier->value);

		if (token_pipe->next_significant_token_is(WTokenType::CLOSE_CURLY_BRACE))
			break;

		ASSERT(
			token_pipe->next_significant_token_is(WTokenType::COMMA),
			"Expected a COMMA"
		);
	}

	return MAKE_STATEMENT(EnumDefinition(is_public, identifier->value, members));
}

// Type Parsers

Type_ptr Parser::parse_type()
{
	if (token_pipe->next_significant_token_is(WTokenType::OPEN_BRACKET))
	{
		return parse_list_type();
	}
	else if (token_pipe->next_significant_token_is(WTokenType::OPEN_CURLY_BRACE))
	{
		return parse_map_type();
	}
	else if (token_pipe->next_significant_token_is(WTokenType::OPEN_PARENTHESIS))
	{
		return parse_tuple_type();
	}

	return consume_datatype_word();
}

Type_ptr Parser::parse_list_type()
{
	auto type = parse_type();

	ASSERT(
		token_pipe->next_significant_token_is(WTokenType::CLOSE_BRACKET),
		"Expected a CLOSE_BRACKET"
	);

	return MAKE_TYPE(ListType(move(type)));
}

Type_ptr Parser::parse_tuple_type()
{
	vector<Type_ptr> types;

	while (true)
	{
		auto type = parse_type();
		types.push_back(type);

		if (token_pipe->next_significant_token_is(WTokenType::COMMA))
		{
			continue;
		}

		ASSERT(
			token_pipe->next_significant_token_is(WTokenType::CLOSE_PARENTHESIS),
			"Expected a CLOSE_PARENTHESIS"
		);
	}

	return MAKE_TYPE(TupleType(types));
}

Type_ptr Parser::parse_map_type()
{
	auto key_type = parse_type();

	ASSERT(
		token_pipe->next_significant_token_is(WTokenType::ARROW),
		"Expected a ARROW"
	);

	auto value_type = parse_type();

	ASSERT(
		token_pipe->next_significant_token_is(WTokenType::CLOSE_CURLY_BRACE),
		"Expected a CLOSE_CURLY_BRACE"
	);

	return MAKE_TYPE(MapType(move(key_type), move(value_type)));
}

Type_ptr Parser::consume_datatype_word()
{
	auto token = token_pipe->get_significant_token();
	NULL_CHECK(token);

	switch (token->type)
	{
	case WTokenType::NUM:
	{
		ADVANCE_PTR;
		return MAKE_TYPE(NumberType());
	}
	case WTokenType::STR:
	{
		ADVANCE_PTR;
		return MAKE_TYPE(StringType());
	}
	case WTokenType::BOOL:
	{
		ADVANCE_PTR;
		return MAKE_TYPE(BooleanType());
	}
	case WTokenType::Identifier:
	{
		ADVANCE_PTR;
		return MAKE_TYPE(UDTType(token->value));
	}
	default:
	{
		FATAL("Expected a datatype");
	}
	}
}

// Utils

void Parser::convert_shortcut_token(Token_ptr token)
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

	FATAL(token->value + " is not a shortcut token");
}