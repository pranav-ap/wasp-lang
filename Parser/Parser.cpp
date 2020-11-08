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
#include <cmath>

#define ADVANCE_PTR token_pipe->advance_pointer()
#define RETREAT_PTR token_pipe->retreat_pointer()
#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define CASE(token_type, call) case token_type: { return call; }
#define MAKE_STATEMENT(x) std::make_shared<Statement>(x)
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)
#define MAKE_TYPE(x) std::make_shared<TypeNode>(x)
#define MAKE_OPTIONAL_TYPE(x) std::make_shared<TypeNode>(VariantTypeNode({ std::make_shared<TypeNode>(x), std::make_shared<TypeNode>(NoneTypeNode()) }))

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
	register_parselet(WTokenType::STRING_LITERAL, make_shared<LiteralParselet>());
	register_parselet(WTokenType::NUMBER_LITERAL, make_shared<LiteralParselet>());
	register_parselet(WTokenType::TRUE_KEYWORD, make_shared<LiteralParselet>());
	register_parselet(WTokenType::FALSE_KEYWORD, make_shared<LiteralParselet>());
	register_parselet(WTokenType::NONE, make_shared<LiteralParselet>());
	register_parselet(WTokenType::OPEN_PARENTHESIS, make_shared<CallParselet>());
	register_parselet(WTokenType::OPEN_PARENTHESIS, make_shared<GroupParselet>());
	register_parselet(WTokenType::OPEN_SQUARE_BRACKET, make_shared<ListParselet>());
	register_parselet(WTokenType::OPEN_ANGLE_BRACKET, make_shared<MapParselet>());
	register_parselet(WTokenType::OPEN_FLOOR_BRACKET, make_shared<TupleParselet>());
	register_parselet(WTokenType::OPEN_CURLY_BRACE, make_shared<SetParselet>());
	register_parselet(WTokenType::NEW, make_shared<NewParselet>());
	register_parselet(WTokenType::COLON, make_shared<TypePatternParselet>());
	register_parselet(WTokenType::IF, make_shared<TernaryConditionParselet>());
	register_parselet(WTokenType::COLON_COLON, make_shared<EnumMemberParselet>());
	register_parselet(WTokenType::DOT_DOT_DOT, make_shared<SpreadParselet>());
	register_parselet(WTokenType::DOT, make_shared<MemberAccessParselet>());
	register_parselet(WTokenType::QUESTION_DOT, make_shared<MemberAccessParselet>());

	register_prefix(WTokenType::PLUS, Precedence::PREFIX);
	register_prefix(WTokenType::MINUS, Precedence::PREFIX);
	register_prefix(WTokenType::BANG, Precedence::PREFIX);
	register_prefix(WTokenType::TYPE_OF, Precedence::PREFIX);

	register_infix_left(WTokenType::QUESTION_QUESTION, Precedence::MEMBER_ACCESS);
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
	register_infix_left(WTokenType::IS, Precedence::COMPARISON);
	register_infix_left(WTokenType::AND, Precedence::AND);
	register_infix_left(WTokenType::OR, Precedence::OR);

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

	if (token.value()->type == WTokenType::CLOSE_PARENTHESIS)
	{
		return nullptr;
	}

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
	infix_parselets.insert(make_pair(token_type, make_shared<InfixOperatorParselet>((int)precedence, false)));
}

