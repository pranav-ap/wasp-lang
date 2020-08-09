#pragma once
#include "pch.h"
#include "Parser.h"
#include "TokenType.h"
#include "TokenPipe.h"
#include "Assertion.h"
#include "spdlog.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <stack>
#include <map>
#include <memory>
#include <utility>
#include <variant>

#define ADVANCE_PTR token_pipe->advance_pointer()
#define RETREAT_PTR token_pipe->retreat_pointer()
#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define CASE(token_type, call) case token_type: { return call; }
#define MAKE_STATEMENT(x) std::make_shared<Statement>(x)
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)
#define MAKE_TYPE(x) std::make_shared<Type>(x)
#define MAKE_OPTIONAL_TYPE(x) std::make_shared<Type>(VariantType({ std::make_shared<Type>(x), std::make_shared<Type>(NoneType()) }))

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::string;
using std::wstring;
using std::vector;
using std::stack;
using std::map;
using std::shared_ptr;
using std::make_shared;
using std::move;
using std::pair;
using std::make_pair;
using std::tuple;
using std::make_tuple;
using std::optional;
using std::make_optional;
using std::holds_alternative;

using identifier_type_pair_vector = std::vector<std::pair<std::wstring, Type_ptr>>;

Module_ptr Parser::execute(std::vector<Token_ptr>& tokens)
{
	init(tokens);

	auto module_ast = std::make_shared<Module>();

	while ((size_t)token_pipe->get_current_index() < token_pipe->get_size())
	{
		Statement_ptr node = parse_statement(false);

		if (node)
		{
			module_ast->add_statement(move(node));
		}
	}

	return module_ast;
}

void Parser::init(std::vector<Token_ptr>& tokens)
{
	token_pipe = std::make_shared<TokenPipe>(tokens);
	expr_parser = std::make_shared<ExpressionParser>(token_pipe);
}

// Statement Parsers

Statement_ptr Parser::parse_statement(bool is_public, int expected_indent)
{
	token_pipe->ignore({ TokenType::EOL, TokenType::COMMENT });

	if (!token_pipe->has_indent(expected_indent))
		return nullptr;

	auto token = token_pipe->current();

	if (!token.has_value())
		return nullptr;

	ADVANCE_PTR;

	switch (token.value()->type)
	{
		CASE(TokenType::IMPORT, parse_import());

		CASE(TokenType::IF, parse_branching(expected_indent + 4));
		CASE(TokenType::FOR, parse_for_in_loop(expected_indent + 4));
		CASE(TokenType::WHILE, parse_while_loop(expected_indent + 4));

		CASE(TokenType::LET, parse_variable_definition(is_public, true));
		CASE(TokenType::CONST_KEYWORD, parse_variable_definition(is_public, false));
		CASE(TokenType::TYPE, parse_type_definition(is_public, expected_indent + 4));
		CASE(TokenType::FN, parse_function_definition(is_public, expected_indent + 4));
		CASE(TokenType::GEN, parse_generator_definition(is_public, expected_indent + 4));
		CASE(TokenType::ENUM, parse_enum_definition(is_public, expected_indent + 4));
		CASE(TokenType::PUB, parse_public_statement(expected_indent + 4));

		CASE(TokenType::PASS, parse_pass());
		CASE(TokenType::BREAK, parse_break());
		CASE(TokenType::RETURN, parse_return());
		CASE(TokenType::YIELD_KEYWORD, parse_yield());
		CASE(TokenType::CONTINUE, parse_continue());

	default:
	{
		RETREAT_PTR;
		return parse_expression_statement();
	}
	}
}

Statement_ptr Parser::parse_public_statement(int expected_indent)
{
	auto token = token_pipe->current();
	OPT_CHECK(token);

	ADVANCE_PTR;

	const bool is_public = true;

	switch (token.value()->type)
	{
		CASE(TokenType::LET, parse_variable_definition(is_public, true));
		CASE(TokenType::CONST_KEYWORD, parse_variable_definition(is_public, false));

		CASE(TokenType::TYPE, parse_type_definition(is_public, expected_indent));
		CASE(TokenType::FN, parse_function_definition(is_public, expected_indent));
		CASE(TokenType::ENUM, parse_enum_definition(is_public, expected_indent));
	default:
	{
		FATAL(ERROR_CODE::UNEXPECTED_KEYWORD);
	}
	}
}

