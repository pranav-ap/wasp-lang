#pragma once

#ifdef INTERPRETER_EXPORTS
#define INTERPRETER_API __declspec(dllexport)
#else
#define INTERPRETER_API __declspec(dllimport)
#endif

#include "Module.h"
#include "Statement.h"
#include "ObjectSystem.h"
#include "Environment.h"
#include <list>

class INTERPRETER_API Interpreter
{
	Module mod;
	std::list<Environment> env_list;

	void evaluate_statement(Statement_ptr statement);

	// Expressions

	Object_ptr evaluate_expression(Expression_ptr expression);
	Object_ptr evaluate_unary_expression(Expression_ptr expression);
	Object_ptr evaluate_binary_expression(Expression_ptr expression);

	Object_ptr evaluate_function_call();

	// Statements

	Object_ptr evaluate_return(Statement_ptr statement);

	void create_variable(Statement_ptr statement);
	void update_variable(Statement_ptr statement);
	void evaluate_branch(Statement_ptr statement);
	void evaluate_loop(Statement_ptr statement);
	void store_UDT(Statement_ptr statement);
	void store_function(Statement_ptr statement);

	// Environment Utils

	VariableInfo_ptr get_variable(std::string name);
	FunctionInfo_ptr get_function(std::string name);
	UDTInfo_ptr get_UDT(std::string name);

	void set_variable(std::string name, VariableInfo_ptr info);
	void set_function(std::string name, FunctionInfo_ptr info);
	void set_UDT(std::string name, UDTInfo_ptr info);

	// Convert to Object

	std::shared_ptr<VectorObject> to_vector_object(Expression_ptr expression);
	std::shared_ptr<TupleObject> to_tuple_object(Expression_ptr expression);
	std::shared_ptr<MapObject> to_map_object(Expression_ptr expression);
	std::shared_ptr<RecordObject> to_record_object(Expression_ptr expression);

public:
	Interpreter(Module mod) : mod(mod), env_list({ Environment() }) { };
	void execute();
};