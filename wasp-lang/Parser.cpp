#pragma once
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

// API

Module Parser::execute()
{
	Module mod;

	while (true)
	{
		if (this->tokens.size() <= (size_t)this->pointer.get_index())
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
	this->ignore(TokenType::EOL);

	auto token = this->get_current_token();
	ADVANCE_PTR;

	if (token == nullptr)
		return nullptr;

	switch (token->get_type())
	{
		CASE(TokenType::LET, this->parse_variable_declaration(is_public, true));
		CASE(TokenType::CONST, this->parse_variable_declaration(is_public, false));
		CASE(TokenType::Identifier, this->handle_identifier(move(token)));
		CASE(TokenType::BREAK, this->parse_break_statement());
		CASE(TokenType::CONTINUE, this->parse_continue_statement());
		CASE(TokenType::RETURN, this->parse_return_statement());
		CASE(TokenType::PUB, this->parse_public_statement());
		CASE(TokenType::IF, this->parse_branching_statement());
		CASE(TokenType::LOOP, this->parse_loop_statement());
		CASE(TokenType::TYPE, this->parse_type_declaration(is_public));
		CASE(TokenType::FN, this->parse_function_definition(is_public));
		//CASE(TokenType::ENUM, this->parse_enum_definition(is_public));
		//CASE(TokenType::IMPORT, this->parse_import_statement());
	default: {
		this->pointer.retreat();
		return this->parse_expression_statement();
	}
	}
}

Expression_ptr Parser::parse_expression()
{
	vector<Expression_ptr> ast;
	stack<Token_ptr> op_stack;

	while (true)
	{
		Token_ptr token = this->get_current_token();

		if (token == nullptr)
			return nullptr;

		auto type = token->get_type();

		if (type == TokenType::EOL || type == TokenType::COMMA)
			break;

		if (type == TokenType::NumberLiteral)
		{
			ast.push_back(make_shared<NumberLiteral>(stod(token->get_value())));
			ADVANCE_PTR;
		}
		else if (type == TokenType::StringLiteral)
		{
			ast.push_back(make_shared<StringLiteral>(token->get_value()));
			ADVANCE_PTR;
		}
		else if (type == TokenType::TRUE || type == TokenType::FALSE)
		{
			bool x = token->get_value() == "true" ? true : false;
			ast.push_back(make_shared<BooleanLiteral>(x));
			ADVANCE_PTR;
		}
		else if (type == TokenType::Identifier)
		{
			ADVANCE_PTR;

			if (this->expect_current_token(TokenType::OPEN_BRACKET))
			{
				auto expression = this->parse_expression();
				RETURN_IF_NULLPTR(expression);
				RETURN_IF_TRUE(!this->expect_current_token(TokenType::CLOSE_BRACKET));

				ast.push_back(make_shared<MemberAccess>(token->get_value(), move(expression)));
			}
			else if (this->expect_current_token(TokenType::DOT))
			{
				auto identifier = this->consume_token(TokenType::Identifier);
				RETURN_IF_NULLPTR(identifier);

				auto expr = make_shared<Identifier>(identifier->get_value());

				ast.push_back(make_shared<MemberAccess>(token->get_value(), expr));
			}
			else
			{
				ast.push_back(make_shared<Identifier>(token->get_value()));
			}
		}
		else if (type == TokenType::OPEN_BRACKET)
		{
			ADVANCE_PTR;
			auto vec = this->parse_vector_literal();
			RETURN_IF_NULLPTR(vec);

			ast.push_back(move(vec));
		}
		else if (type == TokenType::OPEN_PARENTHESIS)
		{
			ADVANCE_PTR;
			auto tuple = this->parse_tuple_literal();
			RETURN_IF_NULLPTR(tuple);

			ast.push_back(move(tuple));
		}
		else if (type == TokenType::AND || type == TokenType::OR)
		{
			this->push_operator_to_operator_stack(move(token), op_stack, ast);
			ADVANCE_PTR;
		}
		else if (type == TokenType::OPEN_PARENTHESIS)
		{
			this->inside_function_call.push(false);
			op_stack.push(move(token));
			ADVANCE_PTR;
		}
		else if (type == TokenType::CLOSE_PARENTHESIS) // ???
		{
			if (this->inside_function_call.top() == true)
				break;

			this->pop_until_open_parenthesis_from_stack_into_ast(op_stack, ast);
			ADVANCE_PTR;
			break;
		}
		else if (type == TokenType::OPEN_CURLY_BRACE || type == TokenType::CLOSE_CURLY_BRACE || type == TokenType::CLOSE_BRACKET)
		{
			break;
		}
		else if (type == TokenType::BANG ||
			type == TokenType::UNARY_MINUS ||
			type == TokenType::UNARY_PLUS ||
			type == TokenType::PLUS ||
			type == TokenType::MINUS ||
			type == TokenType::DIVISION ||
			type == TokenType::REMINDER ||
			type == TokenType::STAR ||
			type == TokenType::POWER ||
			type == TokenType::GREATER_THAN ||
			type == TokenType::GREATER_THAN_EQUAL ||
			type == TokenType::LESSER_THAN ||
			type == TokenType::LESSER_THAN_EQUAL ||
			type == TokenType::EQUAL_EQUAL ||
			type == TokenType::BANG_EQUAL)
		{
			this->push_operator_to_operator_stack(move(token), op_stack, ast);
			ADVANCE_PTR;
		}
		else if (type == TokenType::FunctionIdentifier)
		{
			ADVANCE_PTR;
			vector<Expression_ptr> expressions;

			RETURN_IF_TRUE(!this->expect_current_token(TokenType::OPEN_PARENTHESIS));

			this->inside_function_call.push(true);

			if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
			{
				this->inside_function_call.pop();
				ast.push_back(make_shared<FunctionCall>(token->get_value(), expressions));
			}
			else
			{
				while (true)
				{
					auto expression = this->parse_expression();
					RETURN_IF_NULLPTR(expression);

					expressions.push_back(move(expression));

					if (this->expect_current_token(TokenType::COMMA))
						continue;

					if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
					{
						ast.push_back(make_shared<FunctionCall>(token->get_value(), expressions));
						break;
					}
				}
			}
		}
		else
		{
			ADVANCE_PTR; // ??
		}
	}

	this->pop_all_from_stack_into_ast(op_stack, ast);

	if (ast.size() > 1)
		return nullptr;

	return move(ast[0]);
}

// Variable declaration parsers

Statement_ptr Parser::parse_variable_declaration(bool is_public, bool is_mutable)
{
	auto identifier = this->consume_token(TokenType::Identifier);
	RETURN_IF_NULLPTR(identifier);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::COLON));

	auto type = this->parse_type();
	RETURN_IF_NULLPTR(type);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EQUAL));

	Expression_ptr expression = nullptr;

	if (this->expect_current_token(TokenType::OPEN_CURLY_BRACE))
	{
		expression = this->parse_map_or_record_literal();
		RETURN_IF_NULLPTR(expression);
	}

	if (expression == nullptr)
		expression = this->parse_expression();

	RETURN_IF_NULLPTR(expression);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EOL));

	return make_shared<VariableDeclaration>(is_public, is_mutable, identifier->get_value(), move(type), move(expression));
}

