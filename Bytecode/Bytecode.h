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
#include <string>

struct BYTECODE_API Bytecode
{
	Instructions instructions;
	std::map<int, Object_ptr> constant_pool;
	std::map<int, std::wstring> id_to_name;

	Bytecode(
		Instructions instructions,
		std::map<int, Object_ptr> constant_pool,
		std::map<int, std::wstring> id_to_name)
		: instructions(instructions),
		constant_pool(constant_pool),
		id_to_name(id_to_name) {};

	void print();

private:
	void print_instruction(OpCode opcode);
	void print_instruction(OpCode opcode, int operand);
	void print_instruction(OpCode opcode, int operand_1, int operand_2);
};

using Bytecode_ptr = BYTECODE_API std::shared_ptr<Bytecode>;

int BYTECODE_API get_opcode_arity(std::byte opcode);
int BYTECODE_API get_opcode_arity(OpCode opcode);