Statement_ptr Parser::parse_expression_statement()
{
	auto lhs_expressions = expr_parser->parse_expressions();

	if (token_pipe->optional(TokenType::EQUAL))
	{
		auto assignment = consume_assignment(lhs_expressions);
		return assignment;
	}

	ASSERT(lhs_expressions.size() == 1, ERROR_CODE::UNEXPECTED_TOKEN);

	if (token_pipe->optional(TokenType::EOL))
	{
		return MAKE_STATEMENT(ExpressionStatement(move(lhs_expressions.front())));
	}

	auto current_token = token_pipe->current();
	OPT_CHECK(current_token);

	auto expression = lhs_expressions.front();

	switch (current_token.value()->type)
	{
	case TokenType::PLUS_EQUAL:
	case TokenType::MINUS_EQUAL:
	case TokenType::STAR_EQUAL:
	case TokenType::DIVISION_EQUAL:
	case TokenType::REMINDER_EQUAL:
	case TokenType::POWER_EQUAL:
	{
		ADVANCE_PTR;
		return consume_shortcut_assignment(expression, current_token.value());
	}
	default:
	{
		FATAL(ERROR_CODE::MALFORMED_STATEMENT);
	}
	}
}

// Assignment

Statement_ptr Parser::consume_assignment(ExpressionVector lhs_expressions)
{
	auto rhs_expressions = expr_parser->parse_expressions();
	token_pipe->expect(TokenType::EOL);

	return MAKE_STATEMENT(Assignment(lhs_expressions, rhs_expressions));
}

Statement_ptr Parser::consume_shortcut_assignment(Expression_ptr lhs_expression, Token_ptr operator_token)
{
	convert_shortcut_token(operator_token);

	auto rhs_expression = expr_parser->parse_expression();

	auto unshortcut_rhs_expression = MAKE_EXPRESSION(Binary(move(lhs_expression), operator_token, move(rhs_expression)));
	token_pipe->expect(TokenType::EOL);

	return MAKE_STATEMENT(Assignment({ move(lhs_expression) }, { move(unshortcut_rhs_expression) }));
}

// Block statement parsing

Block Parser::parse_block(int expected_indent)
{
	Block statements;

	while (true)
	{
		auto statement = parse_statement(false, expected_indent);

		if (!statement)
			break;

		statements.push_back(move(statement));
	}

	return statements;
}

pair<Expression_ptr, Block> Parser::parse_condition_and_consequence(int expected_indent)
{
	auto condition = expr_parser->parse_expression();
	token_pipe->expect(TokenType::COLON);
	token_pipe->expect(TokenType::EOL);

	auto consequence = parse_block(expected_indent);

	return make_pair(condition, consequence);
}

Statement_ptr Parser::parse_branching(int expected_indent)
{
	std::vector<std::pair<Expression_ptr, Block>> branches;

	auto condition_consequence_pair = parse_condition_and_consequence(expected_indent);
	branches.push_back(condition_consequence_pair);

	while (true)
	{
		if (token_pipe->optional(TokenType::ELIF))
		{
			condition_consequence_pair = parse_condition_and_consequence(expected_indent);
			branches.push_back(condition_consequence_pair);

			continue;
		}

		break;
	}

	Block else_block;

	if (token_pipe->optional(TokenType::ELSE))
	{
		token_pipe->expect(TokenType::COLON);
		token_pipe->expect(TokenType::EOL);
		else_block = parse_block(expected_indent);
	}

	return MAKE_STATEMENT(Branching(branches, else_block));
}

Statement_ptr Parser::parse_while_loop(int expected_indent)
{
	auto condition = expr_parser->parse_expression();
	token_pipe->expect(TokenType::COLON);
	token_pipe->expect(TokenType::EOL);

	auto Block = parse_block(expected_indent);

	return MAKE_STATEMENT(WhileLoop(condition, Block));
}

