#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include <vector>
#include <string>

enum class MEMORYSYSTEM_API OpCode
{
	NO_OP,
	START,
	STOP,

	PUSH_TO_STACK,
	POP_FROM_STACK,
	POP_N_FROM_STACK,

	UNARY_POSITIVE,
	UNARY_NEGATIVE,
	UNARY_NOT,

	ASSERT,
	SWEAR,
	IMPLORE,

	ADD,
	SUBTRACT,
	MULTIPLY,
	DIVISION,
	REMINDER,
	POWER,

	NOT_EQUAL,
	EQUAL,

	LESSER_THAN,
	LESSER_THAN_EQUAL,
	GREATER_THAN,
	GREATER_THAN_EQUAL,

	AND,
	OR,

	RETURN_VOID,
	RETURN_VALUE,

	YIELD_VOID,
	YIELD_VALUE,

	PUSH_CONSTANT_TRUE,
	PUSH_CONSTANT_FALSE,

	PUSH_CONSTANT, // constant_pool_id

	STORE_LOCAL, // variable_id - TOS to Table
	STORE_GLOBAL,

	LOAD_LOCAL, // variable_id - Table to TOS
	LOAD_GLOBAL,
	LOAD_BUILTIN,

	MAKE_LIST, // length
	MAKE_TUPLE,
	MAKE_SET,
	MAKE_MAP,

	MAKE_INSTANCE, // class id, argument count

	JUMP, // target
	JUMP_IF_FALSE,

	POP_JUMP,
	POP_JUMP_IF_FALSE,

	LABEL, // label

	ITERATE_OVER_LIST, // target
	ITERATE_OVER_MAP,
	ITERATE_OVER_STRING,
	ITERATE_OVER_IDENTIFIER,

	GET_ELEMENT_FROM_LIST, // variable_id, index
	SET_ELEMENT_IN_LIST,

	GET_VALUE_FROM_MAP,
	SET_VALUE_FROM_MAP,

	GET_PAIR_FROM_MAP,
	SET_PAIR_FROM_MAP,

	GET_CHAR_FROM_STRING,
	SET_CHAR_FROM_STRING,

	CALL_FUNCTION, // id, number of arguments
	CALL_GENERATOR,

	GET_CLASS_PROPERTY, // class id, property name
	SET_CLASS_PROPERTY,

	GET_CLASS_INSTANCE_PROPERTY, // instance id, property name
	SET_CLASS_INSTANCE_PROPERTY,

	GET_CLASS_FUNCTION, // instance id, method name
	GET_CLASS_GENERATOR,

	GET_ENUM_MEMBER // enum id, member id
};

using ByteVector = MEMORYSYSTEM_API std::vector<std::byte>;

MEMORYSYSTEM_API int get_opcode_arity(std::byte opcode);
MEMORYSYSTEM_API int get_opcode_arity(OpCode opcode);

MEMORYSYSTEM_API std::wstring stringify_opcode(std::byte  opcode);
MEMORYSYSTEM_API std::wstring stringify_opcode(OpCode opcode);
