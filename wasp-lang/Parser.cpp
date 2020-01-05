#pragma once
#include <iostream>
#include "Parser.h"

// API

Module Parser::execute()
{
	Module mod;

	while (true)
	{
		if (this->pointer.get_index() >= this->tokens.size())
			break;

		StatementNode_ptr node = this->parse_statement(false);

		if (node != nullptr)
			mod.add(move(node));
	}

	return mod;
}

// Parsers

StatementNode_ptr Parser::parse_statement(bool is_public)
{
	this->ignore(TokenType::EOL);

	auto token = this->get_current_token();
	ADVANCE_PTR;

	if (token == nullptr)
		return nullptr;

	switch (token->get_type())
	{
		CASE(TokenType::LET, this->parse_let_declaration(is_public));
		CASE(TokenType::CONST, this->parse_const_declaration(is_public));
		CASE(TokenType::Identifier, this->handle_identifier(token));
		CASE(TokenType::BREAK, this->parse_break_statement());
		CASE(TokenType::CONTINUE, this->parse_continue_statement());
		CASE(TokenType::RETURN, this->parse_return_statement());
		CASE(TokenType::PUB, this->parse_public_statement());
		CASE(TokenType::IF, this->parse_branching_statement());
		CASE(TokenType::LOOP, this->parse_loop_statement());
		CASE(TokenType::TYPE, this->parse_type_declaration(is_public));
		//CASE(TokenType::FN, this->parse_function_definition(is_public));
		//CASE(TokenType::ENUM, this->parse_enum_definition(is_public));
		//CASE(TokenType::IMPORT, this->parse_import_statement());
	default: {
		this->pointer.retreat();
		return this->parse_expression_statement();
	}
	}
}

ExpressionNode_ptr Parser::parse_expression()
{
	vector<ExpressionNode_ptr> ast;
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
			ast.push_back(MAKE_EXPR(NumberLiteral(stod(token->get_value()))));
			ADVANCE_PTR;
		}
		else if (type == TokenType::StringLiteral)
		{
			ast.push_back(MAKE_EXPR(StringLiteral(token->get_value())));
			ADVANCE_PTR;
		}
		else if (type == TokenType::TRUE || type == TokenType::FALSE)
		{
			bool x = token->get_value() == "true" ? true : false;
			ast.push_back(MAKE_EXPR(BooleanLiteral(x)));
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

				ast.push_back(MAKE_EXPR(MemberAccess(token->get_value(), expression)));
			}
			else if (this->expect_current_token(TokenType::DOT))
			{
				auto identifier = this->consume_token(TokenType::Identifier);
				RETURN_IF_NULLPTR(identifier);

				ast.push_back(MAKE_EXPR(RecordMemberAccess(token->get_value(), identifier->get_value())));
			}
			else
			{
				ast.push_back(MAKE_EXPR(Identifier(token->get_value())));
			}
		}
		else if (type == TokenType::OPEN_BRACKET)
		{
			ADVANCE_PTR;
			auto vec = this->parse_vector_literal();
			RETURN_IF_NULLPTR(vec);

			ast.push_back(vec);
		}
		else if (type == TokenType::OPEN_PARENTHESIS)
		{
			ADVANCE_PTR;
			auto tuple = this->parse_tuple_literal();
			RETURN_IF_NULLPTR(tuple);

			ast.push_back(tuple);
		}
		else if (type == TokenType::AND || type == TokenType::OR)
		{
			this->push_operator_to_operator_stack(token, op_stack, ast);
			ADVANCE_PTR;
		}
		else if (type == TokenType::OPEN_PARENTHESIS)
		{
			this->inside_function_call.push(false);
			op_stack.push(token);
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
			this->push_operator_to_operator_stack(token, op_stack, ast);
			ADVANCE_PTR;
		}
		else if (type == TokenType::FunctionIdentifier)
		{
			ADVANCE_PTR;
			vector<ExpressionNode_ptr> expressions;

			RETURN_IF_TRUE(!this->expect_current_token(TokenType::OPEN_PARENTHESIS));

			this->inside_function_call.push(true);

			if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
			{
				this->inside_function_call.pop();
				ast.push_back(MAKE_EXPR(FunctionCall(token->get_value(), expressions)));
			}
			else
			{
				while (true)
				{
					auto expression = this->parse_expression();
					RETURN_IF_NULLPTR(expression);

					expressions.push_back(expression);

					if (this->expect_current_token(TokenType::COMMA))
						continue;

					if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
					{
						ast.push_back(MAKE_EXPR(FunctionCall(token->get_value(), expressions)));
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

	return ast[0];
}

// Variable declaration parsers

StatementNode_ptr Parser::parse_let_declaration(bool is_public)
{
	auto identifier = this->consume_token(TokenType::Identifier);
	RETURN_IF_NULLPTR(identifier);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::COLON));

	auto type = this->parse_type();
	RETURN_IF_NULLPTR(type);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EQUAL));

	ExpressionNode_ptr expression = nullptr;

	if (this->expect_current_token(TokenType::OPEN_CURLY_BRACE))
	{
		expression = this->parse_map_or_record_literal();
		RETURN_IF_NULLPTR(expression);
	}

	if (expression == nullptr)
		expression = this->parse_expression();

	RETURN_IF_NULLPTR(expression);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EOL));

	return MAKE_STAT(Let(is_public, identifier->get_value(), type, expression));
}