void Parser::register_infix_right(WTokenType token_type, Precedence precedence)
{
	infix_parselets.insert(make_pair(token_type, make_shared<InfixOperatorParselet>((int)precedence, true)));
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

File_ptr Parser::execute(std::vector<Token_ptr>& tokens)
{
	token_pipe = std::make_shared<TokenPipe>(tokens);

	auto module_ast = std::make_shared<File>();

	while ((size_t)token_pipe->get_current_index() < token_pipe->get_size())
	{
		Statement_ptr node = parse_statement();

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
		CASE(WTokenType::TYPE, parse_class_definition(is_public));
		CASE(WTokenType::FN, parse_function_definition(is_public));
		CASE(WTokenType::GEN, parse_generator_definition(is_public));

		CASE(WTokenType::NAMESPACE, parse_namespace(is_public));

		CASE(WTokenType::IF, parse_branching(token.value()->type));
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
		CASE(WTokenType::TYPE, parse_class_definition(is_public));
		CASE(WTokenType::FN, parse_function_definition(is_public));
		CASE(WTokenType::GEN, parse_generator_definition(is_public));

		CASE(WTokenType::NAMESPACE, parse_namespace(is_public));

		CASE(WTokenType::LET, parse_variable_definition(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, parse_variable_definition(is_public, false));

	default:
	{
		FATAL("UNEXPECTED_KEYWORD");
	}
	}
}

Statement_ptr Parser::parse_expression_statement()
{
	auto expression = parse_expression();
	token_pipe->require(WTokenType::EOL);

	return MAKE_STATEMENT(ExpressionStatement(move(expression)));
}

Statement_ptr Parser::parse_return()
{
	if (auto expression = parse_expression())
	{
		token_pipe->require(WTokenType::EOL);
		return MAKE_STATEMENT(Return(move(expression)));
	}

	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(Return());
}

Statement_ptr Parser::parse_yield()
{
	if (auto expression = parse_expression())
	{
		token_pipe->require(WTokenType::EOL);
		return MAKE_STATEMENT(YieldStatement(move(expression)));
	}

	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(YieldStatement());
}

Statement_ptr Parser::parse_assert()
{
	auto expression = parse_expression();
	NULL_CHECK(expression);

	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(Assert(move(expression)));
}

Statement_ptr Parser::parse_implore()
{
	auto expression = parse_expression();
	NULL_CHECK(expression);

	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(Implore(move(expression)));
}

Statement_ptr Parser::parse_swear()
{
	auto expression = parse_expression();
	NULL_CHECK(expression);

	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(Swear(move(expression)));
}

Statement_ptr Parser::parse_break()
{
	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(Break());
}

Statement_ptr Parser::parse_continue()
{
	token_pipe->require(WTokenType::EOL);
	return MAKE_STATEMENT(Continue());
}

// Blocks

Statement_ptr Parser::parse_namespace(bool is_public)
{
	Block statements;

	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	NULL_CHECK(identifier);

	while (!token_pipe->optional(WTokenType::END))
	{
		Statement_ptr node = parse_statement();
		statements.push_back(node);
	}

	return MAKE_STATEMENT(Namespace(identifier->value, statements, is_public));
}

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

Statement_ptr Parser::parse_condition_and_consequence()
{
	auto condition = parse_expression();
	token_pipe->require(WTokenType::THEN);
	token_pipe->require(WTokenType::EOL);

	auto block = parse_conditional_block();
	return MAKE_STATEMENT(IfBranch(move(condition), block));
}

Expression_ptr Parser::parse_ternary_condition(Expression_ptr condition)
{
	Expression_ptr then_arm = parse_expression();

	token_pipe->require(WTokenType::ELSE);
	Expression_ptr else_arm = parse_expression((int)Precedence::TERNARY_CONDITION - 1);

	return MAKE_EXPRESSION(TernaryCondition(move(condition), move(then_arm), move(else_arm)));
}

Statement_ptr Parser::parse_branching(WTokenType token_type)
{
	auto condition = parse_expression();
	token_pipe->require(WTokenType::THEN);

	if (token_type == WTokenType::IF && !token_pipe->optional(WTokenType::EOL))
	{
		Expression_ptr ternary = parse_ternary_condition(condition);
		token_pipe->require(WTokenType::EOL);

		return MAKE_STATEMENT(ExpressionStatement(move(ternary)));
	}

	Block block = parse_conditional_block();
	auto if_branch = IfBranch(condition, block);

	if (token_pipe->optional(WTokenType::ELIF))
	{
		auto alternative = parse_branching(WTokenType::ELIF);
		if_branch.alternative = alternative;
		return MAKE_STATEMENT(if_branch);
	}
	else if (token_pipe->optional(WTokenType::ELSE))
	{
		token_pipe->require(WTokenType::EOL);
		Block else_block = parse_conditional_block();
		if_branch.alternative = MAKE_STATEMENT(ElseBranch(else_block));
	}

	token_pipe->require(WTokenType::END);

	return MAKE_STATEMENT(if_branch);
}

Statement_ptr Parser::parse_while_loop()
{
	auto condition = parse_expression();
	NULL_CHECK(condition);

	token_pipe->require(WTokenType::DO);

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
	auto expression = parse_expression();
	NULL_CHECK(expression);

	ASSERT(holds_alternative<Infix>(*expression), "Infix must be an IN expression");
	auto infix_expression = get_if<Infix>(&*expression);
	ASSERT(infix_expression->op->type == WTokenType::IN_KEYWORD, "Infix must be IN");

	auto lhs_pattern = infix_expression->left;
	auto rhs_pattern = infix_expression->right;

	token_pipe->require(WTokenType::DO);

	if (token_pipe->optional(WTokenType::EOL))
	{
		auto block = parse_block();
		return MAKE_STATEMENT(ForInLoop(lhs_pattern, rhs_pattern, block));
	}

	auto statement = parse_non_block_statement();
	return MAKE_STATEMENT(ForInLoop(lhs_pattern, rhs_pattern, { statement }));
}

// TypeNode Parsers

TypeNode_ptr Parser::parse_type(bool is_optional)
{
	if (token_pipe->optional(WTokenType::OPT))
		is_optional = true;

	vector<TypeNode_ptr> types;

	while (true)
	{
		TypeNode_ptr type;

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
		else if (token_pipe->optional(WTokenType::OPEN_PARENTHESIS))
		{
			type = parse_function_type(is_optional);
		}
		else if (token_pipe->optional(WTokenType::FN))
		{
			token_pipe->require(WTokenType::OPEN_PARENTHESIS);
			type = parse_function_type(is_optional);
		}
		else if (token_pipe->optional(WTokenType::GEN))
		{
			token_pipe->require(WTokenType::OPEN_PARENTHESIS);
			type = parse_generator_type(is_optional);
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
			return MAKE_TYPE(VariantTypeNode(types));

		return move(types.front());
	}
}

TypeNode_ptr Parser::parse_list_type(bool is_optional)
{
	vector<TypeNode_ptr> types;

	while (true)
	{
		auto type = parse_type();
		types.push_back(type);

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		token_pipe->require(WTokenType::CLOSE_SQUARE_BRACKET);
		break;
	}

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(ListTypeNode(move(types.front())));
	}

	return MAKE_TYPE(ListTypeNode(move(types.front())));
}

TypeNode_ptr Parser::parse_set_type(bool is_optional)
{
	vector<TypeNode_ptr> types;

	while (true)
	{
		auto type = parse_type();
		types.push_back(type);

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		token_pipe->require(WTokenType::CLOSE_CURLY_BRACE);
		break;
	}

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(ListTypeNode(move(types.front())));
	}

	return MAKE_TYPE(ListTypeNode(move(types.front())));
}

TypeNode_ptr Parser::parse_tuple_type(bool is_optional)
{
	vector<TypeNode_ptr> types;

	while (true)
	{
		auto type = parse_type();
		types.push_back(type);

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		token_pipe->require(WTokenType::CLOSE_FLOOR_BRACKET);
		break;
	}

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(TupleTypeNode(types));
	}

	return MAKE_TYPE(TupleTypeNode(types));
}

