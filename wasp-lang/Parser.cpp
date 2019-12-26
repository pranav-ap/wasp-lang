#include <iostream>
#include "Parser.h"

using namespace std;

// API

Module Parser::execute()
{
	Module mod;

	while (true)
	{
		Node node = this->parse_statement();

		if (node.index() == 0)
		{
			break;
		}

		mod.add(node);
	}

	return mod;
}

// Parsers

Node Parser::parse_statement()
{
	auto token = this->get_current_token();
	this->pointer.advance();

	if (token == NULL) {
		return Node();
	}

	//cout << token->get_value() << endl;

	switch (token->get_type())
	{
	case TokenType::LET: {
		return this->parse_let_declaration();
	}
	case TokenType::CONST: {
		return this->parse_const_declaration();
	}
	case TokenType::PUB: {
		return this->parse_pub();
	}
	default:
		return Node();
	}
}

Node Parser::parse_expression()
{
	return Node();
}

// Variable declaration parsers

Node Parser::parse_let_declaration()
{
	return Node();
}

Node Parser::parse_const_declaration()
{
	return Node();
}

// Literal parsers

Node Parser::parse_vector_literal()
{
	return Node();
}

Node Parser::parse_tuple_literal()
{
	return Node();
}

Node Parser::parse_map_literal()
{
	return Node();
}

Node Parser::parse_record_literal()
{
	return Node();
}

// Statement parsers

Node Parser::parse_block()
{
	return Node();
}

Node Parser::parse_return_statement()
{
	return Node();
}

Node Parser::parse_branching_statement()
{
	return Node();
}

Node Parser::parse_loop_statement()
{
	return Node();
}

Node Parser::parse_break_statement()
{
	return Node();
}

Node Parser::parse_continue_statement()
{
	return Node();
}

Node Parser::parse_import_statement()
{
	return Node();
}

// Type parsers

Node Parser::parse_type()
{
	return Node();
}

Node Parser::parse_vector_type()
{
	return Node();
}

Node Parser::parse_tuple_type()
{
	return Node();
}

Node Parser::parse_map_type()
{
	return Node();
}

// Definition parsers

Node Parser::parse_function_definition()
{
	return Node();
}

Node Parser::parse_enum_definition()
{
	return Node();
}

Node Parser::parse_record_definition()
{
	return Node();
}

// Other

Node Parser::parse_expression_statement()
{
	return Node();
}

Node Parser::handle_identifier()
{
	return Node();
}

Node Parser::parse_pub()
{
	return Node();
}

// Token Consumers

bool Parser::consume_keyword(std::string keyword)
{
	return false;
}

bool Parser::consume_punctuation(std::string keyword)
{
	return false;
}

bool Parser::consume_identifier(std::string keyword)
{
	return false;
}

bool Parser::consume_function_identifier(std::string keyword)
{
	return false;
}

bool Parser::consume_identifier_or_function_identifier(std::string keyword)
{
	return false;
}

bool Parser::consume_string_literal(std::string keyword)
{
	return false;
}

bool Parser::consume_scalar_datatype(std::string keyword)
{
	return false;
}

bool Parser::consume_valid_key_datatype(std::string keyword)
{
	return false;
}

bool Parser::consume_datatype(std::string keyword)
{
	return false;
}

bool Parser::consume_declaration_pair(std::string keyword)
{
	return false;
}

// Utils

unique_ptr<Token> Parser::get_current_token()
{
	int index = this->pointer.get_index();

	if ((size_t)index >= this->tokens.size()) {
		return NULL;
	}

	return move(this->tokens[index]);
}