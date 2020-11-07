#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include <memory>
#include <vector>

struct MEMORYSYSTEM_API CodeSection
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
	ByteVector operands_of(int opcode_index);
};

using CodeSection_ptr = MEMORYSYSTEM_API std::shared_ptr<CodeSection>;