StatementNode_ptr Parser::parse_const_declaration(bool is_public)
{
	auto identifier = this->consume_token(TokenType::Identifier);
	RETURN_IF_NULLPTR(identifier);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::COLON));

	auto type = this->parse_type();
	RETURN_IF_NULLPTR(type);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EQUAL));

	ExpressionNode_ptr expression = nullptr;

	if (this->expect_current_token(TokenType::OPEN_CURLY_BRACE))
	{
		expression = this->parse_map_or_record_literal();
		RETURN_IF_NULLPTR(expression);
	}

	if (expression == nullptr)
		expression = this->parse_expression();

	RETURN_IF_NULLPTR(expression);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EOL));

	return MAKE_STAT(Const(is_public, identifier->get_value(), type, expression));
}

// Expression Statement

StatementNode_ptr Parser::parse_expression_statement()
{
	// ensure that pointer is at first token of expr ??
	auto expression = this->parse_expression();
	RETURN_IF_NULLPTR(expression);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EOL));

	return MAKE_STAT(ExpressionStatement(expression));
}

// Literal parsers

ExpressionNode_ptr Parser::parse_vector_literal()
{
	vector<ExpressionNode_ptr> elements;

	if (this->expect_current_token(TokenType::CLOSE_BRACKET))
		return MAKE_EXPR(VectorLiteral(elements));

	while (true)
	{
		auto element = this->parse_expression();
		RETURN_IF_NULLPTR(element);

		elements.push_back(element);

		if (this->expect_current_token(TokenType::CLOSE_BRACKET))
			return MAKE_EXPR(VectorLiteral(elements));

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COMMA));
	}
}

ExpressionNode_ptr Parser::parse_tuple_literal()
{
	vector<ExpressionNode_ptr> elements;

	if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
		return MAKE_EXPR(TupleLiteral(elements));

	while (true)
	{
		auto element = this->parse_expression();
		RETURN_IF_NULLPTR(element);

		elements.push_back(element);

		if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
			return MAKE_EXPR(TupleLiteral(elements));

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COMMA));
	}
}

ExpressionNode_ptr Parser::parse_map_literal()
{
	vector<pair<ExpressionNode_ptr, ExpressionNode_ptr>> pairs;

	if (this->expect_current_token(TokenType::CLOSE_CURLY_BRACE))
		return MAKE_EXPR(MapLiteral(pairs));

	while (true)
	{
		this->ignore(TokenType::EOL);

		auto key = this->consume_valid_map_key();
		RETURN_IF_NULLPTR(key);

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COLON));

		auto value = this->parse_expression();
		RETURN_IF_NULLPTR(value);

		pairs.push_back(make_pair(key, value));

		this->ignore(TokenType::EOL);

		if (this->expect_current_token(TokenType::CLOSE_CURLY_BRACE))
			return MAKE_EXPR(MapLiteral(pairs));

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COMMA));
	}
}