// Expression Statement

Statement_ptr Parser::parse_expression_statement()
{
	// ensure that pointer is at first token of expr ??
	auto expression = this->parse_expression();
	RETURN_IF_NULLPTR(expression);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EOL));

	return make_shared<ExpressionStatement>(move(expression));
}

// Literal parsers

Expression_ptr Parser::parse_vector_literal()
{
	vector<Expression_ptr> elements;

	if (this->expect_current_token(TokenType::CLOSE_BRACKET))
		return make_shared<VectorLiteral>(elements);

	while (true)
	{
		auto element = this->parse_expression();
		RETURN_IF_NULLPTR(element);

		elements.push_back(move(element));

		if (this->expect_current_token(TokenType::CLOSE_BRACKET))
			return make_shared<VectorLiteral>(elements);

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COMMA));
	}
}

Expression_ptr Parser::parse_tuple_literal()
{
	vector<Expression_ptr> elements;

	if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
		return make_shared<TupleLiteral>(elements);

	while (true)
	{
		auto element = this->parse_expression();
		RETURN_IF_NULLPTR(element);

		elements.push_back(move(element));

		if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
			return make_shared<TupleLiteral>(elements);

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COMMA));
	}
}

Expression_ptr Parser::parse_map_literal()
{
	vector<pair<Expression_ptr, Expression_ptr>> pairs;

	if (this->expect_current_token(TokenType::CLOSE_CURLY_BRACE))
		return make_shared<MapLiteral>(pairs);

	while (true)
	{
		this->ignore(TokenType::EOL);

		auto key = this->consume_valid_map_key();
		RETURN_IF_NULLPTR(key);

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COLON));

		auto value = this->parse_expression();
		RETURN_IF_NULLPTR(value);

		pairs.push_back(make_pair(move(key), move(value)));

		this->ignore(TokenType::EOL);

		if (this->expect_current_token(TokenType::CLOSE_CURLY_BRACE))
			return make_shared<MapLiteral>(pairs);

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COMMA));
	}
}

