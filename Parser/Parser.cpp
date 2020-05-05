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
#include <tuple>
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
#define MAKE_OPTIONAL_TYPE(x) std::make_shared<Type>(VariantType({ std::make_shared<Type>(x), std::make_shared<Type>(NoneType()) }))

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

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

	pop_context(StatementContext::GLOBAL);

	return mod;
}

// Statement Parsers

Statement_ptr Parser::parse_statement(bool is_public)
{
	token_pipe->skip_empty_lines();

	auto [current_indent, expected_indent] = get_indent_pair();

	if (current_indent < expected_indent)
		return nullptr;

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
		CASE(WTokenType::Identifier, parse_assignment_or_expression(move(token)));

		CASE(WTokenType::IF, parse_branching());
		CASE(WTokenType::FOR, parse_for_in_loop());
		CASE(WTokenType::WHILE, parse_while_loop());
		CASE(WTokenType::TYPE, parse_UDT_definition(is_public));
		CASE(WTokenType::FN, parse_function_definition(is_public));
		CASE(WTokenType::ENUM, parse_enum_definition(is_public));

		CASE(WTokenType::PASS, parse_pass());
		CASE(WTokenType::BREAK, parse_break());
		CASE(WTokenType::RETURN, parse_return());
		CASE(WTokenType::CONTINUE, parse_continue());
		CASE(WTokenType::PUB, parse_public_statement());
	default:
	{
		RETREAT_PTR;
		return parse_expression_statement();
	}
	}
}

