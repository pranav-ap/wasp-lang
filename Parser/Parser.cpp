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
	register_parselet(WTokenType::OPEN_PARENTHESIS, make_shared<GroupParselet>());
	register_parselet(WTokenType::OPEN_SQUARE_BRACKET, make_shared<ListParselet>());
	register_parselet(WTokenType::OPEN_ANGLE_BRACKET, make_shared<MapParselet>());
	register_parselet(WTokenType::OPEN_FLOOR_BRACKET, make_shared<TupleParselet>());
	register_parselet(WTokenType::OPEN_CURLY_BRACE, make_shared<SetParselet>());
	register_parselet(WTokenType::COLON, make_shared<TypePatternParselet>());
	register_parselet(WTokenType::IF, make_shared<TernaryConditionParselet>());
	register_parselet(WTokenType::DOT_DOT_DOT, make_shared<SpreadParselet>());
	register_prefix(WTokenType::PLUS, Precedence::PREFIX);
	register_prefix(WTokenType::MINUS, Precedence::PREFIX);
	register_prefix(WTokenType::BANG, Precedence::PREFIX);
	register_prefix(WTokenType::TYPE_OF, Precedence::PREFIX);
	register_prefix(WTokenType::AT_SIGN, Precedence::PREFIX);
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
	register_infix_left(WTokenType::QUESTION_QUESTION, Precedence::COALESE);
	register_infix_left(WTokenType::AND, Precedence::AND);
	register_infix_left(WTokenType::OR, Precedence::OR);
	register_infix_right(WTokenType::POWER, Precedence::EXPONENT);
	register_infix_right(WTokenType::EQUAL, Precedence::ASSIGNMENT);
}

Module_ptr Parser::run(std::vector<Token_ptr>& tokens)
{
	token_pipe = std::make_shared<TokenPipe>(tokens);

	auto module_ast = std::make_shared<Module>();

	while ((size_t)token_pipe->get_current_index() < token_pipe->get_size())
	{
		Statement_ptr node = parse_statement();

		if (node)
		{
			module_ast->add_statement(move(node));
		}
	}

	return move(module_ast);
}
