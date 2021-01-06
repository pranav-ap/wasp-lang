#pragma once

#ifdef VIRTUALMACHINE_EXPORTS
#define VIRTUALMACHINE_API __declspec(dllexport)
#else
#define VIRTUALMACHINE_API __declspec(dllimport)
#endif

#include "ObjectStore.h"
#include "CodeObject.h"
#include "LocalScope.h"
#include "CallFrame.h"
#include <memory>
#include <stack>

enum class VIRTUALMACHINE_API OpResult
{
	OK,
	FAILURE,
	DONE
};

class VIRTUALMACHINE_API VirtualMachine
{
	ObjectStore_ptr constant_pool;

	ObjectStore_ptr variable_store; // IDs set by SA
	std::stack<LocalScope_ptr> scope_stack;

	std::stack<CallFrame_ptr> call_stack;
	std::stack<Object_ptr> value_stack;

	// Unary operations

	Object_ptr perform_unary_negative(Object_ptr obj);
	Object_ptr perform_unary_not(Object_ptr obj);

	// Binary operations
	
	Object_ptr perform_add(Object_ptr left, Object_ptr right);
	Object_ptr perform_subtract(Object_ptr left, Object_ptr right);
	Object_ptr perform_multiply(Object_ptr left, Object_ptr right);
	Object_ptr perform_division(Object_ptr left, Object_ptr right);
	Object_ptr perform_reminder(Object_ptr left, Object_ptr right);
	Object_ptr perform_power(Object_ptr left, Object_ptr right);
	Object_ptr perform_not_equal(Object_ptr left, Object_ptr right);
	Object_ptr perform_equal(Object_ptr left, Object_ptr right);
	Object_ptr perform_lesser_than(Object_ptr left, Object_ptr right);
	Object_ptr perform_lesser_than_equal(Object_ptr left, Object_ptr right);
	Object_ptr perform_greater_than(Object_ptr left, Object_ptr right);
	Object_ptr perform_greater_than_equal(Object_ptr left, Object_ptr right);
	Object_ptr perform_and(Object_ptr left, Object_ptr right);
	Object_ptr perform_or(Object_ptr left, Object_ptr right);
	Object_ptr perform_nullish_coalese(Object_ptr left, Object_ptr right);

	// Perform operation

	OpResult perform_nullary_operation(OpCode opcode);
	OpResult perform_unary_operation(OpCode opcode);
	OpResult perform_binary_operation(OpCode opcode);

	// Execute instruction

	OpResult execute(OpCode opcode);
	OpResult execute(OpCode opcode, int operand);
	OpResult execute(OpCode opcode, int operand_1, int operand_2);

	// Utils

	void push_to_value_stack(Object_ptr o);
	Object_ptr pop_from_value_stack();
	ObjectVector pop_n_from_value_stack(int n);
	Object_ptr top_of_value_stack();

	void push_empty_scope_to_local_scope_stack();
	void pop_from_local_scope_stack();

	void push_to_call_stack(CodeObject_ptr function_object, int base_pointer);
	void pop_from_call_stack();

	bool is_truthy(Object_ptr obj);
	Object_ptr make_iterable(Object_ptr obj);

	int get_ip();
	void set_ip(int ip);

	CodeObject_ptr get_current_code_object();

public:
	VirtualMachine(ObjectStore_ptr constant_pool, CodeObject_ptr main_code_object);
	OpResult run();
};

using VirtualMachine_ptr = VIRTUALMACHINE_API std::unique_ptr<VirtualMachine>;
