#pragma once

#ifdef INTERPRETER_EXPORTS
#define INTERPRETER_API __declspec(dllexport)
#else
#define INTERPRETER_API __declspec(dllimport)
#endif

#include "Module.h"
#include "Statement.h"
#include "ObjectSystem.h"
#include <stack>
#include <map>

struct INTERPRETER_API VariableInfo
{
	bool is_public;
	bool is_mutable;
	Type_ptr type;
	Expression_ptr expression;
};

struct INTERPRETER_API FunctionInfo
{
	bool is_public;
	std::map<std::string, Type_ptr> arguments;
	std::optional<Type_ptr> return_type;
	Block_ptr body;
};

struct INTERPRETER_API TypeInfo
{
	bool is_public;
	std::map<std::string, Type_ptr> member_types;
	std::map<std::string, Expression_ptr> member_values;
};

class INTERPRETER_API Environment
{
	std::map<std::string, VariableInfo> store;
	std::map<std::string, FunctionInfo> fn_store;
	std::map<std::string, TypeInfo> type_store;
};

class INTERPRETER_API Interpreter
{
	Module mod;
	std::stack<Environment> env_stack;

	void evaluate_statement(Statement_ptr statement);

	// Expressions

	Object_ptr evaluate_expression(Expression_ptr expression);
	Object_ptr evaluate_unary_expression(Expression_ptr expression);
	Object_ptr evaluate_binary_expression(Expression_ptr expression);

	// Statements

	Object_ptr evaluate_variable_declaration();
	Object_ptr evaluate_assignment();
	Object_ptr evaluate_branch();
	Object_ptr evaluate_loop();
	Object_ptr evaluate_break();
	Object_ptr evaluate_continue();
	Object_ptr evaluate_alias();
	Object_ptr evaluate_record_definition();
	Object_ptr evaluate_function_definition();
	Object_ptr evaluate_return();
	Object_ptr evaluate_import();
public:
	Interpreter(Module mod) : mod(mod) {};
	void execute();
};