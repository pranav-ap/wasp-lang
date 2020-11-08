#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "ObjectStore.h"
#include "CodeObject.h"
#include <memory>

// MemorySystem

class MEMORYSYSTEM_API MemorySystem
{
public:
	CodeObject_ptr code_object;
	ObjectStore_ptr object_store;

	MemorySystem();
	void print();
};

using MemorySystem_ptr = MEMORYSYSTEM_API std::shared_ptr<MemorySystem>;

// InstructionPrinter

class MEMORYSYSTEM_API InstructionPrinter
{
	ObjectStore_ptr object_store;

	std::wstring stringify_instruction(std::byte opcode, std::byte operand);
	std::wstring stringify_instruction(std::byte opcode, std::byte operand_1, std::byte operand_2);

public:
	InstructionPrinter(ObjectStore_ptr object_store) : object_store(object_store) {};

	void print(CodeObject_ptr code_object);
};

using InstructionPrinter_ptr = MEMORYSYSTEM_API std::shared_ptr<InstructionPrinter>;
