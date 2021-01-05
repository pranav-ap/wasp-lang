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

struct CallFrame
{
	std::shared_ptr<FunctionObject> function_object;
	int ip;
	int base_pointer;

	CallFrame(std::shared_ptr<FunctionObject> function_object, int base_pointer)
		: function_object(function_object), ip(-1), base_pointer(base_pointer) {};
};

using CallFrame_ptr = std::shared_ptr<CallFrame>;

enum class VIRTUALMACHINE_API OpResult
{
	OK,
	FAILURE,
	DONE
};

class VIRTUALMACHINE_API VirtualMachine
{
	int frame_ptr;

	ObjectStore_ptr constant_pool;
	ObjectStore_ptr variable_store; // IDs set by SA

	std::stack<CallFrame_ptr> call_stack;
	std::stack<Object_ptr> value_stack;

	CodeObject_ptr code_object;
	std::map<int, CodeObject_ptr> function_code_objects;
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

public:
	VirtualMachine(ObjectStore_ptr constant_pool, CodeObject_ptr code_object, std::map<int, CodeObject_ptr> function_code_objects)
		: ip(0), 
		constant_pool(constant_pool), 
		variable_store(std::make_shared<ObjectStore>()),
		code_object(code_object),
		function_code_objects(function_code_objects) {};

	OpResult run();
};

using VirtualMachine_ptr = VIRTUALMACHINE_API std::unique_ptr<VirtualMachine>;
