#include <iostream>
#include "Parser.h"

using namespace std;

int get_parity(TokenType token_type)
{
	switch (token_type)
	{
	case TokenType::BANG:
	case TokenType::UNARY_MINUS:
	case TokenType::UNARY_PLUS:
	{
		return 1;
	}
	case TokenType::POWER:
	case TokenType::DIVISION:
	case TokenType::STAR:
	case TokenType::REMINDER:
	case TokenType::PLUS:
	case TokenType::MINUS:
	case TokenType::GREATER_THAN:
	case TokenType::GREATER_THAN_EQUAL:
	case TokenType::LESSER_THAN:
	case TokenType::LESSER_THAN_EQUAL:
	case TokenType::EQUAL_EQUAL:
	case TokenType::BANG_EQUAL:
	case TokenType::EQUAL:
	case TokenType::COMMA:
	case TokenType::AND:
	case TokenType::OR:
	{
		return 2;
	}
	default:
		return 0;
	}
}

int get_precedence(TokenType token_type)
{
	switch (token_type)
	{
	case TokenType::OPEN_PARENTHESIS:
	case TokenType::CLOSE_PARENTHESIS:
	{
		return 11;
	}
	case TokenType::OPEN_BRACKET:
	case TokenType::CLOSE_BRACKET:
	{
		return 10;
	}
	case TokenType::BANG:
	case TokenType::UNARY_MINUS:
	case TokenType::UNARY_PLUS:
	{
		return 9;
	}
	case TokenType::POWER:
	{
		return 8;
	}
	case TokenType::DIVISION:
	case TokenType::STAR:
	case TokenType::REMINDER:
	{
		return 7;
	}
	case TokenType::PLUS:
	case TokenType::MINUS:
	{
		return 6;
	}
	case TokenType::GREATER_THAN:
	case TokenType::GREATER_THAN_EQUAL:
	case TokenType::LESSER_THAN:
	case TokenType::LESSER_THAN_EQUAL:
	{
		return 5;
	}
	case TokenType::EQUAL_EQUAL:
	case TokenType::BANG_EQUAL:
	{
		return 4;
	}
	case TokenType::AND:
	case TokenType::OR:
	{
		return 3;
	}
	case TokenType::EQUAL:
	{
		return 2;
	}
	case TokenType::COMMA:
	{
		return 1;
	}
	default:
		return 0;
	}
}

bool is_right_associative(TokenType token_type)
{
	switch (token_type)
	{
	case TokenType::POWER:
	case TokenType::UNARY_MINUS:
	case TokenType::UNARY_PLUS:
	case TokenType::BANG:
	case TokenType::EQUAL: {
		return true;
	}
	default:
		return false;
	}
}

// API

Module Parser::execute()
{
	Module mod;

	while (true)
	{
		StatementNode_ptr node = this->parse_statement(false);

		if (node->index() == 0)
		{
			break;
		}

		mod.add(move(node));
	}

	return mod;
}

// Parsers

shared_ptr<StatementNode> Parser::parse_statement(bool is_public)
{
	auto token = this->get_current_token();
	this->pointer.advance();

	if (token == nullptr)
	{
		return make_shared<StatementNode>();
	}

	switch (token->get_type())
	{
		/*case TokenType::Identifier:
		{
			return this->handle_identifier();
		}*/
	case TokenType::LET:
	{
		return this->parse_let_declaration(is_public);
	}
	/*case TokenType::CONST:
	{
		return this->parse_const_declaration();
	}
	case TokenType::PUB:
	{
		return this->parse_public_statement();
	}
	case TokenType::IF:
	{
		return this->parse_branching_statement();
	}
	case TokenType::LOOP:
	{
		return this->parse_loop_statement();
	}
	case TokenType::BREAK:
	{
		return this->parse_break_statement();
	}
	case TokenType::CONTINUE:
	{
		return this->parse_continue_statement();
	}
	case TokenType::RETURN:
	{
		return this->parse_return_statement();
	}
	case TokenType::TYPE:
	{
		return this->parse_type_declaration();
	}
	case TokenType::FN:
	{
		return this->parse_function_definition();
	}
	case TokenType::ENUM:
	{
		return this->parse_enum_definition();
	}
	case TokenType::IMPORT:
	{
		return this->parse_import_statement();
	}*/
	default:
		//return parse_expression_statement();
		return make_shared<StatementNode>();
	}
}

