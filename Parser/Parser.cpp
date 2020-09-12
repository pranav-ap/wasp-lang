#pragma once
#include "pch.h"
#include "Parser.h"
#include "WTokenType.h"
#include "TokenPipe.h"
#include "Assertion.h"

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

Parser::Parser()
{
	register_parselet(WTokenType::EQUAL, make_shared<AssignmentParselet>());

	register_parselet(WTokenType::IDENTIFIER, make_shared<IdentifierParselet>());
	register_parselet(WTokenType::STRING_LITERAL, make_shared<StringParselet>());
	register_parselet(WTokenType::NUMBER_LITERAL, make_shared<NumberParselet>());

	register_parselet(WTokenType::TRUE_KEYWORD, make_shared<BooleanParselet>());
	register_parselet(WTokenType::FALSE_KEYWORD, make_shared<BooleanParselet>());

	register_parselet(WTokenType::OPEN_PARENTHESIS, make_shared<CallParselet>());
	register_parselet(WTokenType::OPEN_PARENTHESIS, make_shared<GroupParselet>());
	register_parselet(WTokenType::OPEN_SQUARE_BRACKET, make_shared<ListParselet>());
	register_parselet(WTokenType::OPEN_ANGLE_BRACKET, make_shared<MapParselet>());
	register_parselet(WTokenType::OPEN_FLOOR_BRACKET, make_shared<TupleParselet>());
	register_parselet(WTokenType::OPEN_CURLY_BRACE, make_shared<SetParselet>());

	register_parselet(WTokenType::NEW, make_shared<UDTCreationParselet>());

	register_parselet(WTokenType::DOT, make_shared<UDTMemberAccessParselet>());
	register_parselet(WTokenType::COLON, make_shared<TypePatternParselet>());

	register_prefix(WTokenType::PLUS, Precedence::PREFIX);
	register_prefix(WTokenType::MINUS, Precedence::PREFIX);
	register_prefix(WTokenType::BANG, Precedence::PREFIX);

	register_infix_left(WTokenType::PLUS, Precedence::TERM);
	register_infix_left(WTokenType::MINUS, Precedence::TERM);

	register_infix_left(WTokenType::STAR, Precedence::PRODUCT);
	register_infix_left(WTokenType::DIVISION, Precedence::PRODUCT);
	register_infix_left(WTokenType::REMINDER, Precedence::PRODUCT);

	register_infix_left(WTokenType::EQUAL_EQUAL, Precedence::EQUALITY);
	register_infix_left(WTokenType::BANG_EQUAL, Precedence::EQUALITY);

	register_infix_left(WTokenType::LESSER_THAN, Precedence::COMPARISON);
	register_infix_left(WTokenType::LESSER_THAN_EQUAL, Precedence::COMPARISON);
	register_infix_left(WTokenType::GREATER_THAN, Precedence::COMPARISON);
	register_infix_left(WTokenType::GREATER_THAN_EQUAL, Precedence::COMPARISON);

	register_infix_left(WTokenType::IN_KEYWORD, Precedence::COMPARISON);
	register_infix_left(WTokenType::TYPE_OF, Precedence::COMPARISON);

	register_infix_left(WTokenType::AND, Precedence::LOGICAL);
	register_infix_left(WTokenType::OR, Precedence::LOGICAL);

	register_infix_right(WTokenType::POWER, Precedence::EXPONENT);
	register_infix_right(WTokenType::EQUAL, Precedence::ASSIGNMENT);
}

// Expression Parser

Expression_ptr Parser::parse_expression()
{
	return parse_expression(0);
}

Expression_ptr Parser::parse_expression(int precedence)
{
	token_pipe->ignore_whitespace();

	auto token = token_pipe->current();
	OPT_CHECK(token);

	IPrefixParselet_ptr prefix_parselet = prefix_parselets.at(token.value()->type);
	NULL_CHECK(prefix_parselet);

	Expression_ptr left = prefix_parselet->parse(this, token.value());
	NULL_CHECK(left);

	while (precedence < get_next_operator_precedence())
	{
		token = token_pipe->current();
		OPT_CHECK(token);

		ADVANCE_PTR;

		IInfixParselet_ptr infix_parselet = infix_parselets.at(token.value()->type);
		left = infix_parselet->parse(this, left, token.value());
	}

	return left;
}

