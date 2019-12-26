#pragma once
#include <vector>
#include <memory>
#include "Token.h"
#include "Nodes.h"
#include "Pointer.h"

class Parser
{
	std::vector<std::unique_ptr<Token>> tokens;
	std::vector<bool> inside_function_call;
	Pointer pointer;

	// Token Consumers

	bool consume_keyword(std::string keyword);
	bool consume_punctuation(std::string keyword);
	bool consume_identifier(std::string keyword);
	bool consume_function_identifier(std::string keyword);
	bool consume_identifier_or_function_identifier(std::string keyword);
	bool consume_string_literal(std::string keyword);
	bool consume_scalar_datatype(std::string keyword);
	bool consume_valid_key_datatype(std::string keyword);
	bool consume_declaration_pair(std::string keyword);
	bool consume_datatype(std::string keyword);

	// Parsers

	Node parse_statement();
	Node parse_expression();

	// Variable declaration parsers

	Node parse_let_declaration();
	Node parse_const_declaration();

	// Literal parsers

	Node parse_vector_literal();
	Node parse_tuple_literal();
	Node parse_map_literal();
	Node parse_record_literal();

	// Block statement parsing

	Node parse_block();
	Node parse_return_statement();
	Node parse_branching_statement();
	Node parse_loop_statement();
	Node parse_break_statement();
	Node parse_continue_statement();

	// Type parsers

	Node parse_type();
	Node parse_vector_type();
	Node parse_tuple_type();
	Node parse_map_type();

	// Definition Parsers

	Node parse_enum_definition();
	Node parse_record_definition();
	Node parse_function_definition();

	// Other

	Node parse_expression_statement();
	Node handle_identifier();
	Node parse_import_statement();
	Node parse_pub();

	// utils

	std::unique_ptr<Token> get_current_token();

public:
	Parser(std::vector<std::unique_ptr<Token>>& token_list) :
		tokens(std::move(token_list)), inside_function_call({}), pointer(Pointer()) {};

	Module execute();
};