ExpressionNode_ptr Parser::parse_expression()
{
	vector<ExpressionNode_ptr> ast;
	vector<Token_ptr> op_stack;

	while (true)
	{
		Token_ptr token = this->get_current_token();

		switch (token->get_type())
		{
		case TokenType::NumberLiteral:
		{
			ast.push_back(
				make_shared<ExpressionNode>(NumberLiteral(stod(token->get_value())))
			);
			this->pointer.advance();
			break;
		}
		case TokenType::StringLiteral:
		{
			ast.push_back(
				make_shared<ExpressionNode>(StringLiteral(token->get_value()))
			);
			this->pointer.advance();
			break;
		}
		case TokenType::BooleanLiteral:
		{
			bool x = token->get_value() == "true" ? true : false;
			ast.push_back(
				make_shared<ExpressionNode>(BooleanLiteral(x))
			);
			this->pointer.advance();
			break;
		}
		case TokenType::Identifier:
		{
			this->pointer.advance();

			if (this->expect_current_token(TokenType::OPEN_BRACKET))
			{
				auto expression = this->parse_expression();
				if (expression != nullptr)
				{
					return make_shared<ExpressionNode>();
				}

				if (!this->expect_current_token(TokenType::CLOSE_BRACKET))
				{
					return make_shared<ExpressionNode>();
				}

				ast.push_back(make_shared<ExpressionNode>(MemberAccess(token->get_value(), expression)));
			}
			else if (this->expect_current_token(TokenType::DOT))
			{
				auto identifier = this->consume_token(TokenType::Identifier);
				if (identifier != nullptr)
				{
					return make_shared<ExpressionNode>();
				}

				ast.push_back(make_shared<ExpressionNode>(RecordMemberAccess(token->get_value(), identifier->get_value())));
			}
			else
			{
				ast.push_back(make_shared<ExpressionNode>(Identifier(token->get_value())));
			}

			break;
		}
		case TokenType::AND:
		case TokenType::OR:
		{
			this->push_operator_to_operator_stack(token, op_stack, ast);
			this->pointer.advance();
			break;
		}
		case TokenType::COMMA:
		{
			break;
		}
		case TokenType::OPEN_PARENTHESIS:
		{
			this->inside_function_call.push_back(false);
			op_stack.push_back(token);
			this->pointer.advance();
			break;
		}
		case TokenType::CLOSE_PARENTHESIS:
		{
			if (this->inside_function_call.back() == true)
			{
				break;
			}

			this->pop_until_open_parenthesis_from_stack_into_ast(op_stack, ast);
			this->pointer.advance();
			break;
		}
		case TokenType::OPEN_CURLY_BRACE:
		case TokenType::CLOSE_CURLY_BRACE:
		case TokenType::CLOSE_BRACKET:
		{
			break;
		}
		case TokenType::BANG:
		case TokenType::UNARY_MINUS:
		case TokenType::UNARY_PLUS:
		case TokenType::PLUS:
		case TokenType::MINUS:
		case TokenType::DIVISION:
		case TokenType::REMINDER:
		case TokenType::STAR:
		case TokenType::POWER:
		case TokenType::GREATER_THAN:
		case TokenType::GREATER_THAN_EQUAL:
		case TokenType::LESSER_THAN:
		case TokenType::LESSER_THAN_EQUAL:
		case TokenType::EQUAL_EQUAL:
		case TokenType::BANG_EQUAL:
		{
			this->push_operator_to_operator_stack(token, op_stack, ast);
			this->pointer.advance();
			break;
		}
		case TokenType::FunctionIdentifier:
		{
			this->pointer.advance();
			vector<ExpressionNode_ptr> expressions;

			if (this->expect_current_token(TokenType::OPEN_PARENTHESIS))
			{
				this->inside_function_call.push_back(true);

				if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
				{
					this->inside_function_call.pop_back();
					return make_shared<ExpressionNode>(FunctionCall(token->get_value(), expressions));
				}
			}

			while (true)
			{
				if (this->expect_current_token(TokenType::COMMA))
				{
					continue;
				}

				if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
				{
					return make_shared<ExpressionNode>(FunctionCall(token->get_value(), expressions));
				}

				return nullptr;
			}

			this->pointer.advance();
			break;
		}
		default:
		{
			this->pointer.advance();
			break;
		}
		}

		this->pop_all_from_stack_into_ast(op_stack, ast);

		if (ast.size() > 1)
		{
			return nullptr;
		}

		return ast[0];
	}

	return nullptr;
}

