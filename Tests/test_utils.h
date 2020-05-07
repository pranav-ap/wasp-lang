#pragma once
#include "Token.h"
#include "Lexer.h"
#include "Statement.h"
#include "Module.h"
#include <vector>

#define LEXER_ACT										\
	Lexer lexer(raw_source);							\
	auto tokens = lexer.execute();						\
	auto actual_tokens = prepare_to_assert(tokens);

#define PARSER_ACT										\
	auto prepared_tokens = prepare_to_assert(tokens);	\
	Parser parser(prepared_tokens);						\
	Module mod = parser.execute();						\
	auto actual_statements = prepare_to_assert(mod);

// ------ STATEMENT MACROS ------

#define CREATE_VariableDeclaration(is_public, is_mutable, name, type, expression) VariableDeclaration { is_public, is_mutable, name, type, expression }
#define CREATE_Assignment(name, expression) Assignment { name, expression }
#define CREATE_Branch(condition, consequence, alternative) Branch { condition, consequence, alternative }
#define CREATE_Loop(block) Loop { block }
#define CREATE_Break Break {}
#define CREATE_Continue Continue {}
#define CREATE_UDTDefinition(is_public, name, member_types) UDTDefinition { is_public, name, member_types }
#define CREATE_FunctionDefinition(is_public, name, arguments, return_type, body) FunctionDefinition { is_public, name, arguments, return_type, body }
#define CREATE_Return(expression) Return { expression }
#define CREATE_ExpressionStatement(expression) ExpressionStatement { expression }
#define CREATE_Import(goods, path) Import { goods, path }

// Functions

std::vector<Token> prepare_to_assert(std::vector<Token_ptr> vector_of_token_pointers);
std::vector<Token_ptr> prepare_to_assert(std::vector<Token> vector_of_tokens);
