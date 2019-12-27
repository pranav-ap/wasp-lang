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

	if (token == nullptr)
	{
		return Node();
	}

	switch (token->get_type())
	{
	case TokenType::Identifier: {
		return this->handle_identifier();
	}
	case TokenType::LET: {
		return this->parse_let_declaration();
	}
	case TokenType::CONST: {
		return this->parse_const_declaration();
	}
	case TokenType::PUB: {
		return this->parse_public_statement();
	}
	case TokenType::IF: {
		return this->parse_branching_statement();
	}
	case TokenType::LOOP: {
		return this->parse_loop_statement();
	}
	case TokenType::BREAK: {
		return this->parse_break_statement();
	}
	case TokenType::CONTINUE: {
		return this->parse_continue_statement();
	}
	case TokenType::RETURN: {
		return this->parse_return_statement();
	}
	case TokenType::TYPE: {
		return this->parse_type_declaration();
	}
	case TokenType::FN: {
		return this->parse_function_definition();
	}
	case TokenType::ENUM: {
		return this->parse_enum_definition();
	}
	case TokenType::IMPORT: {
		return this->parse_import_statement();
	}
	default:
		return parse_expression_statement();
	}
}

ExpressionNode Parser::parse_expression()
{
}

// Variable declaration parsers

Node Parser::parse_let_declaration()
{
	auto identifier = this->consume_token(TokenType::Identifier);
	if (identifier == nullptr) {
		return Node();
	}

	if (!this->consume_token(TokenType::COLON)) {
		return Node();
	}

	auto datatype = this->consume_datatype();
	if (datatype == nullptr) {
		return Node();
	}

	if (!this->consume_token(TokenType::EQUAL)) {
		return Node();
	}

	auto expression = this->parse_expression();
	if (expression.index() == 0) {
		return Node();
	}

	if (!this->consume_token(TokenType::EOL)) {
		return Node();
	}
}

Node Parser::parse_const_declaration()
{
	auto identifier = this->consume_token(TokenType::CONST);
	if (identifier == nullptr) {
		return Node();
	}

	if (!this->consume_token(TokenType::COLON)) {
		return Node();
	}

	auto datatype = this->consume_datatype();
	if (datatype == nullptr) {
		return Node();
	}

	if (!this->consume_token(TokenType::EQUAL)) {
		return Node();
	}

	auto expression = this->parse_expression();
	if (expression.index() == 0) {
		return Node();
	}

	if (!this->consume_token(TokenType::EOL)) {
		return Node();
	}
}

// Literal parsers

ExpressionNode Parser::parse_vector_literal()
{
	if (!this->is_followed_by(TokenType::OPEN_BRACKET))
	{
		return ExpressionNode();
	}

	// Empty vector

	if (!this->is_followed_by(TokenType::CLOSE_BRACKET))
	{
		return ExpressionNode(VectorLiteral());
	}

	// Non Empty vector

	vector<ExpressionNode> expressions;

	while (true)
	{
		ExpressionNode expression_node = this->parse_expression();
		if (expression_node.index() == 0) {
			return ExpressionNode();
		}

		if (this->is_followed_by(TokenType::COMMA))
		{
			continue;
		}

		break;
	}

	if (!this->is_followed_by(TokenType::CLOSE_BRACKET))
	{
		return ExpressionNode();
	}

	return ExpressionNode(VectorLiteral(expressions));
}

ExpressionNode Parser::parse_tuple_literal()
{
	return ExpressionNode();
}

ExpressionNode Parser::parse_map_literal()
{
	return ExpressionNode();
}

ExpressionNode Parser::parse_record_literal()
{
	return ExpressionNode();
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

Node Parser::parse_type_declaration()
{
	return Node();
}

// Other

Node Parser::parse_expression_statement()
{
	auto expression = this->parse_expression();
	if (expression.index() == 0) {
		return Node();
	}

	if (!this->consume_token(TokenType::EOL)) {
		return Node();
	}
}

Node Parser::handle_identifier()
{
	return Node();
}

Node Parser::parse_public_statement()
{
	return Node();
}

// Token Consumers

bool Parser::is_followed_by(TokenType token_type)
{
	auto token = this->refer_current_token();

	if (token_type == token.get_type())
	{
		this->pointer.advance();
		return true;
	}

	return false;
}

Token Parser::consume_token(TokenType token_type)
{
	auto token = this->refer_current_token();

	if (token_type == token.get_type())
	{
		this->pointer.advance();
		return token;
	}

	return;
}

Token Parser::consume_scalar_datatype()
{
	auto token = this->refer_current_token();
	auto token_type = token.get_type();

	switch (token_type)
	{
	case TokenType::NUM:
	case TokenType::STR:
	case TokenType::BOOL: {
		this->pointer.advance();
		return token;
	}

	default: {
		return;
	}
	}
}

Token Parser::consume_valid_key_datatype()
{
	auto token = this->refer_current_token();
	auto token_type = token.get_type();

	switch (token_type)
	{
	case TokenType::NUM:
	case TokenType::STR: {
		this->pointer.advance();
		return token;
	}

	default: {
		return;
	}
	}
}

Token Parser::consume_datatype()
{
	auto token = this->refer_current_token();
	auto token_type = token.get_type();

	switch (token_type)
	{
	case TokenType::NUM:
	case TokenType::STR:
	case TokenType::BOOL:
	case TokenType::Identifier:
	{
		this->pointer.advance();
		return token;
	}

	default: {
		return;
	}
	}
}

// Utils

unique_ptr<Token> Parser::get_current_token()
{
	int index = this->pointer.get_index();

	if ((size_t)index >= this->tokens.size()) {
		return nullptr;
	}

	return move(this->tokens[index]);
}

const Token& Parser::refer_current_token()
{
	int index = this->pointer.get_index();

	if ((size_t)index >= this->tokens.size()) {
		return;
	}

	return *this->tokens[index].get();
}

// Expression Utils

void Parser::pop_all_from_stack_into_ast(std::vector<std::unique_ptr<Token>>& op_stack, std::vector<std::unique_ptr<Expression>>& ast)
{
}

void Parser::push_unary_operator_to_ast(std::unique_ptr<Token> op, std::vector<std::unique_ptr<Expression>>& ast)
{
}

void Parser::push_binary_operator_into_ast(std::unique_ptr<Token> op, std::vector<std::unique_ptr<Expression>>& ast)
{
}

void Parser::pop_until_open_parenthesis_from_stack_into_ast(std::vector<std::unique_ptr<Token>>& op_stack, std::vector<std::unique_ptr<Expression>>& ast)
{
}

void Parser::push_operator_to_operator_stack(std::vector<std::unique_ptr<Token>>& op_stack, std::vector<std::unique_ptr<Expression>>& ast)
{
}