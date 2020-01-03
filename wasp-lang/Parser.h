#pragma once
#include <vector>
#include <stack>
#include <memory>
#include "Token.h"
#include "Pointer.h"
#include "Types.h"
#include "Nodes.h"

class Parser
{
	std::vector<Token_ptr> tokens;
	std::stack<bool> inside_function_call;
	Pointer pointer;

	// Parsers

	StatementNode_ptr parse_statement(bool is_public);
	ExpressionNode_ptr parse_expression();

	// Variable declaration parsers

	StatementNode_ptr parse_let_declaration(bool is_public);
	StatementNode_ptr parse_const_declaration(bool is_public);

	// Literal parsers

	ExpressionNode_ptr parse_vector_literal();
	ExpressionNode_ptr parse_tuple_literal();
	ExpressionNode_ptr parse_map_literal();
	ExpressionNode_ptr parse_record_literal();

	// Block statement parsing

	StatementNode_ptr parse_block();
	StatementNode_ptr parse_return_statement();
	StatementNode_ptr parse_branching_statement();
	StatementNode_ptr parse_loop_statement();
	StatementNode_ptr parse_break_statement();
	StatementNode_ptr parse_continue_statement();

	// Type parsers

	TypeNode_ptr parse_type();
	TypeNode_ptr parse_vector_type();
	TypeNode_ptr parse_tuple_type();
	TypeNode_ptr parse_map_type();
	TypeNode_ptr parse_variant_type();

	TypeNode_ptr consume_scalar_datatype();
	TypeNode_ptr consume_datatype_word();

	KeyTypeNode_ptr consume_valid_map_key_datatype();

	// Definition Parsers

	StatementNode_ptr parse_enum_definition(bool is_public);
	StatementNode_ptr parse_type_declaration(bool is_public);
	StatementNode_ptr parse_function_definition(bool is_public);

	// Other

	StatementNode_ptr handle_identifier();
	StatementNode_ptr parse_expression_statement();
	StatementNode_ptr parse_import_statement();
	StatementNode_ptr parse_public_statement();

	// Expression parsing utils

	void pop_all_from_stack_into_ast(std::stack<Token_ptr>& op_stack, std::vector<ExpressionNode_ptr>& ast);
	void push_unary_operator_to_ast(Token_ptr op, std::vector<ExpressionNode_ptr>& ast);
	void push_binary_operator_to_ast(Token_ptr op, std::vector<ExpressionNode_ptr>& ast);
	void pop_until_open_parenthesis_from_stack_into_ast(std::stack<Token_ptr>& op_stack, std::vector<ExpressionNode_ptr>& ast);
	void push_operator_to_operator_stack(Token_ptr op, std::stack<Token_ptr>& op_stack, std::vector<ExpressionNode_ptr>& ast);

	// Utils

	Token_ptr get_current_token();
	Token_ptr consume_token(TokenType token_type);
	bool expect_current_token(TokenType token_type);

public:
	Parser(std::vector<Token_ptr>& token_list) : tokens(std::move(token_list)), pointer(Pointer()) {};
	Module execute();
};

int get_parity(TokenType token_type);
int get_precedence(TokenType token_type);
bool is_right_associative(TokenType token_type);
