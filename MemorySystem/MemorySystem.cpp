#pragma once

#include "pch.h"
#include "MemorySystem.h"
#include "ObjectSystem.h"
#include "Assertion.h"
#include <string>
#include <algorithm>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)

using std::byte;
using std::vector;

// MemorySystem

ConstantPool_ptr MemorySystem::get_constant_pool()
{
	return constant_pool;
}

CodeSection_ptr MemorySystem::get_code_section()
{
	return code_section;
}

// CodeSection

int CodeSection::length()
{
	return instructions.size();
}

void CodeSection::push(ByteVector instruction)
{
	instructions.insert(
		std::end(instructions),
		std::begin(instruction),
		std::end(instruction)
	);
}

void CodeSection::replace(int index, std::byte replacement)
{
	instructions.at(index) = replacement;
}

void CodeSection::emit(OpCode opcode)
{
	ByteVector instruction;
	instruction.push_back(static_cast<byte>(opcode));

	push(instruction);
}

void CodeSection::emit(OpCode opcode, int operand)
{
	ByteVector instruction;
	instruction.push_back(static_cast<byte>(opcode));
	instruction.push_back(static_cast<byte>(operand));

	push(instruction);
}

void CodeSection::emit(OpCode opcode, int operand_1, int operand_2)
{
	ByteVector instruction;
	instruction.push_back(static_cast<byte>(opcode));
	instruction.push_back(static_cast<byte>(operand_1));
	instruction.push_back(static_cast<byte>(operand_2));

	push(instruction);
}

// ConstantPool

int ConstantPool::allocate(int number)
{
	auto result = find_if(
		pool.begin(),
		pool.end(),
		[number](const auto& p) {
			if (holds_alternative<NumberObject>(*p.second))
			{
				NumberObject* x = get_if<NumberObject>(&*p.second);
				return x->value == number;
			}

			return false;
		});

	if (result != pool.end())
	{
		return result->first;
	}

	int id = pool.size();
	auto value = MAKE_OBJECT_VARIANT(NumberObject(number));
	pool.insert({ id, value });

	return id;
}

int ConstantPool::allocate(std::wstring text)
{
	auto result = find_if(
		pool.begin(),
		pool.end(),
		[text](const auto& p) {
			if (holds_alternative<StringObject>(*p.second))
			{
				StringObject* x = get_if<StringObject>(&*p.second);
				return x->value == text;
			}

			return false;
		});

	if (result != pool.end())
	{
		return result->first;
	}

	int id = pool.size();
	auto value = MAKE_OBJECT_VARIANT(StringObject(text));
	pool.insert({ id, value });

	return id;
}

Object_ptr ConstantPool::get(int id)
{
	if (pool.contains(id))
	{
		auto value = pool.at(id);
		return value;
	}

	return MAKE_OBJECT_VARIANT(ErrorObject(L"Constant does not exist in pool"));
}