Expression_ptr Parser::parse_record_literal()
{
	vector<pair<string, Expression_ptr>> pairs;

	if (this->expect_current_token(TokenType::CLOSE_CURLY_BRACE))
		return make_shared<RecordLiteral>(pairs);

	while (true)
	{
		this->ignore(TokenType::EOL);

		auto key = this->consume_valid_record_key();
		RETURN_IF_NULLPTR(key);

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COLON));

		auto value = this->parse_expression();
		RETURN_IF_NULLPTR(value);

		pairs.push_back(make_pair(*key.get(), move(value)));

		this->ignore(TokenType::EOL);

		if (this->expect_current_token(TokenType::CLOSE_CURLY_BRACE))
			return make_shared<RecordLiteral>(pairs);

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COMMA));
	}
}

Expression_ptr Parser::parse_map_or_record_literal()
{
	this->ignore(TokenType::EOL);

	auto token = this->get_current_token();

	if (token->get_type() == TokenType::Identifier)
		return this->parse_record_literal();

	return this->parse_map_literal();
}

Expression_ptr Parser::consume_valid_map_key()
{
	auto token = this->get_current_token();
	RETURN_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case TokenType::NumberLiteral:
	{
		ADVANCE_PTR;
		return make_shared<NumberLiteral>(stod(token->get_value()));
	}
	case TokenType::StringLiteral:
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
	RETURN_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case TokenType::Identifier:
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
	if (this->expect_current_token(TokenType::EQUAL))
	{
		auto expression = this->parse_expression();
		RETURN_IF_NULLPTR(expression);

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::EOL));

		return make_shared<Assignment>(identifier->get_value(), move(expression));
	}

	this->pointer.retreat();
	return this->parse_expression_statement();
}

Statement_ptr Parser::parse_type_declaration(bool is_public)
{
	auto name = this->consume_token(TokenType::Identifier);
	RETURN_IF_NULLPTR(name);

	if (this->expect_current_token(TokenType::OPEN_CURLY_BRACE))
	{
		vector<pair<string, Type_ptr>> member_types;

		while (true)
		{
			this->ignore(TokenType::EOL);

			auto identifier = this->consume_token(TokenType::Identifier);
			RETURN_IF_NULLPTR(identifier);

			RETURN_IF_TRUE(!this->expect_current_token(TokenType::COLON));

			auto type = this->parse_type();
			RETURN_IF_NULLPTR(type);

			member_types.push_back(make_pair(identifier->get_value(), move(type)));

			this->ignore(TokenType::EOL);

			if (this->expect_current_token(TokenType::CLOSE_CURLY_BRACE))
				return make_shared<RecordDefinition>(is_public, name->get_value(), member_types);

			RETURN_IF_TRUE(!this->expect_current_token(TokenType::COMMA));
		}
	}

	auto type = this->parse_type();
	RETURN_IF_NULLPTR(type);

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
		CASE(TokenType::LET, this->parse_variable_declaration(is_public, true));
		CASE(TokenType::CONST, this->parse_variable_declaration(is_public, false));
		CASE(TokenType::TYPE, this->parse_type_declaration(is_public));
		CASE(TokenType::FN, this->parse_function_definition(is_public));
		//CASE(TokenType::ENUM, this->parse_enum_definition(is_public));
	default: {
		return nullptr;
	}
	}
}

// Block Statements Parsers

shared_ptr<Block> Parser::parse_block()
{
	shared_ptr<Block> statements = make_shared<Block>();

	this->ignore(TokenType::EOL);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::OPEN_CURLY_BRACE));

	while (true)
	{
		if (this->expect_current_token(TokenType::CLOSE_CURLY_BRACE))
			return statements;

		this->ignore(TokenType::EOL);

		auto statement = this->parse_statement(false);
		RETURN_IF_NULLPTR(statement);

		statements->push_back(move(statement));
	}
}

Statement_ptr Parser::parse_loop_statement()
{
	auto block = this->parse_block();
	RETURN_IF_NULLPTR(block);

	return make_shared<Loop>(*block.get());
}

Statement_ptr Parser::parse_return_statement()
{
	auto expression = this->parse_expression();
	RETURN_IF_NULLPTR(expression);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EOL));

	return make_shared<Return>(std::make_optional(move(expression)));
}