// Variable declaration parsers

StatementNode_ptr Parser::parse_let_declaration(bool is_public)
{
	auto identifier = this->consume_token(TokenType::Identifier);
	if (identifier == nullptr)
	{
		return make_shared<StatementNode>();
	}

	if (!this->consume_token(TokenType::COLON))
	{
		return make_shared<StatementNode>();
	}

	auto type = this->parse_type();
	if (type == nullptr)
	{
		return make_shared<StatementNode>();
	}

	if (!this->consume_token(TokenType::EQUAL))
	{
		return make_shared<StatementNode>();
	}

	auto expression = this->parse_expression();
	if (expression->index() == 0)
	{
		return make_shared<StatementNode>();
	}

	if (!this->consume_token(TokenType::EOL))
	{
		return make_shared<StatementNode>();
	}

	return make_shared<StatementNode>(Let(is_public, identifier->get_value(), type, expression));
}

// Type parsers

TypeNode_ptr Parser::parse_type()
{
	if (this->expect_current_token(TokenType::OPEN_BRACKET))
	{
		return this->parse_vector_type();
	}

	if (this->expect_current_token(TokenType::OPEN_PARENTHESIS))
	{
		return this->parse_tuple_type();
	}

	if (this->expect_current_token(TokenType::OPEN_CURLY_BRACE))
	{
		return this->parse_map_type();
	}

	if (this->expect_current_token(TokenType::OPEN_ANGLE_BRACKET))
	{
		return this->parse_variant_type();
	}

	auto type = this->consume_datatype_word();
	if (type != nullptr)
	{
		return type;
	}

	return nullptr;
}

TypeNode_ptr Parser::parse_vector_type()
{
	auto type = this->parse_type();

	if (type == nullptr)
	{
		return TypeNode_ptr();
	}

	if (this->expect_current_token(TokenType::CLOSE_BRACKET))
	{
		return make_shared<TypeNode>(Vector(type));
	}

	return TypeNode_ptr();
}

TypeNode_ptr Parser::parse_tuple_type()
{
	vector<TypeNode_ptr> types;

	while (true)
	{
		auto type = this->parse_type();

		if (type == nullptr) {
			return TypeNode_ptr();
		}

		if (this->expect_current_token(TokenType::CLOSE_PARENTHESIS))
		{
			return make_shared<TypeNode>(Tuple(types));
		}

		if (this->expect_current_token(TokenType::COMMA))
		{
			continue;
		}

		return TypeNode_ptr();
	}
}

TypeNode_ptr Parser::parse_map_type()
{
	auto key_type = this->consume_valid_map_key_datatype();

	if (key_type == nullptr) {
		return TypeNode_ptr();
	}

	auto value_type = this->parse_type();

	if (value_type == nullptr) {
		return TypeNode_ptr();
	}

	if (this->expect_current_token(TokenType::CLOSE_CURLY_BRACE))
	{
		return TypeNode_ptr();
	}

	return make_shared<TypeNode>(Map(key_type, value_type));
}

TypeNode_ptr Parser::parse_variant_type()
{
	vector<TypeNode_ptr> types;

	while (true)
	{
		auto type = this->parse_type();

		if (type == nullptr) {
			return TypeNode_ptr();
		}

		if (this->expect_current_token(TokenType::CLOSE_ANGLE_BRACKET))
		{
			return make_shared<TypeNode>(Variant(types));
		}

		if (this->expect_current_token(TokenType::COMMA))
		{
			continue;
		}

		return TypeNode_ptr();
	}
}

TypeNode_ptr Parser::consume_scalar_datatype()
{
	auto token = this->get_current_token();
	auto token_type = token->get_type();

	switch (token_type)
	{
	case TokenType::NUM:
	{
		this->pointer.advance();
		return make_shared<TypeNode>(Number());
	}
	case TokenType::STR:
	{
		this->pointer.advance();
		return make_shared<TypeNode>(String());
	}
	case TokenType::BOOL:
	{
		this->pointer.advance();
		return make_shared<TypeNode>(Bool());
	}

	default:
	{
		return nullptr;
	}
	}
}

