#pragma once
#include "pch.h"
#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <utility>

#include "Parser.h"

using std::string;
using std::vector;
using std::stack;
using std::shared_ptr;
using std::pair;
using std::make_shared;
using std::make_pair;
using std::move;
using std::map;

// API

Module Parser::execute()
{
	Module mod;
	size_t length = this->tokens.size();

	while (true)
	{
		if ((size_t)this->pointer.get_index() >= length)
			break;

		Statement_ptr node = this->parse_statement(false);

		if (node != nullptr)
			mod.add(move(node));
	}

	return mod;
}

// Parsers

Statement_ptr Parser::parse_statement(bool is_public)
{
	this->ignore(WTokenType::EOL);

	auto token = this->get_current_token();
	ADVANCE_PTR;

	if (token == nullptr)
		return nullptr;

	switch (token->get_type())
	{
		CASE(WTokenType::LET, this->parse_variable_declaration(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, this->parse_variable_declaration(is_public, false));
		CASE(WTokenType::Identifier, this->handle_identifier(move(token)));
		CASE(WTokenType::BREAK, this->parse_break_statement());
		CASE(WTokenType::CONTINUE, this->parse_continue_statement());
		CASE(WTokenType::RETURN, this->parse_return_statement());
		CASE(WTokenType::PUB, this->parse_public_statement());
		CASE(WTokenType::IF, this->parse_branching_statement());
		CASE(WTokenType::LOOP, this->parse_loop_statement());
		CASE(WTokenType::TYPE, this->parse_type_declaration(is_public));
		CASE(WTokenType::FN, this->parse_function_definition(is_public));
		//CASE(WTokenType::ENUM, this->parse_enum_definition(is_public));
		//CASE(WTokenType::IMPORT, this->parse_import_statement());
	default: {
		this->pointer.retreat();
		return this->parse_expression_statement();
	}
	}
}

Expression_ptr Parser::parse_expression()
{
	vector<Expression_ptr> ast_forest;
	stack<Token_ptr> operator_stack;

	while (true)
	{
		Token_ptr token = this->get_current_token();

		if (token == nullptr)
			return nullptr;

		auto type = token->get_type();

		if (type == WTokenType::EOL || type == WTokenType::COMMA)
			break;

		if (type == WTokenType::NumberLiteral)
		{
			ast_forest.push_back(make_shared<NumberLiteral>(stod(token->get_value())));
			ADVANCE_PTR;
		}
		else if (type == WTokenType::StringLiteral)
		{
			ast_forest.push_back(make_shared<StringLiteral>(token->get_value()));
			ADVANCE_PTR;
		}
		else if (type == WTokenType::TRUE_KEYWORD || type == WTokenType::FALSE_KEYWORD)
		{
			bool bool_value = token->get_value() == "true" ? true : false;
			ast_forest.push_back(make_shared<BooleanLiteral>(bool_value));
			ADVANCE_PTR;
		}
		else if (type == WTokenType::Identifier)
		{
			ADVANCE_PTR;

			if (this->expect_current_token(WTokenType::OPEN_BRACKET))
			{
				auto expression = this->parse_expression();
				RETURN_NULLPTR_IF_NULLPTR(expression);
				RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::CLOSE_BRACKET));

				ast_forest.push_back(make_shared<MemberAccess>(token->get_value(), move(expression)));
			}
			else if (this->expect_current_token(WTokenType::DOT))
			{
				auto identifier = this->consume_token(WTokenType::Identifier);
				RETURN_NULLPTR_IF_NULLPTR(identifier);

				ast_forest.push_back(make_shared<RecordMemberAccess>(token->get_value(), identifier->get_value()));
			}
			else
			{
				ast_forest.push_back(make_shared<Identifier>(token->get_value()));
			}
		}
		else if (type == WTokenType::OPEN_BRACKET)
		{
			ADVANCE_PTR;
			auto vec = this->parse_vector_literal();
			RETURN_NULLPTR_IF_NULLPTR(vec);

			ast_forest.push_back(move(vec));
		}
		else if (type == WTokenType::OPEN_PARENTHESIS)
		{
			ADVANCE_PTR;
			auto tuple = this->parse_tuple_literal();
			RETURN_NULLPTR_IF_NULLPTR(tuple);

			ast_forest.push_back(move(tuple));
		}
		else if (type == WTokenType::AND || type == WTokenType::OR)
		{
			this->push_operator_to_operator_stack(move(token), operator_stack, ast_forest);
			ADVANCE_PTR;
		}
		else if (type == WTokenType::OPEN_PARENTHESIS)
		{
			this->inside_function_call.push(false);
			operator_stack.push(move(token));
			ADVANCE_PTR;
		}
		else if (type == WTokenType::CLOSE_PARENTHESIS)
		{
			if (this->inside_function_call.top() == true)
				break;

			this->pop_until_open_parenthesis_from_stack_into_ast(operator_stack, ast_forest);
			ADVANCE_PTR;
			break;
		}
		else if (type == WTokenType::OPEN_CURLY_BRACE || type == WTokenType::CLOSE_CURLY_BRACE || type == WTokenType::CLOSE_BRACKET)
		{
			break;
		}
		else if (type == WTokenType::BANG ||
			type == WTokenType::UNARY_MINUS ||
			type == WTokenType::UNARY_PLUS ||
			type == WTokenType::PLUS ||
			type == WTokenType::MINUS ||
			type == WTokenType::DIVISION ||
			type == WTokenType::REMINDER ||
			type == WTokenType::STAR ||
			type == WTokenType::POWER ||
			type == WTokenType::GREATER_THAN ||
			type == WTokenType::GREATER_THAN_EQUAL ||
			type == WTokenType::LESSER_THAN ||
			type == WTokenType::LESSER_THAN_EQUAL ||
			type == WTokenType::EQUAL_EQUAL ||
			type == WTokenType::BANG_EQUAL)
		{
			this->push_operator_to_operator_stack(move(token), operator_stack, ast_forest);
			ADVANCE_PTR;
		}
		else if (type == WTokenType::FunctionIdentifier)
		{
			ADVANCE_PTR;
			vector<Expression_ptr> expressions;

			RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::OPEN_PARENTHESIS));

			this->inside_function_call.push(true);

			if (this->expect_current_token(WTokenType::CLOSE_PARENTHESIS))
			{
				this->inside_function_call.pop();
				ast_forest.push_back(make_shared<FunctionCall>(token->get_value(), expressions));
			}
			else
			{
				while (true)
				{
					auto expression = this->parse_expression();
					RETURN_NULLPTR_IF_NULLPTR(expression);

					expressions.push_back(move(expression));

					if (this->expect_current_token(WTokenType::COMMA))
						continue;

					if (this->expect_current_token(WTokenType::CLOSE_PARENTHESIS))
					{
						ast_forest.push_back(make_shared<FunctionCall>(token->get_value(), expressions));
						break;
					}
				}
			}
		}
		else
		{
			// TODO : NECESSARY?
			ADVANCE_PTR;
		}
	}

	this->pop_all_from_stack_into_ast(operator_stack, ast_forest);

	if (ast_forest.size() > 1)
		return nullptr;

	return move(ast_forest[0]);
}