ExpressionNode_ptr Parser::parse_record_literal()
{
	vector<pair<string, ExpressionNode_ptr>> pairs;

	if (this->expect_current_token(TokenType::CLOSE_CURLY_BRACE))
		return MAKE_EXPR(RecordLiteral(pairs));

	while (true)
	{
		this->ignore(TokenType::EOL);

		auto key = this->consume_valid_record_key();
		RETURN_IF_NULLPTR(key);

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COLON));

		auto value = this->parse_expression();
		RETURN_IF_NULLPTR(value);

		pairs.push_back(make_pair(*key.get(), value));

		this->ignore(TokenType::EOL);

		if (this->expect_current_token(TokenType::CLOSE_CURLY_BRACE))
			return MAKE_EXPR(RecordLiteral(pairs));

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COMMA));
	}
}

ExpressionNode_ptr Parser::parse_map_or_record_literal()
{
	this->ignore(TokenType::EOL);

	auto token = this->get_current_token();

	if (token->get_type() == TokenType::Identifier)
		return this->parse_record_literal();

	return this->parse_map_literal();
}

ExpressionNode_ptr Parser::consume_valid_map_key()
{
	auto token = this->get_current_token();
	RETURN_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case TokenType::NumberLiteral:
	{
		ADVANCE_PTR;
		return MAKE_EXPR(NumberLiteral(stod(token->get_value())));
	}
	case TokenType::StringLiteral:
	{
		ADVANCE_PTR;
		return MAKE_EXPR(StringLiteral(token->get_value()));
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

StatementNode_ptr Parser::handle_identifier(Token_ptr identifier)
{
	if (this->expect_current_token(TokenType::EQUAL))
	{
		auto expression = this->parse_expression();
		RETURN_IF_NULLPTR(expression);

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::EOL));

		return MAKE_STAT(Assignment(identifier->get_value(), expression));
	}

	this->pointer.retreat();
	return this->parse_expression_statement();
}

StatementNode_ptr Parser::parse_type_declaration(bool is_public)
{
	auto name = this->consume_token(TokenType::Identifier);
	RETURN_IF_NULLPTR(name);

	if (this->expect_current_token(TokenType::OPEN_CURLY_BRACE))
	{
		vector<pair<string, TypeNode_ptr>> member_types;

		while (true)
		{
			this->ignore(TokenType::EOL);

			auto identifier = this->consume_token(TokenType::Identifier);
			RETURN_IF_NULLPTR(identifier);

			RETURN_IF_TRUE(!this->expect_current_token(TokenType::COLON));

			auto type = this->parse_type();
			RETURN_IF_NULLPTR(type);

			member_types.push_back(make_pair(identifier->get_value(), type));

			this->ignore(TokenType::EOL);

			if (this->expect_current_token(TokenType::CLOSE_CURLY_BRACE))
				return MAKE_STAT(RecordDefinition(is_public, name->get_value(), member_types));

			RETURN_IF_TRUE(!this->expect_current_token(TokenType::COMMA));
		}
	}

	auto type = this->parse_type();
	RETURN_IF_NULLPTR(type);

	return MAKE_STAT(Alias(name->get_value(), type));
}

// Public Statement

StatementNode_ptr Parser::parse_public_statement()
{
	auto token = this->get_current_token();
	ADVANCE_PTR;

	if (token == nullptr)
		return nullptr;

	const bool is_public = true;

	switch (token->get_type())
	{
		CASE(TokenType::LET, this->parse_let_declaration(is_public));
		CASE(TokenType::CONST, this->parse_const_declaration(is_public));
		CASE(TokenType::TYPE, this->parse_type_declaration(is_public));
		//CASE(TokenType::FN, this->parse_function_definition(is_public));
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

		statements->push_back(statement);
	}
}

StatementNode_ptr Parser::parse_loop_statement()
{
	auto block = this->parse_block();
	RETURN_IF_NULLPTR(block);

	return MAKE_STAT(Loop(*block.get()));
}

StatementNode_ptr Parser::parse_return_statement()
{
	auto expression = this->parse_expression();
	RETURN_IF_NULLPTR(expression);
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EOL));

	return MAKE_STAT(Return(expression));
}

StatementNode_ptr Parser::parse_branching_statement()
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

			auto alternative = make_shared<vector<StatementNode_ptr>>();
			alternative->push_back(alternative_stat);

			return MAKE_STAT(Branch(condition, *consequence.get(), *alternative.get()));
		}

		auto alternative = this->parse_block();
		RETURN_IF_NULLPTR(alternative);

		return MAKE_STAT(Branch(condition, *consequence.get(), *alternative.get()));
	}

	auto alternative = make_shared<vector<StatementNode_ptr>>();
	return MAKE_STAT(Branch(condition, *consequence.get(), *alternative.get()));
}

