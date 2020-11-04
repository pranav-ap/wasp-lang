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

// ObjectStore

class MEMORYSYSTEM_API ObjectStore
{
	std::map<int, Object_ptr> objects;

public:
	std::map<int, std::wstring> name_map;

	void set(int id, Object_ptr value);
	Object_ptr get(int id);
};

using ObjectStore_ptr = MEMORYSYSTEM_API std::shared_ptr<ObjectStore>;

// ConstantPool

class MEMORYSYSTEM_API ConstantPool
{
	std::map<int, Object_ptr> pool;

public:
	int allocate();
	int allocate(int value);
	int allocate(double value);
	int allocate(std::wstring value);
	int allocate(Object_ptr value);

	Object_ptr get(int id);
};

using ConstantPool_ptr = MEMORYSYSTEM_API std::shared_ptr<ConstantPool>;

// CodeSection

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

// MemorySystem

class MEMORYSYSTEM_API MemorySystem
{
public:
	ObjectStore_ptr object_store;
	ConstantPool_ptr constant_pool;
	CodeSection_ptr code_section;

	MemorySystem()
		: object_store(std::make_shared<ObjectStore>()),
		constant_pool(std::make_shared<ConstantPool>()),
		code_section(std::make_shared<CodeSection>()) {};

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
