#pragma once

#include "pch.h"
#include "MemorySystem.h"
#include <string>
#include <iostream>
#include <iomanip>

using std::wstring;
using std::cout;
using std::wcout;
using std::endl;

void MemorySystem::emit(OpCode opcode)
{
	std::vector<std::byte> instruction;
	instruction.push_back(static_cast<std::byte>(opcode));

	bytecode->push(instruction);
}

void MemorySystem::emit(OpCode opcode, int operand)
{
	std::vector<std::byte> instruction;
	instruction.push_back(static_cast<std::byte>(opcode));
	instruction.push_back(static_cast<std::byte>(operand));

	bytecode->push(instruction);
}

void MemorySystem::emit(OpCode opcode, int operand_1, int operand_2)
{
	std::vector<std::byte> instruction;
	instruction.push_back(static_cast<std::byte>(opcode));
	instruction.push_back(static_cast<std::byte>(operand_1));
	instruction.push_back(static_cast<std::byte>(operand_2));

	bytecode->push(instruction);
}

// Constant Pool

void MemorySystem::add_to_constant_pool(int id, Object_ptr value)
{
	constant_pool.insert({ id, value });
}

int MemorySystem::find_string_constant(wstring text)
{
	auto result = std::find_if(
		constant_pool.begin(),
		constant_pool.end(),
		[text](const auto& p)
		{
			if (holds_alternative<StringObject>(*p.second))
			{
				StringObject* x = get_if<StringObject>(&*p.second);
				return x->value == text;
			}

			return false;
		}
	);

	if (result != constant_pool.end())
	{
		return result->first;
	}

	return -1;
}

int MemorySystem::find_number_constant(int number)
{
	auto result = std::find_if(
		constant_pool.begin(),
		constant_pool.end(),
		[number](const auto& p)
		{
			if (holds_alternative<NumberObject>(*p.second))
			{
				NumberObject* x = get_if<NumberObject>(&*p.second);
				return x->value == number;
			}

			return false;
		}
	);

	if (result != constant_pool.end())
	{
		return result->first;
	}

	return -1;
}

// Utils

void MemorySystem::print()
{
	cout << "Memory ";
}