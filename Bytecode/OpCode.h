#pragma once

#ifdef BYTECODE_EXPORTS
#define BYTECODE_API __declspec(dllexport)
#else
#define BYTECODE_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <map>
#include <memory>

#define MAKE_OPCODE_DEF(opcode, name, operand_count) \
	{ opcode, std::make_shared<OpCodeDefinition>(name, operand_count) }

enum class BYTECODE_API OpCode
{
	PUSH,
	POP,

	UNARY_ADD,
	UNARY_SUBTRACT,
	UNARY_BANG,

	BINARY_ADD,
	BINARY_SUBTRACT,
	BINARY_MULTIPLY,
	BINARY_DIVISION,
	BINARY_REMINDER,
	BINARY_POWER,

	BINARY_NOT_EQUAL,
	BINARY_EQUAL,

	BINARY_LESSER_THAN,
	BINARY_LESSER_THAN_EQUAL,
	BINARY_GREATER_THAN,
	BINARY_GREATER_THAN_EQUAL,

	BINARY_AND,
	BINARY_OR,

	ASSERT,

	JUMP,
	JUMP_IF_TRUE,
	JUMP_IF_FALSE,

	CALL_FUNCTION,
	CALL_GENERATOR,

	RETURN,
	RETURN_VALUE,
	YIELD_OP,
	YIELD_VALUE_OP,

	GET_VARIABLE,
	SET_VARIABLE,

	CONSTANT,
	CONSTANT_TRUE,
	CONSTANT_FALSE,

	ARRAY,
	TUPLE,

	MAP
};

using Instruction = BYTECODE_API std::vector<std::byte>;
using Instructions = BYTECODE_API std::vector<std::byte>;
