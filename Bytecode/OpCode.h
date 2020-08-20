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

	PUSH_CONSTANT_TRUE, // PUSH Boolean Object to Stack
	PUSH_CONSTANT_FALSE,

	PUSH_CONSTANT, // constant_pool_id

	STORE_LOCAL, // variable_id - TOS to Table
	STORE_GLOBAL, // variable_id

	LOAD_LOCAL, // variable_id - Table to TOS
	LOAD_GLOBAL, // variable_id

	MAKE_LIST, // length
	MAKE_TUPLE, // length
	MAKE_MAP, // length

	JUMP_ABSOLUTE, // target
	POP_JUMP_ABSOLUTE, // target
	JUMP_ABSOLUTE_IF_FALSE, // target
	POP_JUMP_ABSOLUTE_IF_FALSE, // target

	JUMP_FORWARD, // skip_id
	JUMP_FORWARD_IF_FALSE, // skip_id
	POP_JUMP_FORWARD_IF_FALSE, // skip_id

	JUMP_BACKWARD, // skip_id
	JUMP_BACKWARD_IF_FALSE, // skip_id
	POP_JUMP_BACKWARD_IF_FALSE, // skip_id

	BREAK_LOOP, // target
	CONTINUE_LOOP, // target

	ITERATE_OVER_LIST, // skip_id
	ITERATE_OVER_MAP, // skip_id
	ITERATE_OVER_STRING, // skip_id

	GET_ELEMENT_FROM_LIST, // variable_id, index
	SET_ELEMENT_IN_LIST, // variable_id, index

	GET_VALUE_FROM_MAP, // variable_id, key_index
	SET_VALUE_FROM_MAP, // variable_id, key_index

	GET_PAIR_FROM_MAP, // variable_id, index
	SET_PAIR_FROM_MAP, // variable_id, index

	GET_CHAR_FROM_STRING, // variable_id, index
	SET_CHAR_FROM_STRING, // variable_id, index

	CALL // function or generator id, number of arguments
};

using Instruction = BYTECODE_API std::vector<std::byte>;
using Instructions = BYTECODE_API std::vector<std::byte>;
