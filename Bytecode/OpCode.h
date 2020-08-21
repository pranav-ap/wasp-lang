#pragma once

#ifdef BYTECODE_EXPORTS
#define BYTECODE_API __declspec(dllexport)
#else
#define BYTECODE_API __declspec(dllimport)
#endif

#include <vector>

enum class BYTECODE_API OpCode
{
	NO_OP,
	STOP,

	PUSH_TO_STACK,
	POP_FROM_STACK,
	POP_N_FROM_STACK,

	UNARY_POSITIVE,
	UNARY_NEGATIVE,
	UNARY_NOT,

	ASSERT,

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
	MAKE_MAP,

	JUMP, // target
	JUMP_IF_FALSE,

	POP_JUMP,
	POP_JUMP_IF_FALSE,

	LABEL, // id

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

	CALL // function or generator id, number of arguments
};

using Instruction = BYTECODE_API std::vector<std::byte>;
using Instructions = BYTECODE_API std::vector<std::byte>;
