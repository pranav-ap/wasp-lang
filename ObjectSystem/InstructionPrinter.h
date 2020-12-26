#pragma once

#ifdef OBJECTSYSTEM_EXPORTS
#define OBJECTSYSTEM_API __declspec(dllexport)
#else
#define OBJECTSYSTEM_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "ObjectStore.h"
#include "CodeObject.h"

#include <string>
#include <vector>
#include <memory>

class OBJECTSYSTEM_API InstructionPrinter
{
	ObjectStore_ptr object_store;

	std::wstring stringify_instruction(std::byte opcode, std::byte operand);
	std::wstring stringify_instruction(std::byte opcode, std::byte operand_1, std::byte operand_2);

public:
	InstructionPrinter(ObjectStore_ptr object_store) : object_store(object_store) {};
	void print(CodeObject_ptr code_object);
};

using InstructionPrinter_ptr = OBJECTSYSTEM_API std::shared_ptr<InstructionPrinter>;
