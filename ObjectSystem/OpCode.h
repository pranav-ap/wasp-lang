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
	// 0-arity

	NO_OP,
	START,
	STOP,
	FUNCTION_START,
	FUNCTION_STOP,
	LOCAL_SCOPE_START,
	LOCAL_SCOPE_STOP,
	POP_FROM_STACK,
	UNARY_NEGATIVE,
	UNARY_NOT,
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
	ASSERT,
	SWEAR,
	IMPLORE,
	MAKE_ITERABLE,

	// 1-arity

	PUSH_CONSTANT, // id
	CREATE_LOCAL,
	STORE_LOCAL,
	LOAD_LOCAL,
	MAKE_LIST, // length
	MAKE_TUPLE,
	MAKE_SET,
	MAKE_MAP,
	JUMP, // target
	JUMP_IF_FALSE,
	POP_JUMP,
	POP_JUMP_IF_FALSE,
	GET_NEXT_OR_JUMP,
	LABEL, // label

	// 2-arity

	CALL_FUNCTION, // id, number of arguments
	CALL_GENERATOR
};

OBJECTSYSTEM_API int get_opcode_arity(std::byte opcode);
OBJECTSYSTEM_API int get_opcode_arity(OpCode opcode);

OBJECTSYSTEM_API std::wstring stringify_opcode(std::byte  opcode);
OBJECTSYSTEM_API std::wstring stringify_opcode(OpCode opcode);
