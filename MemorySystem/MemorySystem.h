#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "ObjectSystem.h"
#include "Bytecode.h"

#include <memory>
#include <vector>
#include <map>
#include <string>

class MEMORYSYSTEM_API MemorySystem
{
	std::map<int, Object_ptr> constant_pool;
	Bytecode_ptr bytecode;

	std::map<int, std::wstring> id_to_name;

	std::wstring stringify_instruction(std::byte opcode);
	std::wstring stringify_instruction(std::byte opcode, std::byte operand);
	std::wstring stringify_instruction(std::byte opcode, std::byte operand_1, std::byte operand_2);

	std::wstring stringify_instruction_at(int index);

	ByteVector instruction_at(int index);
	ByteVector operands_of(int opcode_index);

public:
	MemorySystem() : bytecode(std::make_shared<Bytecode>()) {};

	// Emit

	void emit(OpCode opcode);
	void emit(OpCode opcode, int operand);
	void emit(OpCode opcode, int operand_1, int operand_2);

	// Constant Pool

	void add_to_constant_pool(int id, Object_ptr constant);
	void bind_name_to_id(int id, std::wstring name);

	int find_string_constant(std::wstring text);
	int find_number_constant(int number);

	void print();
};

using MemorySystem_ptr = MEMORYSYSTEM_API std::shared_ptr<MemorySystem>;