// Variable declaration parsers

Statement_ptr Parser::parse_variable_declaration(bool is_public, bool is_mutable)
{
	auto identifier = this->consume_token(WTokenType::Identifier);
	RETURN_NULLPTR_IF_NULLPTR(identifier);
	RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::COLON));

	auto type = this->parse_type();
	RETURN_NULLPTR_IF_NULLPTR(type);
	RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::EQUAL));

	Expression_ptr expression = nullptr;

	if (this->expect_current_token(WTokenType::OPEN_CURLY_BRACE))
	{
		expression = this->parse_map_or_record_literal();
		RETURN_NULLPTR_IF_NULLPTR(expression);
	}

	if (expression == nullptr)
		expression = this->parse_expression();

	RETURN_NULLPTR_IF_NULLPTR(expression);
	RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::EOL));

	return make_shared<VariableDeclaration>(is_public, is_mutable, identifier->get_value(), move(type), move(expression));
}

// Expression Statement

Statement_ptr Parser::parse_expression_statement()
{
	// ensure that pointer is at first token of expr ??
	auto expression = this->parse_expression();
	RETURN_NULLPTR_IF_NULLPTR(expression);
	RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::EOL));

	return make_shared<ExpressionStatement>(move(expression));
}

// Literal parsers

Expression_ptr Parser::parse_vector_literal()
{
	vector<Expression_ptr> elements;

	if (this->expect_current_token(WTokenType::CLOSE_BRACKET))
		return make_shared<VectorLiteral>(elements);

	while (true)
	{
		auto element = this->parse_expression();
		RETURN_NULLPTR_IF_NULLPTR(element);

		elements.push_back(move(element));

		if (this->expect_current_token(WTokenType::CLOSE_BRACKET))
			return make_shared<VectorLiteral>(elements);

		RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::COMMA));
	}
}

