#pragma once

#ifdef VIRTUALMACHINE_EXPORTS
#define VIRTUALMACHINE_API __declspec(dllexport)
#else
#define VIRTUALMACHINE_API __declspec(dllimport)
#endif

#include "ObjectStore.h"
#include "CodeObject.h"
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
	ObjectStore_ptr object_store; // constant pool
	std::stack<Object_ptr> stack;

	CodeObject_ptr code_object;
	int ip; // points to the instruction about to be executed

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

	void push_to_stack(Object_ptr o);
	Object_ptr pop_from_stack();
	ObjectVector pop_n_from_stack(int n);
	Object_ptr top_of_stack();
	
	bool is_truthy(Object_ptr obj);
	Object_ptr make_iterable(Object_ptr obj);
	
	ObjectVector to_vector(std::deque<Object_ptr> values);
	ObjectVector to_vector(std::wstring text);

public:
	VirtualMachine(ObjectStore_ptr object_store, CodeObject_ptr code_object)
		: ip(0), 
		object_store(object_store), 
		code_object(code_object) {};

	OpResult run();
};

using VirtualMachine_ptr = VIRTUALMACHINE_API std::unique_ptr<VirtualMachine>;