Statement_ptr Parser::parse_branching_statement()
{
	auto condition = this->parse_expression();
	RETURN_IF_NULLPTR(condition);

	auto consequence = this->parse_block();
	RETURN_IF_NULLPTR(consequence);

	if (this->expect_current_token(TokenType::ELSE))
	{
		if (this->expect_current_token(TokenType::IF))
		{
			auto alternative_stat = this->parse_branching_statement();
			RETURN_IF_NULLPTR(alternative_stat);

			auto alternative = make_shared<vector<Statement_ptr>>();
			alternative->push_back(move(alternative_stat));

			return make_shared<Branch>(move(condition), *consequence.get(), *alternative.get());
		}

		auto alternative = this->parse_block();
		RETURN_IF_NULLPTR(alternative);

		return make_shared<Branch>(move(condition), *consequence.get(), *alternative.get());
	}

	auto alternative = make_shared<vector<Statement_ptr>>();
	return make_shared<Branch>(move(condition), *consequence.get(), *alternative.get());
}

Statement_ptr Parser::parse_break_statement()
{
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EOL));
	return make_shared<Break>();
}

Statement_ptr Parser::parse_continue_statement()
{
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EOL));
	return make_shared<Continue>();
}

Statement_ptr Parser::parse_function_definition(bool is_public)
{
	auto identifier = this->consume_token(TokenType::FunctionIdentifier);
	RETURN_IF_NULLPTR(identifier);

	RETURN_IF_TRUE(!this->expect_current_token(TokenType::OPEN_PARENTHESIS));

	vector<pair<string, Type_ptr>> arguments;

	while (true)
	{
		if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
			break;

		auto identifier = this->consume_token(TokenType::Identifier);
		RETURN_IF_NULLPTR(identifier);

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COLON));

		auto type = this->parse_type();
		RETURN_IF_NULLPTR(type);

		arguments.push_back(make_pair(identifier->get_value(), move(type)));
	}

	std::optional<Type_ptr> return_type = std::nullopt;

	if (this->consume_token(TokenType::ARROW))
	{
		auto type_temp = this->parse_type();
		RETURN_IF_NULLPTR(type_temp);

		return_type = std::make_optional(type_temp);
	}

	this->ignore(TokenType::EOL);

	auto block = this->parse_block();
	RETURN_IF_NULLPTR(block);

	return make_shared<FunctionDefinition>(is_public, identifier->get_value(), arguments, move(return_type), *block.get());
}

// Type parsers

Type_ptr Parser::parse_type()
{
	if (this->expect_current_token(TokenType::OPEN_BRACKET))
		return this->parse_vector_type();

	if (this->expect_current_token(TokenType::OPEN_PARENTHESIS))
		return this->parse_tuple_type();

	if (this->expect_current_token(TokenType::OPEN_CURLY_BRACE))
		return this->parse_map_type();

	auto type = this->consume_datatype_word();

	if (type != nullptr)
		return type;

	return nullptr;
}

Type_ptr Parser::parse_vector_type()
{
	auto type = this->parse_type();
	RETURN_IF_NULLPTR(type);

	RETURN_IF_TRUE(!this->expect_current_token(TokenType::CLOSE_BRACKET));
	return make_shared<Vector>(move(type));
}

Type_ptr Parser::parse_tuple_type()
{
	vector<Type_ptr> types;

	while (true)
	{
		auto type = this->parse_type();
		RETURN_IF_NULLPTR(type);

		types.push_back(move(type));

		if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
			return make_shared<Tuple>(types);

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COMMA));
		continue;
	}
}

Type_ptr Parser::parse_map_type()
{
	auto key_type = this->consume_valid_map_key_datatype();
	RETURN_IF_NULLPTR(key_type);

	RETURN_IF_TRUE(!this->expect_current_token(TokenType::ARROW));

	auto value_type = this->parse_type();
	RETURN_IF_NULLPTR(value_type);

	RETURN_IF_TRUE(!this->expect_current_token(TokenType::CLOSE_CURLY_BRACE));
	return make_shared<Map>(move(key_type), move(value_type));
}

Type_ptr Parser::consume_scalar_datatype()
{
	auto token = this->get_current_token();
	RETURN_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case TokenType::NUM:
	{
		ADVANCE_PTR;
		return make_shared<Number>();
	}
	case TokenType::STR:
	{
		ADVANCE_PTR;
		return make_shared<String>();
	}
	case TokenType::BOOL:
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
	RETURN_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case TokenType::NUM:
	{
		ADVANCE_PTR;
		return make_shared<Number>();
	}
	case TokenType::STR:
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
	RETURN_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case TokenType::NUM:
	{
		ADVANCE_PTR;
		return make_shared<Number>();
	}
	case TokenType::STR:
	{
		ADVANCE_PTR;
		return make_shared<String>();
	}
	case TokenType::BOOL:
	{
		ADVANCE_PTR;
		return make_shared<Bool>();
	}
	case TokenType::Identifier:
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