Statement_ptr Parser::parse_for_in_loop(int expected_indent)
{
	auto [identifier, item_type] = consume_identifier_type_pair();

	token_pipe->expect(TokenType::IN_KEYWORD);

	auto iterable_expression = expr_parser->parse_expression();
	NULL_CHECK(iterable_expression);

	token_pipe->expect(TokenType::COLON);
	token_pipe->expect(TokenType::EOL);

	auto Block = parse_block(expected_indent);

	return MAKE_STATEMENT(ForInLoop(item_type, identifier, iterable_expression, Block));
}

Statement_ptr Parser::parse_return()
{
	if (auto expression = expr_parser->parse_expression())
	{
		token_pipe->expect(TokenType::EOL);
		return MAKE_STATEMENT(Return(move(expression)));
	}

	token_pipe->expect(TokenType::EOL);

	return MAKE_STATEMENT(Return());
}

Statement_ptr Parser::parse_yield()
{
	if (auto expression = expr_parser->parse_expression())
	{
		token_pipe->expect(TokenType::EOL);
		return MAKE_STATEMENT(YieldStatement(move(expression)));
	}

	token_pipe->expect(TokenType::EOL);

	return MAKE_STATEMENT(YieldStatement());
}

Statement_ptr Parser::parse_assert()
{
	auto expression = expr_parser->parse_expression();
	NULL_CHECK(expression);

	token_pipe->expect(TokenType::EOL);

	return MAKE_STATEMENT(AssertStatement(move(expression)));
}

Statement_ptr Parser::parse_break()
{
	token_pipe->expect(TokenType::EOL);
	return MAKE_STATEMENT(Break());
}

Statement_ptr Parser::parse_pass()
{
	token_pipe->expect(TokenType::EOL);
	return MAKE_STATEMENT(Pass());
}

Statement_ptr Parser::parse_continue()
{
	token_pipe->expect(TokenType::EOL);
	return MAKE_STATEMENT(Continue());
}

// Definitions

Statement_ptr Parser::parse_variable_definition(bool is_public, bool is_mutable)
{
	auto [identifier, type] = consume_identifier_type_pair();
	token_pipe->expect(TokenType::EQUAL);
	auto expression = expr_parser->parse_expression();
	token_pipe->expect(TokenType::EOL);

	return MAKE_STATEMENT(VariableDefinition(is_public, is_mutable, identifier, move(type), move(expression)));
}

Statement_ptr Parser::parse_type_definition(bool is_public, int expected_indent)
{
	auto name = token_pipe->required(TokenType::IDENTIFIER);

	if (token_pipe->optional(TokenType::EQUAL))
	{
		auto type = parse_type();
		token_pipe->expect(TokenType::EOL);
		return MAKE_STATEMENT(AliasDefinition(is_public, name->value, move(type)));
	}

	token_pipe->expect(TokenType::COLON);
	token_pipe->expect(TokenType::EOL);

	map<wstring, Type_ptr> member_types;
	map<wstring, bool> is_public_member_map;

	token_pipe->expect_indent(expected_indent);

	if (token_pipe->optional(TokenType::PASS))
	{
		token_pipe->expect(TokenType::EOL);
		return MAKE_STATEMENT(UDTDefinition(is_public, name->value, member_types, is_public_member_map));
	}

	while (true)
	{
		bool is_public_member = false;

		if (token_pipe->optional(TokenType::PUB))
			is_public_member = true;

		auto [identifier, type] = consume_identifier_type_pair();
		member_types.insert_or_assign(identifier, type);
		is_public_member_map.insert_or_assign(identifier, is_public_member);

		token_pipe->expect(TokenType::EOL);

		if (!token_pipe->has_indent(expected_indent))
			break;
	}

	return MAKE_STATEMENT(UDTDefinition(is_public, name->value, member_types, is_public_member_map));
}

