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

#define ADVANCE_PTR token_pipe->advance_pointer()
#define RETREAT_PTR token_pipe->retreat_pointer()
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
	size_t length = token_pipe->get_size();

	// error on indent

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
	auto token = token_pipe->significant();

	if (token == nullptr)
		return nullptr;

	/*
	count indent
	indent == old

	*/

	ADVANCE_PTR;

	switch (token->type)
	{
		CASE(WTokenType::IMPORT, parse_import());
		CASE(WTokenType::LET, parse_variable_declaration(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, parse_variable_declaration(is_public, false));
	default:
	{
	}
	}
}

Statement_ptr Parser::parse_import()
{
	ASSERT(token_pipe->assert(WTokenType::OPEN_CURLY_BRACE), "Expected a OPEN_CURLY_BRACE");

	string_vector goods;

	while (true)
	{
		auto identifier = token_pipe->required(WTokenType::Identifier);
		goods.push_back(identifier->value);

		if (token_pipe->assert(WTokenType::CLOSE_CURLY_BRACE))
			break;

		ASSERT(token_pipe->assert(WTokenType::COMMA), "Expected a COMMA or a CLOSE_CURLY_BRACE");
	}

	ASSERT(token_pipe->assert(WTokenType::FROM), "Expected the FROM keyword");

	auto current = token_pipe->optional(WTokenType::Identifier);

	if (current)
	{
		return MAKE_STATEMENT(ImportInBuilt(current->value, goods));
	}

	auto path_token = token_pipe->required(WTokenType::StringLiteral);
	return MAKE_STATEMENT(ImportCustom(path_token->value, goods));
}

Statement_ptr Parser::parse_variable_declaration(bool is_public, bool is_mutable)
{
	auto identifier = token_pipe->required(WTokenType::Identifier);
	ASSERT(token_pipe->assert(WTokenType::COLON), "Expected a COLON");

	auto type = parse_type();
	ASSERT(token_pipe->assert(WTokenType::EQUAL), "Expected a EQUAL");

	Expression_ptr expression = expr_parser->parse_expression();

	return MAKE_STATEMENT(VariableDefinition(is_public, is_mutable, identifier->value, move(type), move(expression)));
}

Block Parser::parse_block()
{
	ASSERT(token_pipe->assert(WTokenType::OPEN_CURLY_BRACE), "Expected a OPEN_CURLY_BRACE");

	Block statements;

	while (true)
	{
		if (token_pipe->assert(WTokenType::CLOSE_CURLY_BRACE))
			return statements;

		auto statement = parse_statement(false);
		statements.push_back(move(statement));
	}
}