#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "ObjectSystem.h"
#include "CodeSection.h"
#include <memory>

// MemorySystem

class MEMORYSYSTEM_API MemorySystem
{
public:
	CodeSection_ptr code_section;
	ObjectSystem_ptr object_system;

	MemorySystem();
	void print();
};

using MemorySystem_ptr = MEMORYSYSTEM_API std::shared_ptr<MemorySystem>;

// InstructionPrinter

class MEMORYSYSTEM_API InstructionPrinter
{
	ObjectStore_ptr object_store;
	ConstantPool_ptr constant_pool;

	std::wstring stringify_instruction(std::byte opcode, std::byte operand);
	std::wstring stringify_instruction(std::byte opcode, std::byte operand_1, std::byte operand_2);

public:
	InstructionPrinter(ObjectStore_ptr object_store, ConstantPool_ptr constant_pool)
		: object_store(object_store), constant_pool(constant_pool) {};

	void print(CodeSection_ptr code_section);
};

using InstructionPrinter_ptr = MEMORYSYSTEM_API std::shared_ptr<InstructionPrinter>;