KeyTypeNode_ptr Parser::consume_valid_map_key_datatype()
{
	auto token = this->get_current_token();
	auto token_type = token->get_type();

	switch (token_type)
	{
	case TokenType::NUM:
	{
		this->pointer.advance();
		return make_shared<KeyTypeNode>(Number());
	}
	case TokenType::STR:
	{
		this->pointer.advance();
		return make_shared<KeyTypeNode>(String());
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
	auto token_type = token->get_type();

	switch (token_type)
	{
	case TokenType::NUM:
	{
		this->pointer.advance();
		return make_shared<TypeNode>(Number());
	}
	case TokenType::STR:
	{
		this->pointer.advance();
		return make_shared<TypeNode>(String());
	}
	case TokenType::BOOL:
	{
		this->pointer.advance();
		return make_shared<TypeNode>(Bool());
	}
	case TokenType::Identifier:
	{
		this->pointer.advance();
		return make_shared<TypeNode>(Record(token->get_value()));
	}

	default:
	{
		return nullptr;
	}
	}
}

// Expression Utils

void Parser::pop_all_from_stack_into_ast(vector<Token_ptr>& op_stack, vector<ExpressionNode_ptr>& ast)
{
	while (op_stack.size() > 0)
	{
		Token_ptr top_operator = op_stack.back();
		op_stack.pop_back();

		int parity = get_parity(top_operator->get_type());

		if (parity == 1)
		{
			this->push_unary_operator_to_ast(top_operator, ast);
		}
		else if (parity == 2)
		{
			this->push_binary_operator_to_ast(top_operator, ast);
		}
	}
}

void Parser::push_unary_operator_to_ast(Token_ptr op, vector<ExpressionNode_ptr>& ast)
{
	if (ast.size() > 0)
	{
		ExpressionNode_ptr node = ast.back();
		ast.pop_back();

		ast.push_back(make_shared<ExpressionNode>(Unary(op, node)));
	}
}

void Parser::push_binary_operator_to_ast(Token_ptr op, vector<ExpressionNode_ptr>& ast)
{
	if (ast.size() >= 2)
	{
		ExpressionNode_ptr right_expression = ast.back();
		ast.pop_back();

		ExpressionNode_ptr left_expression = ast.back();
		ast.pop_back();

		ast.push_back(make_shared<ExpressionNode>(Binary(left_expression, op, right_expression)));
	}
}

void Parser::pop_until_open_parenthesis_from_stack_into_ast(vector<Token_ptr>& op_stack, vector<ExpressionNode_ptr>& ast)
{
	while (op_stack.size() > 0)
	{
		Token_ptr top_operator = op_stack.back();
		op_stack.pop_back();

		if (top_operator->get_type() == TokenType::OPEN_PARENTHESIS)
		{
			break;
		}

		int parity = get_parity(top_operator->get_type());

		if (parity == 1)
		{
			this->push_unary_operator_to_ast(top_operator, ast);
		}
		else if (parity == 2)
		{
			this->push_binary_operator_to_ast(top_operator, ast);
		}
	}
}

void Parser::push_operator_to_operator_stack(Token_ptr op, vector<Token_ptr>& op_stack, vector<ExpressionNode_ptr>& ast)
{
	int operator_precedence = get_precedence(op->get_type());

	while (op_stack.size() > 0)
	{
		Token_ptr top_operator = op_stack.back();
		int top_operator_precedence = get_precedence(top_operator->get_type());

		if (
			(
				top_operator->get_type() == TokenType::FunctionIdentifier
				|| (top_operator_precedence > operator_precedence)
				|| (top_operator_precedence == operator_precedence && !is_right_associative(top_operator->get_type()))
				)
			&& top_operator->get_type() != TokenType::OPEN_PARENTHESIS
			)
		{
			op_stack.pop_back();

			int parity = get_parity(top_operator->get_type());

			if (parity == 1)
			{
				this->push_unary_operator_to_ast(top_operator, ast);
			}
			else if (parity == 2)
			{
				this->push_binary_operator_to_ast(top_operator, ast);
			}
		}
	}

	op_stack.push_back(op);
}

// Utils

Token_ptr Parser::get_current_token()
{
	int index = this->pointer.get_index();

	if ((size_t)index >= this->tokens.size())
	{
		return nullptr;
	}

	return this->tokens[index];
}

bool Parser::expect_current_token(TokenType token_type)
{
	auto token = this->get_current_token();

	if (token_type == token->get_type())
	{
		this->pointer.advance();
		return true;
	}

	return false;
}

Token_ptr Parser::consume_token(TokenType token_type)
{
	auto token = this->get_current_token();

	if (token_type == token->get_type())
	{
		this->pointer.advance();
		return move(token);
	}

	return nullptr;
}