Expression_ptr Parser::parse_tuple_literal()
{
	vector<Expression_ptr> elements;

	if (this->expect_current_token(WTokenType::CLOSE_PARENTHESIS))
		return make_shared<TupleLiteral>(elements);

	while (true)
	{
		auto element = this->parse_expression();
		RETURN_NULLPTR_IF_NULLPTR(element);

		elements.push_back(move(element));

		if (this->expect_current_token(WTokenType::CLOSE_PARENTHESIS))
			return make_shared<TupleLiteral>(elements);

		RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::COMMA));
	}
}

Expression_ptr Parser::parse_map_literal()
{
	map<Expression_ptr, Expression_ptr> pairs;

	if (this->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
		return make_shared<MapLiteral>(pairs);

	while (true)
	{
		this->ignore(WTokenType::EOL);

		auto key = this->consume_valid_map_key();
		RETURN_NULLPTR_IF_NULLPTR(key);

		RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::COLON));

		auto value = this->parse_expression();
		RETURN_NULLPTR_IF_NULLPTR(value);

		pairs.insert_or_assign(key, value);

		this->ignore(WTokenType::EOL);

		if (this->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
			return make_shared<MapLiteral>(pairs);

		RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::COMMA));
	}
}

Expression_ptr Parser::parse_record_literal()
{
	map<string, Expression_ptr> pairs;

	if (this->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
		return make_shared<RecordLiteral>(pairs);

	while (true)
	{
		this->ignore(WTokenType::EOL);

		auto key = this->consume_valid_record_key();
		RETURN_NULLPTR_IF_NULLPTR(key);

		RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::COLON));

		auto value = this->parse_expression();
		RETURN_NULLPTR_IF_NULLPTR(value);

		pairs.insert_or_assign(*key.get(), value);

		this->ignore(WTokenType::EOL);

		if (this->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
			return make_shared<RecordLiteral>(pairs);

		RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::COMMA));
	}
}

Expression_ptr Parser::parse_map_or_record_literal()
{
	this->ignore(WTokenType::EOL);

	auto token = this->get_current_token();

	if (token->get_type() == WTokenType::Identifier)
		return this->parse_record_literal();

	return this->parse_map_literal();
}

Expression_ptr Parser::consume_valid_map_key()
{
	auto token = this->get_current_token();
	RETURN_NULLPTR_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case WTokenType::NumberLiteral:
	{
		ADVANCE_PTR;
		return make_shared<NumberLiteral>(stod(token->get_value()));
	}
	case WTokenType::StringLiteral:
	{
		ADVANCE_PTR;
		return make_shared<StringLiteral>(token->get_value());
	}
	default:
	{
		return nullptr;
	}
	}
}

shared_ptr<string> Parser::consume_valid_record_key()
{
	auto token = this->get_current_token();
	RETURN_NULLPTR_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case WTokenType::Identifier:
	{
		ADVANCE_PTR;
		return make_shared<string>(token->get_value());
	}
	default:
	{
		return nullptr;
	}
	}
}

// Assignment or expression statement

Statement_ptr Parser::handle_identifier(Token_ptr identifier)
{
	if (this->expect_current_token(WTokenType::EQUAL))
	{
		auto expression = this->parse_expression();
		RETURN_NULLPTR_IF_NULLPTR(expression);

		RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::EOL));

		return make_shared<Assignment>(identifier->get_value(), move(expression));
	}

	this->pointer.retreat();
	return this->parse_expression_statement();
}

