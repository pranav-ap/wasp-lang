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

public:
	MemorySystem() : bytecode(std::make_shared<Bytecode>()) {};

	// emit

	void emit(OpCode opcode);
	void emit(OpCode opcode, int operand);
	void emit(OpCode opcode, int operand_1, int operand_2);

	// Constant Pool

	void add_to_constant_pool(int id, Object_ptr constant);

	int find_string_constant(std::wstring text);
	int find_number_constant(int number);

	void print();
};

using MemorySystem_ptr = MEMORYSYSTEM_API std::shared_ptr<MemorySystem>;
