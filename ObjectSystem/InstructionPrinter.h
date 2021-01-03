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
#include <map>

class OBJECTSYSTEM_API InstructionPrinter
{
	ObjectStore_ptr constant_pool;
	std::map<int, std::wstring> name_map;

	std::wstring stringify_instruction(std::byte opcode, std::byte operand);
	std::wstring stringify_instruction(std::byte opcode, std::byte operand_1, std::byte operand_2);

public:
	InstructionPrinter(ObjectStore_ptr constant_pool, std::map<int, std::wstring> name_map) 
		: constant_pool(constant_pool), name_map(name_map) {};

	void print(CodeObject_ptr code_object);
};

using InstructionPrinter_ptr = OBJECTSYSTEM_API std::shared_ptr<InstructionPrinter>;