ExpressionVector Parser::parse_expressions()
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

void Parser::register_parselet(WTokenType token_type, IPrefixParselet_ptr parselet)
{
	prefix_parselets.insert(make_pair(token_type, parselet));
}

void Parser::register_parselet(WTokenType token_type, IInfixParselet_ptr parselet)
{
	infix_parselets.insert(make_pair(token_type, parselet));
}

void Parser::register_prefix(WTokenType token_type, Precedence precedence)
{
	prefix_parselets.insert(make_pair(token_type, make_shared<PrefixOperatorParselet>((int)precedence)));
}

void Parser::register_infix_left(WTokenType token_type, Precedence precedence)
{
	infix_parselets.insert(make_pair(token_type, make_shared<BinaryOperatorParselet>((int)precedence, false)));
}

void Parser::register_infix_right(WTokenType token_type, Precedence precedence)
{
	infix_parselets.insert(make_pair(token_type, make_shared<BinaryOperatorParselet>((int)precedence, true)));
}

void Parser::register_postfix(WTokenType token_type, Precedence precedence)
{
	infix_parselets.insert(make_pair(token_type, make_shared<PostfixOperatorParselet>((int)precedence)));
}

int Parser::get_next_operator_precedence()
{
	auto token = token_pipe->current();

	if (token.has_value() && infix_parselets.contains(token.value()->type))
	{
		IInfixParselet_ptr infix_parselet = infix_parselets.at(token.value()->type);
		return infix_parselet->get_precedence();
	}

	return 0;
}

// Statement Parsers

