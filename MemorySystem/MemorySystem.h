#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "ObjectSystem.h"

#include <memory>
#include <vector>
#include <map>
#include <string>

// ConstantPool

class MEMORYSYSTEM_API ConstantPool
{
	std::map<int, Object_ptr> pool;

public:
	int allocate(int value);
	int allocate(std::wstring value);
	int allocate(Object_ptr value);

	Object_ptr get(int id);
};

using ConstantPool_ptr = MEMORYSYSTEM_API std::shared_ptr<ConstantPool>;

// CodeSection

class MEMORYSYSTEM_API CodeSection
{
	ByteVector instructions;

public:
	int length();

	void push(ByteVector instruction);
	void replace(int index, std::byte replacement);

	void emit(OpCode opcode);
	void emit(OpCode opcode, int operand);
	void emit(OpCode opcode, int operand_1, int operand_2);
};

using CodeSection_ptr = MEMORYSYSTEM_API std::shared_ptr<CodeSection>;

// MemorySystem

class MEMORYSYSTEM_API MemorySystem
{
	ConstantPool_ptr constant_pool;
	CodeSection_ptr code_section;

public:
	ConstantPool_ptr get_constant_pool();
	CodeSection_ptr get_code_section();
};

using MemorySystem_ptr = MEMORYSYSTEM_API std::shared_ptr<MemorySystem>;
