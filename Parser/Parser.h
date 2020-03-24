#pragma once

#ifdef PARSER_EXPORTS
#define PARSER_API __declspec(dllexport)
#else
#define PARSER_API __declspec(dllimport)
#endif

#include <vector>
#include <stack>
#include <memory>
#include <utility>

#include "Token.h"
#include "Pointer.h"
#include "Types.h"
#include "Module.h"
#include "ExpressionNodes.h"
#include "StatementNodes.h"

#define ADVANCE_PTR this->pointer.advance()
#define RETURN_NULLPTR_IF_NULLPTR(x) if (x == nullptr) { return nullptr; }
#define RETURN_NULLPTR_IF_TRUE(x) if (x) { return nullptr; }
#define CASE(token_type, call) case token_type: { return call; }

class PARSER_API Parser
{
	std::vector<Token_ptr> tokens;
	std::stack<bool> inside_function_call;
	Pointer pointer;

	// Parsers

	Statement_ptr parse_statement(bool is_public);
	Expression_ptr parse_expression();

	// Variable declaration parser
	Statement_ptr parse_variable_declaration(bool is_public, bool is_mutable);

	// Literal parsers

	Expression_ptr parse_vector_literal();
	Expression_ptr parse_tuple_literal();

	Expression_ptr parse_map_literal();
	Expression_ptr parse_record_literal();
	Expression_ptr parse_map_or_record_literal();

	Expression_ptr consume_valid_map_key();
	std::shared_ptr<std::string> consume_valid_record_key();

	// Block statement parsing

	Block_ptr parse_block();

	Statement_ptr parse_return_statement();
	Statement_ptr parse_branching_statement();
	Statement_ptr parse_loop_statement();
	Statement_ptr parse_break_statement();
	Statement_ptr parse_continue_statement();

	// Type parsers

	Type_ptr parse_type();
	Type_ptr parse_vector_type();
	Type_ptr parse_tuple_type();
	Type_ptr parse_map_type();

	Type_ptr consume_scalar_datatype();
	Type_ptr consume_datatype_word();
	Type_ptr consume_valid_map_key_datatype();

	// Definition Parsers

	Statement_ptr parse_enum_definition(bool is_public);
	Statement_ptr parse_type_declaration(bool is_public);
	Statement_ptr parse_function_definition(bool is_public);

	// Other

	Statement_ptr handle_identifier(Token_ptr identifier);
	Statement_ptr parse_expression_statement();
	Statement_ptr parse_import_statement();
	Statement_ptr parse_public_statement();

	// Expression parsing utils

	void pop_all_from_stack_into_ast(std::stack<Token_ptr>& op_stack, std::vector<Expression_ptr>& ast);
	void push_unary_operator_to_ast(Token_ptr op, std::vector<Expression_ptr>& ast);
	void push_binary_operator_to_ast(Token_ptr op, std::vector<Expression_ptr>& ast);
	void pop_until_open_parenthesis_from_stack_into_ast(std::stack<Token_ptr>& op_stack, std::vector<Expression_ptr>& ast);
	void push_operator_to_operator_stack(Token_ptr op, std::stack<Token_ptr>& op_stack, std::vector<Expression_ptr>& ast);

	// Utils

	Token_ptr get_current_token() const;
	Token_ptr consume_token(WTokenType token_type);
	bool expect_current_token(WTokenType token_type);
	void ignore(WTokenType token_type);

public:
	Parser(std::vector<Token_ptr>& token_list) : tokens(std::move(token_list)), pointer(Pointer()) {};
	Module execute();
};

PARSER_API int get_parity(WTokenType token_type);
PARSER_API int get_precedence(WTokenType token_type);
PARSER_API bool is_right_associative(WTokenType token_type);