TypeNode_ptr Parser::parse_map_type(bool is_optional)
{
	auto key_type = parse_type();
	token_pipe->require(WTokenType::ARROW);

	auto value_type = parse_type();
	token_pipe->require(WTokenType::CLOSE_ANGLE_BRACKET);

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(MapTypeNode(move(key_type), move(value_type)));
	}

	return MAKE_TYPE(MapTypeNode(move(key_type), move(value_type)));
}

tuple<TypeNodeVector, optional<TypeNode_ptr>> Parser::parse_callable_type()
{
	TypeNodeVector input_types;

	if (!token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
	{
		while (true)
		{
			auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
			NULL_CHECK(identifier);

			token_pipe->require(WTokenType::COLON);

			auto type = parse_type();
			NULL_CHECK(type);

			input_types.push_back(type);

			if (token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
				break;

			token_pipe->require(WTokenType::COMMA);
		}
	}

	std::optional<TypeNode_ptr> return_type = std::nullopt;

	if (token_pipe->optional(WTokenType::ARROW))
	{
		return_type = parse_type();
		NULL_CHECK(return_type);
	}

	return make_tuple(input_types, return_type);
}

TypeNode_ptr Parser::parse_function_type(bool is_optional)
{
	auto [input_types, return_type] = parse_callable_type();

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(FunctionTypeNode(input_types, return_type));
	}

	return MAKE_TYPE(FunctionTypeNode(input_types, return_type));
}

TypeNode_ptr Parser::parse_generator_type(bool is_optional)
{
	auto [input_types, return_type] = parse_callable_type();

	if (is_optional)
	{
		return MAKE_OPTIONAL_TYPE(GeneratorTypeNode(input_types, return_type));
	}

	return MAKE_TYPE(GeneratorTypeNode(input_types, return_type));
}

TypeNode_ptr Parser::consume_datatype_word(bool is_optional)
{
	auto token = token_pipe->current();
	OPT_CHECK(token);

	switch (token.value()->type)
	{
	case WTokenType::NUMBER_LITERAL:
	{
		ADVANCE_PTR;

		auto value = std::stod(token.value()->value);

		if (std::fmod(value, 1.0) == 0.0)
		{
			return is_optional
				? MAKE_OPTIONAL_TYPE(IntLiteralTypeNode((int)value))
				: MAKE_TYPE(IntLiteralTypeNode((int)value));
		}

		return is_optional
			? MAKE_OPTIONAL_TYPE(FloatLiteralTypeNode(value))
			: MAKE_TYPE(FloatLiteralTypeNode(value));
	}
	case WTokenType::STRING_LITERAL:
	{
		ADVANCE_PTR;
		return is_optional
			? MAKE_OPTIONAL_TYPE(StringLiteralTypeNode(token.value()->value))
			: MAKE_TYPE(StringLiteralTypeNode(token.value()->value));
	}
	case WTokenType::TRUE_KEYWORD:
	{
		ADVANCE_PTR;
		return is_optional
			? MAKE_OPTIONAL_TYPE(BooleanLiteralTypeNode(true))
			: MAKE_TYPE(BooleanLiteralTypeNode(true));
	}
	case WTokenType::FALSE_KEYWORD:
	{
		ADVANCE_PTR;
		return is_optional
			? MAKE_OPTIONAL_TYPE(BooleanLiteralTypeNode(false))
			: MAKE_TYPE(BooleanLiteralTypeNode(false));
	}
	case WTokenType::INT:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(IntTypeNode()) : MAKE_TYPE(IntTypeNode());
	}
	case WTokenType::FLOAT:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(FloatTypeNode()) : MAKE_TYPE(FloatTypeNode());
	}
	case WTokenType::STRING_KEYWORD:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(StringTypeNode()) : MAKE_TYPE(StringTypeNode());
	}
	case WTokenType::BOOL:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(BooleanTypeNode()) : MAKE_TYPE(BooleanTypeNode());
	}
	case WTokenType::ANY:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(AnyTypeNode()) : MAKE_TYPE(AnyTypeNode());
	}
	case WTokenType::IDENTIFIER:
	{
		ADVANCE_PTR;
		return is_optional ? MAKE_OPTIONAL_TYPE(TypeIdentifierNode(token.value()->value)) : MAKE_TYPE(TypeIdentifierNode(token.value()->value));
	}
	}

	FATAL("EXPECTED_DATATYPE");
}