Module_ptr Parser::execute(std::vector<Token_ptr>& tokens)
{
	token_pipe = std::make_shared<TokenPipe>(tokens);

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

Statement_ptr Parser::parse_statement(bool is_public)
{
	token_pipe->ignore_whitespace();

	auto token = token_pipe->current();

	if (!token.has_value())
		return nullptr;

	ADVANCE_PTR;

	switch (token.value()->type)
	{
		CASE(WTokenType::PUB, parse_public_statement());
		CASE(WTokenType::ENUM, parse_enum_definition(is_public));
		CASE(WTokenType::INTERFACE, parse_interface_definition(is_public));
		CASE(WTokenType::TYPE, parse_type_definition(is_public));
		CASE(WTokenType::FN, parse_function_definition(is_public));
		CASE(WTokenType::GEN, parse_generator_definition(is_public));

		CASE(WTokenType::IF, parse_branching());
		CASE(WTokenType::WHILE, parse_while_loop());
		CASE(WTokenType::FOR, parse_for_in_loop());

		CASE(WTokenType::LET, parse_variable_definition(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, parse_variable_definition(is_public, false));

		CASE(WTokenType::RETURN_KEYWORD, parse_return());
		CASE(WTokenType::YIELD_KEYWORD, parse_yield());
		CASE(WTokenType::ASSERT, parse_assert());
		CASE(WTokenType::IMPLORE, parse_implore());
		CASE(WTokenType::SWEAR, parse_swear());
		CASE(WTokenType::BREAK, parse_break());
		CASE(WTokenType::CONTINUE, parse_continue());

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
	OPT_CHECK(token);

	ADVANCE_PTR;

	const bool is_public = true;

	switch (token.value()->type)
	{
		CASE(WTokenType::INTERFACE, parse_interface_definition(is_public));
		CASE(WTokenType::ENUM, parse_enum_definition(is_public));
		CASE(WTokenType::TYPE, parse_type_definition(is_public));
		CASE(WTokenType::FN, parse_function_definition(is_public));
		CASE(WTokenType::GEN, parse_generator_definition(is_public));

		CASE(WTokenType::LET, parse_variable_definition(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, parse_variable_definition(is_public, false));
	default:
	{
		FATAL("UNEXPECTED_KEYWORD");
	}
	}
}

Statement_ptr Parser::parse_non_block_statement()
{
	auto token = token_pipe->current();
	OPT_CHECK(token);

	ADVANCE_PTR;

	switch (token.value()->type)
	{
		CASE(WTokenType::RETURN_KEYWORD, parse_return());
		CASE(WTokenType::YIELD_KEYWORD, parse_yield());
		CASE(WTokenType::BREAK, parse_break());
		CASE(WTokenType::CONTINUE, parse_continue());
		CASE(WTokenType::ASSERT, parse_assert());
		CASE(WTokenType::IMPLORE, parse_implore());
		CASE(WTokenType::SWEAR, parse_swear());

	default:
	{
		RETREAT_PTR;
		return parse_expression_statement();
	}
	}
}

Statement_ptr Parser::parse_expression_statement()
{
	auto expression = parse_expression();
	token_pipe->expect(WTokenType::EOL);

	return MAKE_STATEMENT(ExpressionStatement(move(expression)));
}

Statement_ptr Parser::parse_return()
{
	if (auto expression = parse_expression())
	{
		token_pipe->expect(WTokenType::EOL);
		return MAKE_STATEMENT(Return(move(expression)));
	}

	token_pipe->expect(WTokenType::EOL);

	return MAKE_STATEMENT(Return());
}

Statement_ptr Parser::parse_yield()
{
	if (auto expression = parse_expression())
	{
		token_pipe->expect(WTokenType::EOL);
		return MAKE_STATEMENT(YieldStatement(move(expression)));
	}

	token_pipe->expect(WTokenType::EOL);

	return MAKE_STATEMENT(YieldStatement());
}

Statement_ptr Parser::parse_assert()
{
	auto expression = parse_expression();
	NULL_CHECK(expression);

	token_pipe->expect(WTokenType::EOL);

	return MAKE_STATEMENT(AssertStatement(move(expression)));
}

Statement_ptr Parser::parse_implore()
{
	auto expression = parse_expression();
	NULL_CHECK(expression);

	token_pipe->expect(WTokenType::EOL);

	return MAKE_STATEMENT(AssertStatement(move(expression)));
}

Statement_ptr Parser::parse_swear()
{
	auto expression = parse_expression();
	NULL_CHECK(expression);

	token_pipe->expect(WTokenType::EOL);

	return MAKE_STATEMENT(AssertStatement(move(expression)));
}

Statement_ptr Parser::parse_break()
{
	token_pipe->expect(WTokenType::EOL);
	return MAKE_STATEMENT(Break());
}

Statement_ptr Parser::parse_continue()
{
	token_pipe->expect(WTokenType::EOL);
	return MAKE_STATEMENT(Continue());
}

// Blocks

Block Parser::parse_block()
{
	Block statements;

	while (true)
	{
		token_pipe->ignore_whitespace();

		auto token = token_pipe->current();
		OPT_CHECK(token);

		if (token.value()->type == WTokenType::END)
		{
			token_pipe->advance_pointer();
			break;
		}

		auto statement = parse_statement(false);

		if (!statement)
			break;

		statements.push_back(move(statement));
	}

	return statements;
}

Block Parser::parse_conditional_block()
{
	Block statements;

	while (true)
	{
		token_pipe->ignore_whitespace();

		auto token = token_pipe->current();
		OPT_CHECK(token);

		auto token_type = token.value()->type;

		switch (token_type)
		{
		case WTokenType::END:
		case WTokenType::ELIF:
		case WTokenType::ELSE:
		{
			return statements;
		}
		}

		auto statement = parse_statement(false);

		if (!statement)
			break;

		statements.push_back(move(statement));
	}

	return statements;
}

pair<Expression_ptr, Block> Parser::parse_condition_and_consequence()
{
	auto condition = parse_expression();
	token_pipe->expect(WTokenType::THEN);

	if (token_pipe->optional(WTokenType::EOL))
	{
		auto block = parse_conditional_block();
		return make_pair(condition, block);
	}

	auto statement = parse_non_block_statement();
	Block block = { move(statement) };
	return make_pair(condition, block);
}

Statement_ptr Parser::parse_branching()
{
	std::vector<std::pair<Expression_ptr, Block>> branches;

	while (true)
	{
		auto condition_consequence_pair = parse_condition_and_consequence();
		branches.push_back(condition_consequence_pair);

		if (token_pipe->optional(WTokenType::ELIF))
			continue;

		break;
	}

	Block else_block;

	if (token_pipe->optional(WTokenType::ELSE))
	{
		token_pipe->expect(WTokenType::EOL);
		else_block = parse_conditional_block();
	}

	token_pipe->expect(WTokenType::END);

	return MAKE_STATEMENT(Branching(branches, else_block));
}

Statement_ptr Parser::parse_while_loop()
{
	auto condition = parse_expression();
	NULL_CHECK(condition);

	token_pipe->expect(WTokenType::DO);

	if (token_pipe->optional(WTokenType::EOL))
	{
		auto block = parse_block();
		return MAKE_STATEMENT(WhileLoop(condition, block));
	}

	auto statement = parse_non_block_statement();
	return MAKE_STATEMENT(WhileLoop(condition, { statement }));
}

Statement_ptr Parser::parse_for_in_loop()
{
	auto [identifier, item_type] = consume_identifier_type_pair();

	token_pipe->expect(WTokenType::IN_KEYWORD);

	auto iterable_expression = parse_expression();
	NULL_CHECK(iterable_expression);

	token_pipe->expect(WTokenType::DO);

	if (token_pipe->optional(WTokenType::EOL))
	{
		auto block = parse_block();
		return MAKE_STATEMENT(ForInLoop(item_type, identifier, iterable_expression, block));
	}

	auto statement = parse_non_block_statement();
	return MAKE_STATEMENT(ForInLoop(item_type, identifier, iterable_expression, { statement }));
}

// Type Parsers

Type_ptr Parser::parse_type(bool is_optional)
{
	if (token_pipe->optional(WTokenType::OPT))
		is_optional = true;

	vector<Type_ptr> types;

	while (true)
	{
		Type_ptr type;

		if (token_pipe->optional(WTokenType::OPEN_SQUARE_BRACKET))
		{
			type = parse_list_type(is_optional);
		}
		else if (token_pipe->optional(WTokenType::OPEN_ANGLE_BRACKET))
		{
			type = parse_map_type(is_optional);
		}
		else if (token_pipe->optional(WTokenType::OPEN_CURLY_BRACE))
		{
			type = parse_set_type(is_optional);
		}
		else if (token_pipe->optional(WTokenType::OPEN_FLOOR_BRACKET))
		{
			type = parse_tuple_type(is_optional);
		}
		else
		{
			type = consume_datatype_word(is_optional);
		}

		types.push_back(type);

		if (token_pipe->optional(WTokenType::VERTICAL_BAR))
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

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		token_pipe->expect(WTokenType::CLOSE_SQUARE_BRACKET);
		break;
	}

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(ListType(move(types.front())));
	}

	return MAKE_TYPE(ListType(move(types.front())));
}

Type_ptr Parser::parse_set_type(bool is_optional)
{
	vector<Type_ptr> types;

	while (true)
	{
		auto type = parse_type();
		types.push_back(type);

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		token_pipe->expect(WTokenType::CLOSE_CURLY_BRACE);
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

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		token_pipe->expect(WTokenType::CLOSE_FLOOR_BRACKET);
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
	token_pipe->expect(WTokenType::ARROW);

	auto value_type = parse_type();
	token_pipe->expect(WTokenType::CLOSE_ANGLE_BRACKET);

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
	case WTokenType::INT:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(IntType()) : MAKE_TYPE(IntType());
	}
	case WTokenType::FLOAT:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(FloatType()) : MAKE_TYPE(FloatType());
	}
	case WTokenType::STRING_KEYWORD:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(StringType()) : MAKE_TYPE(StringType());
	}
	case WTokenType::BOOL:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(BooleanType()) : MAKE_TYPE(BooleanType());
	}
	case WTokenType::ANY:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(AnyType()) : MAKE_TYPE(AnyType());
	}
	case WTokenType::IDENTIFIER:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(UDTType(token.value()->value)) : MAKE_TYPE(UDTType(token.value()->value));
	}
	}

	FATAL("EXPECTED_DATATYPE");
}

