#pragma once

#ifdef OBJECTSYSTEM_EXPORTS
#define OBJECTSYSTEM_API __declspec(dllexport)
#else
#define OBJECTSYSTEM_API __declspec(dllimport)
#endif

#include <vector>
#include <string>

using ByteVector = OBJECTSYSTEM_API std::vector<std::byte>;

enum class OBJECTSYSTEM_API OpCode
{
	NO_OP,
	START,
	STOP,

	PUSH_TO_STACK,
	POP_FROM_STACK,

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
	NULLISH_COALESE,

	RETURN_VOID,
	RETURN_VALUE,

	YIELD_VOID,
	YIELD_VALUE,

	PUSH_CONSTANT_TRUE,
	PUSH_CONSTANT_FALSE,

	ITERATE_OVER,

	PUSH_CONSTANT, // constant_pool_id

	STORE_LOCAL, // variable_id
	STORE_GLOBAL,

	LOAD_LOCAL, // variable_id
	LOAD_GLOBAL,
	LOAD_BUILTIN,

	MAKE_LIST, // length
	MAKE_TUPLE,
	MAKE_SET,
	MAKE_MAP,

	JUMP, // target
	JUMP_IF_FALSE,

	POP_JUMP,
	POP_JUMP_IF_FALSE,

	LABEL, // label

	CALL_FUNCTION, // id, number of arguments
	CALL_GENERATOR
};

OBJECTSYSTEM_API int get_opcode_arity(std::byte opcode);
OBJECTSYSTEM_API int get_opcode_arity(OpCode opcode);

OBJECTSYSTEM_API std::wstring stringify_opcode(std::byte  opcode);
OBJECTSYSTEM_API std::wstring stringify_opcode(OpCode opcode);