pair<wstring, TypeNode_ptr> Parser::consume_identifier_type_pair()
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	token_pipe->require(WTokenType::COLON);
	auto type = parse_type();

	return make_pair(identifier->value, move(type));
}

// Definition Parsers

Statement_ptr Parser::parse_enum_definition(bool is_public)
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
	token_pipe->require(WTokenType::EOL);
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
			token_pipe->require(WTokenType::EOL);
			members.push_back(stem + L"::" + identifier->value);

			auto children = parse_enum_members(stem + L"::" + identifier->value);
			members.insert(end(members), begin(children), end(children));

			continue;
		}

		auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
		members.push_back(stem + L"::" + identifier->value);

		token_pipe->require(WTokenType::EOL);
	}

	return members;
}

Statement_ptr Parser::parse_variable_definition(bool is_public, bool is_mutable)
{
	auto expression = parse_expression();
	token_pipe->require(WTokenType::EOL);

	ASSERT(holds_alternative<Assignment>(*expression), "Must be an Assignment");
	auto assignment = get_if<Assignment>(&*expression);

	ASSERT(holds_alternative<TypePattern>(*assignment->lhs_expression), "Must be a TypePattern");
	auto type_pattern = get_if<TypePattern>(&*assignment->lhs_expression);

	return MAKE_STATEMENT(VariableDefinition(is_public, is_mutable, move(type_pattern->type), move(type_pattern->expression), move(assignment->rhs_expression)));
}

