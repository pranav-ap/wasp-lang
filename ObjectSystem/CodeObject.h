#pragma once

#ifdef OBJECTSYSTEM_EXPORTS
#define OBJECTSYSTEM_API __declspec(dllexport)
#else
#define OBJECTSYSTEM_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include <memory>
#include <vector>

struct OBJECTSYSTEM_API CodeObject
{
	ByteVector instructions;

	int length();

	void push(ByteVector instruction);
	void replace(int index, std::byte replacement);
	void set(ByteVector instructions);

	void emit(OpCode opcode);
	void emit(OpCode opcode, int operand);
	void emit(OpCode opcode, int operand_1, int operand_2);

	ByteVector instruction_at(int index);
	ByteVector operands_of_opcode_at(int opcode_index);

	CodeObject() {};

	CodeObject(ByteVector instructions)
		: instructions(instructions) {};		
};

using CodeObject_ptr = OBJECTSYSTEM_API std::shared_ptr<CodeObject>;
