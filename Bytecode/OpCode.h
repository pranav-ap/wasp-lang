#pragma once

#ifdef BYTECODE_EXPORTS
#define BYTECODE_API __declspec(dllexport)
#else
#define BYTECODE_API __declspec(dllimport)
#endif

#include <vector>

enum class BYTECODE_API OpCode
{
	PUSH,
	POP,

	UNARY_ADD,
	UNARY_SUBTRACT,
	UNARY_BANG,

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

	ASSERT,

	JUMP,
	JUMP_IF_TRUE,
	JUMP_IF_FALSE,

	PASS,

	CALL_FUNCTION,
	CALL_GENERATOR,

	RETURN_VOID,
	RETURN_VALUE,
	YIELD_VOID,
	YIELD_VALUE,

	GET_VARIABLE,
	SET_VARIABLE,

	CONSTANT,
	CONSTANT_TRUE,
	CONSTANT_FALSE,

	LIST,
	TUPLE,

	MAP
};

using Instruction = BYTECODE_API std::vector<std::byte>;
using Instructions = BYTECODE_API std::vector<std::byte>;
