#pragma once
#include "pch.h"
#include "Parser.h"
#include "TokenType.h"
#include "TokenPipe.h"
#include "StatementContext.h"
#include "CommonAssertion.h"

#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include <memory>
#include <utility>
#include <variant>

#define ADVANCE_PTR token_pipe->advance_pointer()
#define RETREAT_PTR token_pipe->retreat_pointer()
#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define CASE(token_type, call) case token_type: { return call; }
#define MAKE_STATEMENT(x) std::make_shared<Statement>(x)
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)
#define MAKE_TYPE(x) std::make_shared<Type>(x)
#define IGNORABLES_TYPE_1 { WTokenType::EOL }

using std::string;
using std::vector;
using std::stack;
using std::map;
using std::shared_ptr;
using std::make_shared;
using std::move;
using std::pair;
using std::make_pair;
using std::optional;
using std::make_optional;
using std::holds_alternative;

// API

Module Parser::execute()
{
	Module mod;

	context_stack.push({ StatementContext::GLOBAL, 0 });

	while ((size_t)token_pipe->get_pointer_index() < token_pipe->get_size())
	{
		Statement_ptr node = parse_statement(false);

		if (node)
		{
			mod.add(move(node));
		}
	}

	ASSERT(context_stack.size() == 1, "Context stack is not in global state");
	context_stack.pop();

	return mod;
}

// Statement Parsers

Statement_ptr Parser::parse_statement(bool is_public)
{
	token_pipe->skip_empty_lines();

	// Handle Indentation

	const int current_indent = token_pipe->consume_spaces();
	const int expected_indent = context_stack.top().second;

	auto token = token_pipe->current();

	if (token == nullptr)
		return nullptr;

	ASSERT(current_indent == expected_indent, "Cannot change indentation for no reason");

	// Parse

	ADVANCE_PTR;

	switch (token->type)
	{
		CASE(WTokenType::IMPORT, parse_import());
		CASE(WTokenType::LET, parse_variable_definition(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, parse_variable_definition(is_public, false));
		CASE(WTokenType::PASS, parse_pass());
		CASE(WTokenType::BREAK, parse_break());
		CASE(WTokenType::CONTINUE, parse_continue());
		CASE(WTokenType::WHILE, parse_while_loop());
	default:
	{
	}
	}
}

Statement_ptr Parser::parse_import()
{
	token_pipe->expect(WTokenType::OPEN_CURLY_BRACE);

	string_vector goods;

	while (true)
	{
		auto identifier = token_pipe->required(WTokenType::Identifier);
		goods.push_back(identifier->value);

		if (token_pipe->optional(WTokenType::CLOSE_CURLY_BRACE))
			break;

		token_pipe->expect(WTokenType::COMMA);
	}

	token_pipe->expect(WTokenType::FROM);

	if (auto current = token_pipe->optional(WTokenType::Identifier))
	{
		return MAKE_STATEMENT(ImportInBuilt(current->value, goods));
	}

	auto path_token = token_pipe->required(WTokenType::StringLiteral);
	return MAKE_STATEMENT(ImportCustom(path_token->value, goods));
}

Statement_ptr Parser::parse_variable_definition(bool is_public, bool is_mutable)
{
	auto identifier = token_pipe->required(WTokenType::Identifier);
	token_pipe->expect(WTokenType::COLON);

	auto type = parse_type();
	token_pipe->expect(WTokenType::EQUAL);

	Expression_ptr expression = expr_parser->parse_expression();

	return MAKE_STATEMENT(VariableDefinition(is_public, is_mutable, identifier->value, move(type), move(expression)));
}

// Block statement parsing

Block Parser::parse_block()
{
	Block statements;

	while (auto statement = parse_statement(false))
	{
		statements.push_back(move(statement));
	}

	return statements;
}

Statement_ptr Parser::parse_while_loop()
{
	token_pipe->skip_empty_lines();

	auto condition = expr_parser->parse_expression();
	token_pipe->expect(WTokenType::COLON);

	const auto previous_indent = context_stack.top().second;
	context_stack.push({ StatementContext::LOOP, previous_indent + 4 });

	auto block = parse_block();

	ASSERT(context_stack.top().first == StatementContext::LOOP, "While Loop Context mismatch");
	context_stack.pop();

	return MAKE_STATEMENT(WhileLoop(condition, block));
}

Statement_ptr Parser::parse_return()
{
	if (auto expression = expr_parser->parse_expression())
	{
		return MAKE_STATEMENT(Return(move(expression)));
	}

	return MAKE_STATEMENT(Return());
}

Statement_ptr Parser::parse_break()
{
	return MAKE_STATEMENT(Break());
}

Statement_ptr Parser::parse_pass()
{
	return MAKE_STATEMENT(Pass());
}

Statement_ptr Parser::parse_continue()
{
	return MAKE_STATEMENT(Continue());
}

// Type Parsers

Type_ptr Parser::parse_type()
{
	if (token_pipe->optional(WTokenType::OPEN_SQUARE_BRACKET))
	{
		return parse_list_type();
	}
	else if (token_pipe->optional(WTokenType::OPEN_CURLY_BRACE))
	{
		return parse_map_type();
	}
	else if (token_pipe->optional(WTokenType::OPEN_PARENTHESIS))
	{
		return parse_tuple_type();
	}

	return consume_datatype_word();
}

Type_ptr Parser::parse_list_type()
{
	auto type = parse_type();

	token_pipe->expect(WTokenType::CLOSE_SQUARE_BRACKET);

	return MAKE_TYPE(ListType(move(type)));
}

Type_ptr Parser::parse_tuple_type()
{
	vector<Type_ptr> types;

	while (true)
	{
		auto type = parse_type();
		types.push_back(type);

		if (token_pipe->optional(WTokenType::COMMA))
		{
			continue;
		}

		token_pipe->expect(WTokenType::CLOSE_PARENTHESIS);
	}

	return MAKE_TYPE(TupleType(types));
}

Type_ptr Parser::parse_map_type()
{
	auto key_type = parse_type();
	token_pipe->expect(WTokenType::ARROW);

	auto value_type = parse_type();
	token_pipe->expect(WTokenType::CLOSE_CURLY_BRACE);

	return MAKE_TYPE(MapType(move(key_type), move(value_type)));
}

Type_ptr Parser::consume_datatype_word()
{
	auto token = token_pipe->current();
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