Statement_ptr Parser::parse_type_declaration(bool is_public)
{
	auto name = this->consume_token(WTokenType::Identifier);
	RETURN_NULLPTR_IF_NULLPTR(name);

	if (this->expect_current_token(WTokenType::OPEN_CURLY_BRACE))
	{
		map<string, Type_ptr> member_types;

		while (true)
		{
			this->ignore(WTokenType::EOL);

			auto identifier = this->consume_token(WTokenType::Identifier);
			RETURN_NULLPTR_IF_NULLPTR(identifier);

			RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::COLON));

			auto type = this->parse_type();
			RETURN_NULLPTR_IF_NULLPTR(type);

			member_types.insert_or_assign(identifier->get_value(), type);

			this->ignore(WTokenType::EOL);

			if (this->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
				return make_shared<RecordDefinition>(is_public, name->get_value(), member_types);

			RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::COMMA));
		}
	}

	auto type = this->parse_type();
	RETURN_NULLPTR_IF_NULLPTR(type);

	return make_shared<Alias>(name->get_value(), move(type));
}

// Public Statement

Statement_ptr Parser::parse_public_statement()
{
	auto token = this->get_current_token();
	ADVANCE_PTR;

	if (token == nullptr)
		return nullptr;

	const bool is_public = true;

	switch (token->get_type())
	{
		CASE(WTokenType::LET, this->parse_variable_declaration(is_public, true));
		CASE(WTokenType::CONST_KEYWORD, this->parse_variable_declaration(is_public, false));
		CASE(WTokenType::TYPE, this->parse_type_declaration(is_public));
		CASE(WTokenType::FN, this->parse_function_definition(is_public));
		//CASE(WTokenType::ENUM, this->parse_enum_definition(is_public));
	default: {
		return nullptr;
	}
	}
}

// Block Statements Parsers

Block_ptr Parser::parse_block()
{
	Block_ptr statements = make_shared<Block>();

	this->ignore(WTokenType::EOL);
	RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::OPEN_CURLY_BRACE));

	while (true)
	{
		if (this->expect_current_token(WTokenType::CLOSE_CURLY_BRACE))
			return statements;

		this->ignore(WTokenType::EOL);

		auto statement = this->parse_statement(false);
		RETURN_NULLPTR_IF_NULLPTR(statement);

		statements->push_back(move(statement));
	}
}

Statement_ptr Parser::parse_loop_statement()
{
	auto block = this->parse_block();
	RETURN_NULLPTR_IF_NULLPTR(block);

	return make_shared<Loop>(block);
}

Statement_ptr Parser::parse_return_statement()
{
	auto expression = this->parse_expression();
	RETURN_NULLPTR_IF_NULLPTR(expression);
	RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::EOL));

	return make_shared<Return>(std::make_optional(move(expression)));
}

Statement_ptr Parser::parse_branching_statement()
{
	auto condition = this->parse_expression();
	RETURN_NULLPTR_IF_NULLPTR(condition);

	auto consequence = this->parse_block();
	RETURN_NULLPTR_IF_NULLPTR(consequence);

	if (this->expect_current_token(WTokenType::ELSE))
	{
		if (this->expect_current_token(WTokenType::IF))
		{
			auto alternative_stat = this->parse_branching_statement();
			RETURN_NULLPTR_IF_NULLPTR(alternative_stat);

			auto alternative = make_shared<vector<Statement_ptr>>();
			alternative->push_back(move(alternative_stat));

			return make_shared<Branch>(move(condition), consequence, alternative);
		}

		auto alternative = this->parse_block();
		RETURN_NULLPTR_IF_NULLPTR(alternative);

		return make_shared<Branch>(move(condition), consequence, alternative);
	}

	auto alternative = make_shared<vector<Statement_ptr>>();
	return make_shared<Branch>(move(condition), consequence, alternative);
}

Statement_ptr Parser::parse_break_statement()
{
	RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::EOL));
	return make_shared<Break>();
}

Statement_ptr Parser::parse_continue_statement()
{
	RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::EOL));
	return make_shared<Continue>();
}