Statement_ptr Parser::parse_public_statement()
{
	auto token = token_pipe->current();
	NULL_CHECK(token);

	ADVANCE_PTR;

	const bool is_public = true;

	switch (token->type)
	{
		CASE(WTokenType::LET, parse_variable_definition(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, parse_variable_definition(is_public, false));
		CASE(WTokenType::TYPE, parse_UDT_definition(is_public));
		CASE(WTokenType::FN, parse_function_definition(is_public));
		CASE(WTokenType::ENUM, parse_enum_definition(is_public));
	default:
	{
		FATAL(token->value + " cannot be made public");
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

Statement_ptr Parser::parse_expression_statement()
{
	auto expression = expr_parser->parse_expression();
	return MAKE_STATEMENT(ExpressionStatement(move(expression)));
}

// Assignment

Statement_ptr Parser::parse_assignment_or_expression(Token_ptr identifier)
{
	auto current_token = token_pipe->current();

	WTokenType current_token_type = current_token ? current_token->type : WTokenType::UNKNOWN;

	switch (current_token_type)
	{
	case WTokenType::EQUAL:
	{
		ADVANCE_PTR;
		return consume_assignment(identifier);
	}
	case WTokenType::COMMA:
	{
		RETREAT_PTR;
		return consume_multiple_assignment(identifier);
	}
	case WTokenType::PLUS_EQUAL:
	case WTokenType::MINUS_EQUAL:
	case WTokenType::STAR_EQUAL:
	case WTokenType::DIVISION_EQUAL:
	case WTokenType::REMINDER_EQUAL:
	case WTokenType::POWER_EQUAL:
	{
		ADVANCE_PTR;
		return consume_shortcut_assignment(identifier, current_token);
	}
	default:
	{
		RETREAT_PTR;
		return parse_expression_statement();
	}
	}
}

Statement_ptr Parser::consume_assignment(Token_ptr identifier)
{
	auto expression = expr_parser->parse_expression();
	return MAKE_STATEMENT(Assignment(identifier->value, move(expression)));
}

Statement_ptr Parser::consume_shortcut_assignment(Token_ptr identifier, Token_ptr shortcut_operator)
{
	auto expression = expr_parser->parse_expression();
	convert_shortcut_token(shortcut_operator);

	auto identifier_expression = MAKE_EXPRESSION(Identifier(identifier->value));
	auto rhs = MAKE_EXPRESSION(Binary(identifier_expression, shortcut_operator, move(expression)));

	return MAKE_STATEMENT(Assignment(identifier->value, move(rhs)));
}

Statement_ptr Parser::consume_multiple_assignment(Token_ptr identifier)
{
	auto lhs_list = expr_parser->parse_expressions();
	token_pipe->expect(WTokenType::EQUAL);
	auto expressions = expr_parser->parse_expressions();
	return MAKE_STATEMENT(MultipleAssignment(lhs_list, expressions));
}

// Block statement parsing

Block Parser::parse_block(StatementContext context)
{
	push_context(context);

	Block statements;

	while (true)
	{
		auto statement = parse_statement(false);

		if (!statement)
			break;

		statements.push_back(move(statement));
	}

	pop_context(context);

	return statements;
}

pair<Expression_ptr, Block> Parser::parse_condition_and_consequence()
{
	auto condition = expr_parser->parse_expression();
	token_pipe->expect(WTokenType::COLON);
	auto consequence = parse_block(StatementContext::BRANCH);

	return make_pair(condition, consequence);
}

Statement_ptr Parser::parse_branching()
{
	std::vector<std::pair<Expression_ptr, Block>> branches;

	auto condition_consequence_pair = parse_condition_and_consequence();
	branches.push_back(condition_consequence_pair);

	while (true)
	{
		token_pipe->skip_empty_lines();

		if (token_pipe->optional(WTokenType::ELIF))
		{
			condition_consequence_pair = parse_condition_and_consequence();
			branches.push_back(condition_consequence_pair);

			continue;
		}

		break;
	}

	token_pipe->skip_empty_lines();

	Block else_branch;

	if (token_pipe->optional(WTokenType::ELSE))
	{
		token_pipe->expect(WTokenType::COLON);
		else_branch = parse_block(StatementContext::BRANCH);
	}

	return MAKE_STATEMENT(Branching(branches, else_branch));
}

Statement_ptr Parser::parse_while_loop()
{
	auto condition = expr_parser->parse_expression();
	token_pipe->expect(WTokenType::COLON);

	auto block = parse_block(StatementContext::LOOP);

	return MAKE_STATEMENT(WhileLoop(condition, block));
}

Statement_ptr Parser::parse_for_in_loop()
{
	auto [identifier, item_type] = consume_identifier_type_pair();

	token_pipe->expect(WTokenType::IN_KEYWORD);

	auto iterable_expression = expr_parser->parse_expression();
	NULL_CHECK(iterable_expression);

	token_pipe->expect(WTokenType::COLON);

	auto block = parse_block(StatementContext::LOOP);

	return MAKE_STATEMENT(ForInLoop(item_type, identifier, iterable_expression, block));
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

// Definitions

Statement_ptr Parser::parse_variable_definition(bool is_public, bool is_mutable)
{
	auto [identifier, type] = consume_identifier_type_pair();
	token_pipe->expect(WTokenType::EQUAL);
	Expression_ptr expression = expr_parser->parse_expression();

	return MAKE_STATEMENT(VariableDefinition(is_public, is_mutable, identifier, move(type), move(expression)));
}

Statement_ptr Parser::parse_UDT_definition(bool is_public)
{
	auto name = token_pipe->required(WTokenType::Identifier);
	token_pipe->expect(WTokenType::COLON);

	push_context(StatementContext::UDT_DEFINITION);

	auto [current_indent, expected_indent] = get_indent_pair();
	ASSERT(current_indent == expected_indent, "Cannot change indentation for no reason");

	map<string, Type_ptr> member_types;
	std::map<std::string, bool> is_public_member_map;

	if (token_pipe->optional(WTokenType::PASS))
		return MAKE_STATEMENT(UDTDefinition(is_public, name->value, member_types, is_public_member_map));

	while (true)
	{
		bool is_public_member = false;

		if (token_pipe->optional(WTokenType::PUB))
			is_public_member = true;

		auto [identifier, type] = consume_identifier_type_pair();
		member_types.insert_or_assign(identifier, type);
		is_public_member_map.insert_or_assign(identifier, is_public_member);

		auto [current_indent, expected_indent] = get_indent_pair();

		if (current_indent < expected_indent)
			break;

		ASSERT(current_indent == expected_indent, "Cannot change indentation for no reason");
	}

	pop_context(StatementContext::UDT_DEFINITION);

	return MAKE_STATEMENT(UDTDefinition(is_public, name->value, member_types, is_public_member_map));
}

Statement_ptr Parser::parse_function_definition(bool is_public)
{
	auto identifier = token_pipe->required(WTokenType::FunctionIdentifier);
	token_pipe->expect(WTokenType::OPEN_PARENTHESIS);

	vector<pair<string, Type_ptr>> arguments;

	if (!token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
	{
		while (true)
		{
			auto identifier_type_pair = consume_identifier_type_pair();
			arguments.push_back(identifier_type_pair);

			if (token_pipe->optional(WTokenType::COMMA))
				continue;

			token_pipe->expect(WTokenType::CLOSE_PARENTHESIS);
			break;
		}
	}

	optional<Type_ptr> optional_return_type = std::nullopt;

	if (token_pipe->optional(WTokenType::ARROW))
	{
		auto return_type = parse_type();
		optional_return_type = std::make_optional(return_type);
	}

	token_pipe->expect(WTokenType::COLON);

	auto block = parse_block(StatementContext::FUNCTION_DEFINITION);

	return MAKE_STATEMENT(FunctionDefinition(is_public, identifier->value, arguments, move(optional_return_type), block));
}

Statement_ptr Parser::parse_enum_definition(bool is_public)
{
	auto identifier = token_pipe->required(WTokenType::Identifier);
	token_pipe->expect(WTokenType::COLON);

	vector<string> members = parse_enum_members();

	return MAKE_STATEMENT(EnumDefinition(is_public, identifier->value, members));
}

vector<string> Parser::parse_enum_members()
{
	push_context(StatementContext::ENUM_DEFINITION);

	vector<string> members;

	while (true)
	{
		token_pipe->skip_empty_lines();

		auto [current_indent, expected_indent] = get_indent_pair();

		if (current_indent < expected_indent)
			break;

		ASSERT(current_indent == expected_indent, "Cannot change indentation for no reason");

		auto identifier = token_pipe->required(WTokenType::Identifier);
		members.push_back(identifier->value);

		if (token_pipe->optional(WTokenType::COLON))
		{
			auto children = parse_enum_members();

			for (auto const& child : children)
				members.push_back(identifier->value + "::" + child);
		}
	}

	pop_context(StatementContext::ENUM_DEFINITION);

	return members;
}

// Type Parsers

Type_ptr Parser::parse_type(bool is_optional)
{
	if (token_pipe->optional(WTokenType::OPT))
		is_optional = true;

	if (token_pipe->optional(WTokenType::OPEN_SQUARE_BRACKET))
	{
		return parse_list_type(is_optional);
	}
	else if (token_pipe->optional(WTokenType::OPEN_CURLY_BRACE))
	{
		return parse_map_type(is_optional);
	}
	else if (token_pipe->optional(WTokenType::OPEN_PARENTHESIS))
	{
		return parse_tuple_type(is_optional);
	}

	return consume_datatype_word(is_optional);
}

Type_ptr Parser::parse_list_type(bool is_optional)
{
	auto type = parse_type();
	token_pipe->expect(WTokenType::CLOSE_SQUARE_BRACKET);

	if (is_optional)
		return MAKE_OPTIONAL_TYPE(ListType(move(type)));

	return MAKE_TYPE(ListType(move(type)));
}

Type_ptr Parser::parse_tuple_type(bool is_optional)
{
	vector<Type_ptr> types;

	while (true)
	{
		auto type = parse_type();
		types.push_back(type);

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		token_pipe->expect(WTokenType::CLOSE_PARENTHESIS);
	}

	if (is_optional)
		return MAKE_OPTIONAL_TYPE(TupleType(types));

	return MAKE_TYPE(TupleType(types));
}

Type_ptr Parser::parse_map_type(bool is_optional)
{
	auto key_type = parse_type();
	token_pipe->expect(WTokenType::ARROW);

	auto value_type = parse_type();
	token_pipe->expect(WTokenType::CLOSE_CURLY_BRACE);

	if (is_optional)
		return MAKE_OPTIONAL_TYPE(MapType(move(key_type), move(value_type)));

	return MAKE_TYPE(MapType(move(key_type), move(value_type)));
}

Type_ptr Parser::consume_datatype_word(bool is_optional)
{
	auto token = token_pipe->current();
	NULL_CHECK(token);

	switch (token->type)
	{
	case WTokenType::NUM:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(NumberType()) : MAKE_TYPE(NumberType());
	}
	case WTokenType::STR:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(StringType()) : MAKE_TYPE(StringType());
	}
	case WTokenType::BOOL:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(BooleanType()) : MAKE_TYPE(BooleanType());
	}
	case WTokenType::Identifier:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(UDTType(token->value)) : MAKE_TYPE(UDTType(token->value));
	}
	default:
	{
		FATAL("Expected a datatype");
	}
	}
}

pair<std::string, Type_ptr>  Parser::consume_identifier_type_pair()
{
	auto identifier = token_pipe->required(WTokenType::Identifier);
	token_pipe->expect(WTokenType::COLON);
	auto type = parse_type();

	return make_pair(identifier->value, move(type));
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

pair<int, int> Parser::get_indent_pair()
{
	int current_indent = token_pipe->consume_spaces();
	int expected_indent = context_stack.top().second;

	return make_pair(current_indent, expected_indent);
}

void Parser::push_context(StatementContext context)
{
	token_pipe->skip_empty_lines();

	int previous_indent = context_stack.top().second;
	context_stack.push({ context, previous_indent + 4 });
}

void Parser::pop_context(StatementContext context)
{
	ASSERT(context_stack.top().first == context, "Context Mismatch");
	context_stack.pop();
}