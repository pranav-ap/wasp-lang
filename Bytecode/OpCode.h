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
	JUMP_TRUE,
	JUMP_FALSE,

	CALL_FUNCTION,
	CALL_GENERATOR,

	RETURN,
	YIELD_OP,

	GET_VARIABLE,
	SET_VARIABLE,

	CONSTANT,

	CONSTANT_TRUE,
	CONSTANT_FALSE
};

using Instruction = BYTECODE_API std::vector<std::byte>;
using Instructions = BYTECODE_API std::vector<std::byte>;

struct BYTECODE_API OpCodeDefinition
{
	std::wstring name;
	int operand_count;

	OpCodeDefinition(std::wstring name, int operand_count)
		: name(name), operand_count(operand_count) {};
};

using OpCodeDefinition_ptr = BYTECODE_API std::shared_ptr<OpCodeDefinition>;

const std::map<OpCode, OpCodeDefinition_ptr> OpCodeDefinitionMap = {
	MAKE_OPCODE_DEF(OpCode::CONSTANT, L"CONSTANT", 1)
};

BYTECODE_API OpCodeDefinition_ptr lookup_definition(OpCode opcode);