tuple<wstring, identifier_type_pair_vector, optional<Type_ptr>, Block> Parser::parse_callable_definition(int expected_indent)
{
	auto identifier = token_pipe->required(TokenType::CALLABLE_IDENTIFIER);
	token_pipe->expect(TokenType::OPEN_PARENTHESIS);

	identifier_type_pair_vector arguments;

	if (!token_pipe->optional(TokenType::CLOSE_PARENTHESIS))
	{
		while (true)
		{
			auto identifier_type_pair = consume_identifier_type_pair();
			arguments.push_back(identifier_type_pair);

			if (token_pipe->optional(TokenType::COMMA))
				continue;

			token_pipe->expect(TokenType::CLOSE_PARENTHESIS);
			break;
		}
	}

	optional<Type_ptr> optional_return_type = std::nullopt;

	if (token_pipe->optional(TokenType::ARROW))
	{
		auto return_type = parse_type();
		optional_return_type = std::make_optional(return_type);
	}

	token_pipe->expect(TokenType::COLON);
	token_pipe->expect(TokenType::EOL);

	Block block = parse_block(expected_indent);

	return make_tuple(identifier->value, arguments, optional_return_type, block);
}

Statement_ptr Parser::parse_function_definition(bool is_public, int expected_indent)
{
	auto [identifier, arguments, optional_return_type, Block] = parse_callable_definition(expected_indent);
	return MAKE_STATEMENT(FunctionDefinition(is_public, identifier, arguments, optional_return_type, Block));
}

Statement_ptr Parser::parse_generator_definition(bool is_public, int expected_indent)
{
	auto [identifier, arguments, optional_return_type, Block] = parse_callable_definition(expected_indent);
	return MAKE_STATEMENT(GeneratorDefinition(is_public, identifier, arguments, optional_return_type, Block));
}

Statement_ptr Parser::parse_enum_definition(bool is_public, int expected_indent)
{
	auto identifier = token_pipe->required(TokenType::IDENTIFIER);
	token_pipe->expect(TokenType::COLON);
	token_pipe->expect(TokenType::EOL);

	auto members = parse_enum_members(expected_indent);

	return MAKE_STATEMENT(EnumDefinition(is_public, identifier->value, members));
}

vector<wstring> Parser::parse_enum_members(int expected_indent)
{
	vector<wstring> members;

	while (true)
	{
		if (!token_pipe->has_indent(expected_indent))
			break;

		auto identifier = token_pipe->required(TokenType::IDENTIFIER);
		members.push_back(identifier->value);

		if (token_pipe->optional(TokenType::COLON))
		{
			token_pipe->expect(TokenType::EOL);
			auto children = parse_enum_members(expected_indent + 4);

			for (auto const& child : children)
			{
				members.push_back(identifier->value + L"::" + child);
			}
		}
		else
		{
			token_pipe->expect(TokenType::EOL);
		}
	}

	return members;
}

// Type Parsers

Type_ptr Parser::parse_type(bool is_optional)
{
	if (token_pipe->optional(TokenType::OPT))
		is_optional = true;

	vector<Type_ptr> types;

	while (true)
	{
		Type_ptr type;

		if (token_pipe->optional(TokenType::OPEN_SQUARE_BRACKET))
		{
			type = parse_list_type(is_optional);
		}
		else if (token_pipe->optional(TokenType::OPEN_ANGLE_BRACKET))
		{
			type = parse_map_type(is_optional);
		}
		else if (token_pipe->optional(TokenType::OPEN_FLOOR_BRACKET))
		{
			type = parse_tuple_type(is_optional);
		}
		else
		{
			type = consume_datatype_word(is_optional);
		}

		types.push_back(type);

		if (token_pipe->optional(TokenType::VERTICAL_BAR))
		{
			continue;
		}

		if (types.size() > 1)
			return MAKE_TYPE(VariantType(types));

		return move(types.front());
	}
}

Type_ptr Parser::parse_list_type(bool is_optional)
{
	vector<Type_ptr> types;

	while (true)
	{
		auto type = parse_type();
		types.push_back(type);

		if (token_pipe->optional(TokenType::COMMA))
			continue;

		token_pipe->expect(TokenType::CLOSE_SQUARE_BRACKET);
		break;
	}

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(ListType(move(types.front())));
	}

	return MAKE_TYPE(ListType(move(types.front())));
}

