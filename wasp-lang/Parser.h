#pragma once
#include <vector>
#include <memory>
#include "Token.h"
#include "Pointer.h"
#include "Types.h"
#include "Nodes.h"
#include "ExpressionNodes.h"

using Token_ptr = std::shared_ptr<Token>;
using StatementNode_ptr = std::shared_ptr<StatementNode>;
using ExpressionNode_ptr = std::shared_ptr<ExpressionNode>;
using Type_ptr = std::shared_ptr<TypeNode>;

class Parser
{
	std::vector<Token_ptr> tokens;
	std::vector<bool> inside_function_call;
	Pointer pointer;

	// Utils

	Token_ptr get_current_token();

	// Token Consumers

	bool expect_current_token(TokenType token_type);
	Token_ptr consume_token(TokenType token_type);

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

	Type_ptr parse_type();
	Type_ptr parse_vector_type();
	Type_ptr parse_tuple_type();
	Type_ptr parse_map_type();

	Type_ptr consume_scalar_datatype();
	Type_ptr consume_valid_key_datatype();
	Type_ptr consume_datatype_word();

	// Definition Parsers

	StatementNode_ptr parse_enum_definition();
	StatementNode_ptr parse_type_declaration();
	StatementNode_ptr parse_function_definition();

	// Other

	StatementNode_ptr handle_identifier();
	StatementNode_ptr parse_expression_statement();
	StatementNode_ptr parse_import_statement();
	StatementNode_ptr parse_public_statement();

	// Expression parsing utils

	void pop_all_from_stack_into_ast(std::vector<Token_ptr>& op_stack, std::vector<ExpressionNode_ptr>& ast);
	void push_unary_operator_to_ast(Token_ptr op, std::vector<ExpressionNode_ptr>& ast);
	void push_binary_operator_to_ast(Token_ptr op, std::vector<ExpressionNode_ptr>& ast);
	void pop_until_open_parenthesis_from_stack_into_ast(std::vector<Token_ptr>& op_stack, std::vector<ExpressionNode_ptr>& ast);
	void push_operator_to_operator_stack(Token_ptr op, std::vector<Token_ptr>& op_stack, std::vector<ExpressionNode_ptr>& ast);

public:
	Parser(std::vector<Token_ptr>& token_list) : tokens(std::move(token_list)), pointer(Pointer()) {};
	Module execute();
};
