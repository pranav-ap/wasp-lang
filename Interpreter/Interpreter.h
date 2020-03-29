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
	std::list<Environment_ptr> env_list;

	void evaluate_statement(Statement_ptr statement);

	// Expressions

	Object_ptr evaluate_expression(Expression_ptr expression);
	Object_ptr evaluate_unary_expression(Expression_ptr expression);
	Object_ptr evaluate_binary_expression(Expression_ptr expression);

	// Unary Evaluation

	Object_ptr evaluate_bang(Object_ptr operand);
	Object_ptr evaluate_unary_minus(Object_ptr operand);

	// Binary Evaluation

	Object_ptr evaluate_power(Object_ptr left, Object_ptr right);
	Object_ptr evaluate_division(Object_ptr left, Object_ptr right);
	Object_ptr evaluate_reminder(Object_ptr left, Object_ptr right);
	Object_ptr evaluate_plus(Object_ptr left, Object_ptr right);
	Object_ptr evaluate_minus(Object_ptr left, Object_ptr right);
	Object_ptr evaluate_star(Object_ptr left, Object_ptr right);

	Object_ptr evaluate_greater_than(Object_ptr left, Object_ptr right);
	Object_ptr evaluate_greater_than_equal(Object_ptr left, Object_ptr right);
	Object_ptr evaluate_less_than(Object_ptr left, Object_ptr right);
	Object_ptr evaluate_less_than_equal(Object_ptr left, Object_ptr right);
	Object_ptr evaluate_equal_equal(Object_ptr left, Object_ptr right);
	Object_ptr evaluate_bang_equal(Object_ptr left, Object_ptr right);

	Object_ptr evaluate_and(Object_ptr left, Object_ptr right);
	Object_ptr evaluate_or(Object_ptr left, Object_ptr right);

	//Object_ptr evaluate_function_call();

	// Statements

	//Object_ptr evaluate_return(Statement_ptr statement);

	void create_variable(Statement_ptr statement);
	void update_variable(Statement_ptr statement);
	void evaluate_branch(Statement_ptr statement);
	void evaluate_loop(Statement_ptr statement);
	void evaluate_block(Block_ptr block);
	/*void store_UDT(Statement_ptr statement);
	void store_function(Statement_ptr statement);*/

	// Environment Utils

	VariableInfo_ptr get_variable(std::string name);
	FunctionInfo_ptr get_function(std::string name);
	UDTInfo_ptr get_UDT(std::string name);

	void create_and_set_variable(std::string name, VariableInfo_ptr info);
	void set_variable(std::string name, VariableInfo_ptr info);
	void set_function(std::string name, FunctionInfo_ptr info);
	void set_UDT(std::string name, UDTInfo_ptr info);

	// Convert to Object

	NumberObject_ptr convert_to_number_object(Expression_ptr expression);
	StringObject_ptr convert_to_string_object(Expression_ptr expression);
	BooleanObject_ptr convert_to_boolean_object(Expression_ptr expression);
	VectorObject_ptr convert_to_vector_object(Expression_ptr expression);
	TupleObject_ptr convert_to_tuple_object(Expression_ptr expression);
	MapObject_ptr to_map_object(Expression_ptr expression);
	RecordObject_ptr to_record_object(Expression_ptr expression);

public:
	Interpreter(Module mod);
	void execute();
};