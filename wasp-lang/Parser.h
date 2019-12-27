#pragma once
#include <vector>
#include <memory>
#include "Token.h"
#include "Nodes.h"
#include "Pointer.h"

class Parser
{
	std::vector<std::shared_ptr<Token>> tokens;
	std::vector<bool> inside_function_call;
	Pointer pointer;

	// Token Consumers

	bool is_followed_by(TokenType token_type);
	Token consume_token(TokenType token_type);

	Token consume_scalar_datatype();
	Token consume_valid_key_datatype();
	Token consume_datatype();

	// Parsers

	Node parse_statement();
	ExpressionNode parse_expression();

	// Variable declaration parsers

	Node parse_let_declaration();
	Node parse_const_declaration();

	// Literal parsers

	ExpressionNode parse_vector_literal();
	ExpressionNode parse_tuple_literal();
	ExpressionNode parse_map_literal();
	ExpressionNode parse_record_literal();

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
	Node parse_type_declaration();
	Node parse_function_definition();

	// Other

	Node handle_identifier();
	Node parse_expression_statement();
	Node parse_import_statement();
	Node parse_public_statement();

	// Expression parsing utils

	void pop_all_from_stack_into_ast(std::vector<std::shared_ptr<Token>>& op_stack, std::vector<std::shared_ptr<Expression>>& ast);
	void push_unary_operator_to_ast(std::shared_ptr<Token> op, std::vector<std::shared_ptr<Expression>>& ast);
	void push_binary_operator_into_ast(std::shared_ptr<Token> op, std::vector<std::shared_ptr<Expression>>& ast);
	void pop_until_open_parenthesis_from_stack_into_ast(std::vector<std::shared_ptr<Token>>& op_stack, std::vector<std::shared_ptr<Expression>>& ast);
	void push_operator_to_operator_stack(std::vector<std::shared_ptr<Token>>& op_stack, std::vector<std::shared_ptr<Expression>>& ast);

	// utils

	std::shared_ptr<Token> get_current_token();

public:
	Parser(std::vector<std::shared_ptr<Token>>& token_list) : tokens(std::move(token_list)), pointer(Pointer()) {};
	Module execute();
};