pair<wstring, Type_ptr> Parser::consume_identifier_type_pair()
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	token_pipe->expect(WTokenType::COLON);
	auto type = parse_type();

	return make_pair(identifier->value, move(type));
}

// Definition Parsers

Statement_ptr Parser::parse_enum_definition(bool is_public)
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	token_pipe->expect(WTokenType::EOL);
	vector<wstring> members = parse_enum_members(identifier->value);
	return MAKE_STATEMENT(EnumDefinition(is_public, identifier->value, members));
}

vector<wstring> Parser::parse_enum_members(wstring stem)
{
	vector<wstring> members;

	while (true)
	{
		token_pipe->ignore_whitespace();

		if (token_pipe->optional(WTokenType::END))
			break;

		if (token_pipe->optional(WTokenType::ENUM))
		{
			auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
			token_pipe->expect(WTokenType::EOL);
			members.push_back(stem + L"::" + identifier->value);

			auto children = parse_enum_members(stem + L"::" + identifier->value);
			members.insert(end(members), begin(children), end(children));

			continue;
		}

		auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
		members.push_back(stem + L"::" + identifier->value);

		token_pipe->expect(WTokenType::EOL);
	}

	return members;
}

Statement_ptr Parser::parse_variable_definition(bool is_public, bool is_mutable)
{
	auto [identifier, type] = consume_identifier_type_pair();
	token_pipe->expect(WTokenType::EQUAL);
	auto expression = parse_expression();
	token_pipe->expect(WTokenType::EOL);

	return MAKE_STATEMENT(VariableDefinition(is_public, is_mutable, identifier, move(type), move(expression)));
}