StringVector Parser::parse_comma_separated_identifiers()
{
	StringVector identifiers;

	while (true)
	{
		token_pipe->ignore_whitespace();

		auto identifier = token_pipe->require(WTokenType::IDENTIFIER);
		NULL_CHECK(identifier);

		identifiers.push_back(identifier->value);

		if (token_pipe->optional(WTokenType::COMMA))
			continue;

		break;
	}

	return identifiers;
}

tuple<map<wstring, TypeNode_ptr>, std::map<std::wstring, bool>, StringVector, StringVector> Parser::parse_class_and_interface_definition()
{
	map<wstring, TypeNode_ptr> member_types;
	std::map<std::wstring, bool> is_public_member;

	StringVector interfaces;

	if (token_pipe->optional(WTokenType::TILDE))
	{
		interfaces = parse_comma_separated_identifiers();
	}

	StringVector base_types;

	if (token_pipe->optional(WTokenType::LESSER_THAN))
	{
		base_types = parse_comma_separated_identifiers();
	}

	while (true)
	{
		token_pipe->ignore_whitespace();

		if (token_pipe->optional(WTokenType::END))
			break;

		bool is_public = false;

		if (token_pipe->optional(WTokenType::PUB))
			is_public = true;

		auto [identifier, type] = consume_identifier_type_pair();
		token_pipe->require(WTokenType::EOL);

		const auto [_, success] = member_types.insert({ identifier, type });
		ASSERT(success, "Duplicate members are found!");

		const auto [_x, ok] = is_public_member.insert({ identifier, is_public });
		ASSERT(ok, "Duplicate members are found!");
	}

	return make_tuple(member_types, is_public_member, interfaces, base_types);
}

Statement_ptr Parser::parse_interface_definition(bool is_public)
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);

	auto [member_types, public_members, interfaces, base_types] = parse_class_and_interface_definition();
	return MAKE_STATEMENT(InterfaceDefinition(is_public, identifier->value, member_types, public_members, interfaces, base_types));
}

