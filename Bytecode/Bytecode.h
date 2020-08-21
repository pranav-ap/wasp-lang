#pragma once

#ifdef BYTECODE_EXPORTS
#define BYTECODE_API __declspec(dllexport)
#else
#define BYTECODE_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "ObjectSystem.h"

#include <memory>
#include <vector>
#include <map>

struct BYTECODE_API Bytecode
{
	Instructions instructions;
	std::map<int, Object_ptr> constant_pool;

	Bytecode(Instructions instructions, std::map<int, Object_ptr> constant_pool)
		: instructions(instructions),
		constant_pool(constant_pool) {};

	void print();

private:
	void print_instruction(OpCode opcode);
	void print_instruction(OpCode opcode, int operand);
	void print_instruction(OpCode opcode, int operand_1, int operand_2);
};

using Bytecode_ptr = BYTECODE_API std::shared_ptr<Bytecode>;

int BYTECODE_API get_opcode_arity(OpCode opcode);