Type_ptr Parser::parse_tuple_type(bool is_optional)
{
	vector<Type_ptr> types;

	while (true)
	{
		auto type = parse_type();
		types.push_back(type);

		if (token_pipe->optional(TokenType::COMMA))
			continue;

		token_pipe->expect(TokenType::CLOSE_FLOOR_BRACKET);
		break;
	}

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(TupleType(types));
	}

	return MAKE_TYPE(TupleType(types));
}

Type_ptr Parser::parse_map_type(bool is_optional)
{
	auto key_type = parse_type();
	token_pipe->expect(TokenType::ARROW);

	auto value_type = parse_type();
	token_pipe->expect(TokenType::CLOSE_ANGLE_BRACKET);

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(MapType(move(key_type), move(value_type)));
	}

	return MAKE_TYPE(MapType(move(key_type), move(value_type)));
}

Type_ptr Parser::consume_datatype_word(bool is_optional)
{
	auto token = token_pipe->current();
	OPT_CHECK(token);

	switch (token.value()->type)
	{
	case TokenType::NUM:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(NumberType()) : MAKE_TYPE(NumberType());
	}
	case TokenType::STR:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(StringType()) : MAKE_TYPE(StringType());
	}
	case TokenType::BOOL:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(BooleanType()) : MAKE_TYPE(BooleanType());
	}
	case TokenType::ANY:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(AnyType()) : MAKE_TYPE(AnyType());
	}
	case TokenType::IDENTIFIER:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(UDTType(token.value()->value)) : MAKE_TYPE(UDTType(token.value()->value));
	}
	}

	FATAL(ERROR_CODE::EXPECTED_DATATYPE);
}

pair<wstring, Type_ptr>  Parser::consume_identifier_type_pair()
{
	auto identifier = token_pipe->required(TokenType::IDENTIFIER);
	token_pipe->expect(TokenType::COLON);
	auto type = parse_type();

	return make_pair(identifier->value, move(type));
}

// Other

Statement_ptr Parser::parse_import()
{
	token_pipe->expect(TokenType::OPEN_CURLY_BRACE);

	StringVector goods;

	while (true)
	{
		auto identifier = token_pipe->required(TokenType::IDENTIFIER);
		goods.push_back(identifier->value);

		if (token_pipe->optional(TokenType::CLOSE_CURLY_BRACE))
			break;

		token_pipe->expect(TokenType::COMMA);
	}

	token_pipe->expect(TokenType::FROM);

	if (auto current = token_pipe->optional(TokenType::IDENTIFIER))
	{
		token_pipe->expect(TokenType::EOL);
		return MAKE_STATEMENT(ImportInBuilt(current.value()->value, goods));
	}

	auto path_token = token_pipe->required(TokenType::STRING_LITERAL);
	token_pipe->expect(TokenType::EOL);

	return MAKE_STATEMENT(ImportCustom(path_token->value, goods));
}

// Utils

void Parser::convert_shortcut_token(Token_ptr token)
{
	switch (token->type)
	{
	case TokenType::PLUS_EQUAL:
	{
		token->type = TokenType::PLUS;
		token->value = L"+";
		break;
	}
	case TokenType::MINUS_EQUAL: {
		token->type = TokenType::MINUS;
		token->value = L"-";
		break;
	}
	case TokenType::STAR_EQUAL:
	{
		token->type = TokenType::STAR;
		token->value = L"*";
		break;
	}
	case TokenType::DIVISION_EQUAL:
	{
		token->type = TokenType::DIVISION;
		token->value = L"/";
		break;
	}
	case TokenType::REMINDER_EQUAL:
	{
		token->type = TokenType::REMINDER;
		token->value = L"%";
		break;
	}
	case TokenType::POWER_EQUAL:
	{
		token->type = TokenType::POWER;
		token->value = L"^";
		break;
	}
	}

	FATAL(ERROR_CODE::UNEXPECTED_TOKEN);
}