Statement_ptr Parser::parse_class_definition(bool is_public)
{
	auto identifier = token_pipe->require(WTokenType::IDENTIFIER);

	if (token_pipe->optional(WTokenType::EQUAL))
	{
		auto type = parse_type();
		token_pipe->require(WTokenType::EOL);
		return MAKE_STATEMENT(AliasDefinition(is_public, identifier->value, move(type)));
	}

	auto [member_types, public_members, interfaces, base_types] = parse_class_and_interface_definition();
	return MAKE_STATEMENT(ClassDefinition(is_public, identifier->value, member_types, public_members, interfaces, base_types));
}

tuple<wstring, wstring, bool, StringVector, TypeNodeVector, optional<TypeNode_ptr>, Block> Parser::parse_callable_definition()
{
	auto first_identifier = token_pipe->require(WTokenType::IDENTIFIER);
	NULL_CHECK(first_identifier);

	bool is_method = false;
	Token_ptr second_identifier;

	if (token_pipe->optional(WTokenType::COLON_COLON))
	{
		is_method = true;
		second_identifier = token_pipe->require(WTokenType::IDENTIFIER);
	}

	token_pipe->require(WTokenType::OPEN_PARENTHESIS);

	StringVector arguments;
	TypeNodeVector argument_types;

	if (!token_pipe->optional(WTokenType::CLOSE_PARENTHESIS))
	{
		while (true)
		{
			auto [identifier, type] = consume_identifier_type_pair();
			arguments.push_back(identifier);
			argument_types.push_back(type);

			if (token_pipe->optional(WTokenType::COMMA))
				continue;

			token_pipe->require(WTokenType::CLOSE_PARENTHESIS);
			break;
		}
	}

	optional<TypeNode_ptr> optional_return_type = std::nullopt;

	if (token_pipe->optional(WTokenType::ARROW))
	{
		auto return_type = parse_type();
		optional_return_type = std::make_optional(return_type);
	}

	token_pipe->require(WTokenType::EOL);

	Block block = parse_block();

	if (is_method)
	{
		return make_tuple(first_identifier->value, second_identifier->value, is_method, arguments, argument_types, optional_return_type, block);
	}

	return make_tuple(first_identifier->value, L"", is_method, arguments, argument_types, optional_return_type, block);
}

Statement_ptr Parser::parse_function_definition(bool is_public)
{
	auto [first_identifier, second_identifier, is_method, arguments, argument_types, optional_return_type, block] = parse_callable_definition();

	if (is_method)
	{
		TypeNode_ptr function_method_type = MAKE_TYPE(FunctionMemberTypeNode(first_identifier, argument_types, optional_return_type));
		//second_identifier += stringify_type(function_type);
		return MAKE_STATEMENT(FunctionMemberDefinition(first_identifier, second_identifier, is_public, arguments, function_method_type, block));
	}

	TypeNode_ptr function_type = MAKE_TYPE(FunctionTypeNode(argument_types, optional_return_type));
	//first_identifier += stringify_type(function_type);
	return MAKE_STATEMENT(FunctionDefinition(is_public, first_identifier, arguments, function_type, block));
}

Statement_ptr Parser::parse_generator_definition(bool is_public)
{
	auto [first_identifier, second_identifier, is_method, arguments, argument_types, optional_return_type, block] = parse_callable_definition();

	if (is_method)
	{
		TypeNode_ptr generator_method_type = MAKE_TYPE(GeneratorMemberTypeNode(first_identifier, argument_types, optional_return_type));
		//second_identifier += stringify_type(function_type);
		return MAKE_STATEMENT(GeneratorMemberDefinition(first_identifier, second_identifier, is_public, arguments, generator_method_type, block));
	}

	TypeNode_ptr function_type = std::make_shared<TypeNode>(GeneratorTypeNode(argument_types, optional_return_type));
	//first_identifier += stringify_type(function_type);
	return MAKE_STATEMENT(GeneratorDefinition(is_public, first_identifier, arguments, function_type, block));
}
