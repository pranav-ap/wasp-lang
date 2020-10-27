#pragma once

#include "pch.h"
#include "MemorySystem.h"
#include "ObjectSystem.h"
#include "Assertion.h"
#include <string>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define OPCODE_WIDTH 23
#define OPERAND_WIDTH 8

using std::byte;
using std::vector;
using std::move;
using std::wcout;
using std::endl;
using std::setw;
using std::wstring;
using std::to_wstring;
using std::to_string;

// MemorySystem

ConstantPool_ptr MemorySystem::get_constant_pool()
{
	return constant_pool;
}

CodeSection_ptr MemorySystem::get_code_section()
{
	return code_section;
}

void MemorySystem::print()
{
	InstructionPrinter_ptr printer = make_shared<InstructionPrinter>(constant_pool);
	printer->print(code_section);
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

void CodeSection::set(ByteVector instructions)
{
	this->instructions = instructions;
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

ByteVector CodeSection::instruction_at(int index)
{
	byte opcode = instructions.at(index);
	ByteVector operands = operands_of(index);

	ByteVector instruction{ opcode };
	instruction.insert(
		end(instruction),
		begin(operands),
		end(operands));

	return instruction;
}

ByteVector CodeSection::operands_of(int opcode_index)
{
	byte opcode = instructions.at(opcode_index);
	int arity = get_opcode_arity(opcode);

	switch (arity)
	{
	case 0:
	{
		return {};
	}
	case 1:
	{
		byte operand = instructions.at(++opcode_index);
		return { operand };
	}
	case 2:
	{
		byte operand_1 = instructions.at(++opcode_index);
		byte operand_2 = instructions.at(++opcode_index);
		return { operand_1, operand_2 };
	}
	default:
	{
		return {};
	}
	}
}

// ConstantPool

int ConstantPool::allocate(int number)
{
	auto result = find_if(
		pool.begin(),
		pool.end(),
		[number](const auto& p) {
			if (holds_alternative<IntObject>(*p.second))
			{
				IntObject* x = get_if<IntObject>(&*p.second);
				return x->value == number;
			}

			return false;
		});

	if (result != pool.end())
	{
		return result->first;
	}

	int id = pool.size();
	auto value = MAKE_OBJECT_VARIANT(IntObject(number));
	pool.insert({ id, value });

	return id;
}

int ConstantPool::allocate(double number)
{
	auto result = find_if(
		pool.begin(),
		pool.end(),
		[number](const auto& p) {
			if (holds_alternative<FloatObject>(*p.second))
			{
				FloatObject* x = get_if<FloatObject>(&*p.second);
				return x->value == number;
			}

			return false;
		});

	if (result != pool.end())
	{
		return result->first;
	}

	int id = pool.size();
	auto value = MAKE_OBJECT_VARIANT(FloatObject(number));
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

int ConstantPool::allocate(Object_ptr value)
{
	int id = pool.size();
	pool.insert({ id, move(value) });

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

// InstructionPrinter

std::wstring InstructionPrinter::stringify_instruction(std::byte opcode, std::byte operand)
{
	int operand_int = to_integer<int>(operand);

	std::wstringstream str_stream;
	str_stream << std::left << setw(OPCODE_WIDTH) << stringify_opcode(opcode) << L" " << operand_int;

	switch ((OpCode)opcode)
	{
	case OpCode::PUSH_CONSTANT:
	case OpCode::STORE_LOCAL:
	case OpCode::STORE_GLOBAL:
	case OpCode::LOAD_LOCAL:
	case OpCode::LOAD_GLOBAL:
	case OpCode::LOAD_BUILTIN:
	{
		wstring comment = stringify_object(constant_pool->get(operand_int));
		str_stream << std::right << setw(OPERAND_WIDTH) << L" (" << comment << L")";

		return str_stream.str();
	}
	case OpCode::MAKE_LIST:
	case OpCode::MAKE_TUPLE:
	case OpCode::MAKE_MAP:
	case OpCode::JUMP:
	case OpCode::JUMP_IF_FALSE:
	case OpCode::POP_JUMP:
	case OpCode::POP_JUMP_IF_FALSE:
	case OpCode::LABEL:
	case OpCode::ITERATE_OVER_LIST:
	case OpCode::ITERATE_OVER_MAP:
	case OpCode::ITERATE_OVER_STRING:
	case OpCode::ITERATE_OVER_IDENTIFIER:
	{
		return str_stream.str();
	}
	default:
	{
		wstring empty = L"";
		return empty;
	}
	}
}

std::wstring InstructionPrinter::stringify_instruction(std::byte opcode, std::byte operand_1, std::byte operand_2)
{
	int operand_1_int = to_integer<int>(operand_1);
	int operand_2_int = to_integer<int>(operand_2);

	std::wstringstream str_stream;
	str_stream << std::left << setw(OPCODE_WIDTH) << stringify_opcode(opcode) << L" " << operand_1_int << L" " << operand_2_int;

	switch ((OpCode)opcode)
	{
	case OpCode::GET_ELEMENT_FROM_LIST:
	case OpCode::SET_ELEMENT_IN_LIST:
	case OpCode::GET_VALUE_FROM_MAP:
	case OpCode::SET_VALUE_FROM_MAP:
	case OpCode::GET_PAIR_FROM_MAP:
	case OpCode::SET_PAIR_FROM_MAP:
	case OpCode::GET_CHAR_FROM_STRING:
	case OpCode::SET_CHAR_FROM_STRING:
	case OpCode::GET_CLASS_PROPERTY:
	case OpCode::SET_CLASS_PROPERTY:
	case OpCode::GET_CLASS_INSTANCE_PROPERTY:
	case OpCode::SET_CLASS_INSTANCE_PROPERTY:
	case OpCode::GET_CLASS_FUNCTION:
	case OpCode::GET_CLASS_GENERATOR:
	{
		wstring variable_name = stringify_object(constant_pool->get(operand_1_int));
		str_stream << std::right << setw(OPERAND_WIDTH) << L" (" << variable_name << L" ," << operand_2_int << L")";

		return str_stream.str();
	}
	case OpCode::CALL_FUNCTION:
	case OpCode::CALL_GENERATOR:
	{
		wstring function_name = stringify_object(constant_pool->get(operand_1_int));
		str_stream << std::right << setw(OPERAND_WIDTH) << L" (" << function_name << L" ," << operand_2_int << L")";

		return str_stream.str();
	}
	default:
	{
		wstring empty = L"";
		return empty;
	}
	}
}

void InstructionPrinter::print(CodeSection_ptr code_section)
{
	int length = code_section->length();
	int index_width = to_string(length).size() + 2;

	for (int index = 0; index < length; index++)
	{
		ByteVector instruction = code_section->instruction_at(index);

		int arity = instruction.size() - 1;

		switch (arity)
		{
		case 0:
		{
			wcout << setw(index_width) << index << " " << stringify_opcode(instruction.at(0)) << std::endl;
			break;
		}
		case 1:
		{
			wcout << setw(index_width) << index << " " << stringify_instruction(instruction.at(0), instruction.at(1)) << std::endl;
			index++;
			break;
		}
		case 2:
		{
			wcout << setw(index_width) << index << " " << stringify_instruction(instruction.at(0), instruction.at(1), instruction.at(2)) << std::endl;
			index += 2;
			break;
		}
		default:
		{
			break;
		}
		}
	}
}