Statement_ptr Parser::parse_interface_definition(bool is_public)
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	token_pipe->expect(WTokenType::EOL);

	std::map<std::wstring, Type_ptr> member_types;

	while (true)
	{
		token_pipe->ignore_whitespace();

		if (token_pipe->optional(WTokenType::END))
			break;

		auto [identifier, type] = consume_identifier_type_pair();
		member_types.insert_or_assign(identifier, type);
	}

	return MAKE_STATEMENT(InterfaceDefinition(is_public, identifier->value, member_types));
}

Statement_ptr Parser::parse_type_definition(bool is_public)
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);

	if (token_pipe->optional(WTokenType::EQUAL))
	{
		auto type = parse_type();
		token_pipe->expect(WTokenType::EOL);
		return MAKE_STATEMENT(AliasDefinition(is_public, identifier->value, move(type)));
	}

	token_pipe->expect(WTokenType::EOL);

	map<wstring, Type_ptr> member_types;
	StringVector public_members;

	while (true)
	{
		token_pipe->ignore_whitespace();

		if (token_pipe->optional(WTokenType::END))
		{
			token_pipe->expect(WTokenType::EOL);
			return MAKE_STATEMENT(UDTDefinition(is_public, identifier->value, member_types, public_members));
		}

		/*if (token_pipe->optional(WTokenType::FN))
		{
			return MAKE_STATEMENT(UDTDefinition(is_public, identifier->value, member_types, public_members));
		}*/

		bool is_public_member = false;

		if (token_pipe->optional(WTokenType::PUB))
			is_public_member = true;

		auto [identifier, type] = consume_identifier_type_pair();
		member_types.insert_or_assign(identifier, type);

		if (is_public_member)
		{
			public_members.push_back(identifier);
		}

		token_pipe->expect(WTokenType::EOL);
	}

	return MAKE_STATEMENT(UDTDefinition(is_public, identifier->value, member_types, public_members));
}

tuple<wstring, StringVector, TypeVector, optional<Type_ptr>, Block> Parser::parse_callable_definition()
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	token_pipe->expect(WTokenType::OPEN_PARENTHESIS);

	StringVector arguments;
	TypeVector argument_types;

	if (!token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
	{
		while (true)
		{
			auto [identifier, type] = consume_identifier_type_pair();
			arguments.push_back(identifier);
			argument_types.push_back(type);

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

	token_pipe->expect(WTokenType::EOL);

	Block block = parse_block();

	return make_tuple(identifier->value, arguments, argument_types, optional_return_type, block);
}

Statement_ptr Parser::parse_function_definition(bool is_public)
{
	auto [identifier, arguments, argument_types, optional_return_type, block] = parse_callable_definition();
	Type_ptr function_type = std::make_shared<Type>(FunctionType(argument_types, optional_return_type));

	return MAKE_STATEMENT(FunctionDefinition(is_public, identifier, arguments, function_type, block));
}

Statement_ptr Parser::parse_generator_definition(bool is_public)
{
	auto [identifier, arguments, argument_types, optional_return_type, block] = parse_callable_definition();
	Type_ptr generator_type = std::make_shared<Type>(GeneratorType(argument_types, optional_return_type));

	return MAKE_STATEMENT(GeneratorDefinition(is_public, identifier, arguments, generator_type, block));
}