StatementNode_ptr Parser::parse_break_statement()
{
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EOL));
	return MAKE_STAT(Break());
}

StatementNode_ptr Parser::parse_continue_statement()
{
	RETURN_IF_TRUE(!this->expect_current_token(TokenType::EOL));
	return MAKE_STAT(Continue());
}

// Type parsers

TypeNode_ptr Parser::parse_type()
{
	if (this->expect_current_token(TokenType::OPEN_BRACKET))
		return this->parse_vector_type();

	if (this->expect_current_token(TokenType::OPEN_PARENTHESIS))
		return this->parse_tuple_type();

	if (this->expect_current_token(TokenType::OPEN_CURLY_BRACE))
		return this->parse_map_type();

	if (this->expect_current_token(TokenType::LESSER_THAN))
		return this->parse_variant_type();

	auto type = this->consume_datatype_word();

	if (type != nullptr)
		return type;

	return nullptr;
}

TypeNode_ptr Parser::parse_vector_type()
{
	auto type = this->parse_type();
	RETURN_IF_NULLPTR(type);

	RETURN_IF_TRUE(!this->expect_current_token(TokenType::CLOSE_BRACKET));
	return MAKE_TYPE(Vector(type));
}

TypeNode_ptr Parser::parse_tuple_type()
{
	vector<TypeNode_ptr> types;

	while (true)
	{
		auto type = this->parse_type();
		RETURN_IF_NULLPTR(type);

		types.push_back(type);

		if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
			return MAKE_TYPE(Tuple(types));

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COMMA));
		continue;
	}
}

TypeNode_ptr Parser::parse_map_type()
{
	auto key_type = this->consume_valid_map_key_datatype();
	RETURN_IF_NULLPTR(key_type);

	RETURN_IF_TRUE(!this->expect_current_token(TokenType::ARROW));

	auto value_type = this->parse_type();
	RETURN_IF_NULLPTR(value_type);

	RETURN_IF_TRUE(!this->expect_current_token(TokenType::CLOSE_CURLY_BRACE));
	return MAKE_TYPE(Map(key_type, value_type));
}

TypeNode_ptr Parser::parse_variant_type()
{
	vector<TypeNode_ptr> types;

	while (true)
	{
		auto type = this->parse_type();
		RETURN_IF_NULLPTR(type);

		types.push_back(type);

		if (this->expect_current_token(TokenType::GREATER_THAN))
			return MAKE_TYPE(Variant(types));

		RETURN_IF_TRUE(!this->expect_current_token(TokenType::COMMA));
		continue;
	}
}

TypeNode_ptr Parser::consume_scalar_datatype()
{
	auto token = this->get_current_token();
	RETURN_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case TokenType::NUM:
	{
		ADVANCE_PTR;
		return MAKE_TYPE(Number());
	}
	case TokenType::STR:
	{
		ADVANCE_PTR;
		return MAKE_TYPE(String());
	}
	case TokenType::BOOL:
	{
		ADVANCE_PTR;
		return MAKE_TYPE(Bool());
	}

	default:
	{
		return nullptr;
	}
	}
}

TypeNode_ptr Parser::consume_valid_map_key_datatype()
{
	auto token = this->get_current_token();
	RETURN_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case TokenType::NUM:
	{
		ADVANCE_PTR;
		return MAKE_TYPE(Number());
	}
	case TokenType::STR:
	{
		ADVANCE_PTR;
		return MAKE_TYPE(String());
	}
	default:
	{
		return nullptr;
	}
	}
}

TypeNode_ptr Parser::consume_datatype_word()
{
	auto token = this->get_current_token();
	RETURN_IF_NULLPTR(token);

	auto token_type = token->get_type();

	switch (token_type)
	{
	case TokenType::NUM:
	{
		ADVANCE_PTR;
		return MAKE_TYPE(Number());
	}
	case TokenType::STR:
	{
		ADVANCE_PTR;
		return MAKE_TYPE(String());
	}
	case TokenType::BOOL:
	{
		ADVANCE_PTR;
		return MAKE_TYPE(Bool());
	}
	case TokenType::Identifier:
	{
		ADVANCE_PTR;
		return MAKE_TYPE(Record(token->get_value()));
	}
	default:
	{
		return nullptr;
	}
	}
}