Statement_ptr Parser::parse_function_definition(bool is_public)
{
	auto identifier = this->consume_token(WTokenType::FunctionIdentifier);
	RETURN_NULLPTR_IF_NULLPTR(identifier);

	RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::OPEN_PARENTHESIS));

	map<string, Type_ptr> arguments;

	while (true)
	{
		if (this->expect_current_token(WTokenType::CLOSE_PARENTHESIS))
			break;

		auto identifier = this->consume_token(WTokenType::Identifier);
		RETURN_NULLPTR_IF_NULLPTR(identifier);

		RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::COLON));

		auto type = this->parse_type();
		RETURN_NULLPTR_IF_NULLPTR(type);

		arguments.insert_or_assign(identifier->get_value(), type);
	}

	std::optional<Type_ptr> return_type = std::nullopt;

	if (this->consume_token(WTokenType::ARROW))
	{
		auto type_temp = this->parse_type();
		RETURN_NULLPTR_IF_NULLPTR(type_temp);

		return_type = std::make_optional(type_temp);
	}

	this->ignore(WTokenType::EOL);

	auto block = this->parse_block();
	RETURN_NULLPTR_IF_NULLPTR(block);

	return make_shared<FunctionDefinition>(is_public, identifier->get_value(), arguments, move(return_type), block);
}

// Type parsers

Type_ptr Parser::parse_type()
{
	if (this->expect_current_token(WTokenType::OPEN_BRACKET))
		return this->parse_vector_type();

	if (this->expect_current_token(WTokenType::OPEN_PARENTHESIS))
		return this->parse_tuple_type();

	if (this->expect_current_token(WTokenType::OPEN_CURLY_BRACE))
		return this->parse_map_type();

	auto type = this->consume_datatype_word();

	if (type != nullptr)
		return type;

	return nullptr;
}

Type_ptr Parser::parse_vector_type()
{
	auto type = this->parse_type();
	RETURN_NULLPTR_IF_NULLPTR(type);

	RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::CLOSE_BRACKET));
	return make_shared<Vector>(move(type));
}

Type_ptr Parser::parse_tuple_type()
{
	vector<Type_ptr> types;

	while (true)
	{
		auto type = this->parse_type();
		RETURN_NULLPTR_IF_NULLPTR(type);

		types.push_back(move(type));

		if (this->expect_current_token(WTokenType::CLOSE_PARENTHESIS))
			return make_shared<Tuple>(types);

		RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::COMMA));
		continue;
	}
}

Type_ptr Parser::parse_map_type()
{
	auto key_type = this->consume_valid_map_key_datatype();
	RETURN_NULLPTR_IF_NULLPTR(key_type);

	RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::ARROW));

	auto value_type = this->parse_type();
	RETURN_NULLPTR_IF_NULLPTR(value_type);

	RETURN_NULLPTR_IF_TRUE(!this->expect_current_token(WTokenType::CLOSE_CURLY_BRACE));
	return make_shared<Map>(move(key_type), move(value_type));
}

Type_ptr Parser::consume_scalar_datatype()
{
	auto token = this->get_current_token();
	RETURN_NULLPTR_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case WTokenType::NUM:
	{
		ADVANCE_PTR;
		return make_shared<Number>();
	}
	case WTokenType::STR:
	{
		ADVANCE_PTR;
		return make_shared<String>();
	}
	case WTokenType::BOOL:
	{
		ADVANCE_PTR;
		return make_shared<Bool>();
	}

	default:
	{
		return nullptr;
	}
	}
}

Type_ptr Parser::consume_valid_map_key_datatype()
{
	auto token = this->get_current_token();
	RETURN_NULLPTR_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case WTokenType::NUM:
	{
		ADVANCE_PTR;
		return make_shared<Number>();
	}
	case WTokenType::STR:
	{
		ADVANCE_PTR;
		return make_shared<String>();
	}
	default:
	{
		return nullptr;
	}
	}
}

Type_ptr Parser::consume_datatype_word()
{
	auto token = this->get_current_token();
	RETURN_NULLPTR_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case WTokenType::NUM:
	{
		ADVANCE_PTR;
		return make_shared<Number>();
	}
	case WTokenType::STR:
	{
		ADVANCE_PTR;
		return make_shared<String>();
	}
	case WTokenType::BOOL:
	{
		ADVANCE_PTR;
		return make_shared<Bool>();
	}
	case WTokenType::Identifier:
	{
		ADVANCE_PTR;
		return make_shared<Record>(token->get_value());
	}
	default:
	{
		return nullptr;
	}
	}
}