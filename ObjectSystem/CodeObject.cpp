#pragma once
#include "pch.h"
#include "CodeObject.h"
#include "Assertion.h"
#include <string>
#include <algorithm>

using std::byte;
using std::vector;
using std::make_shared;

int CodeObject::length()
{
	return instructions.size();
}

void CodeObject::push(ByteVector instruction)
{
	instructions.insert(
		std::end(instructions),
		std::begin(instruction),
		std::end(instruction)
	);	
}

void CodeObject::replace(int index, std::byte replacement)
{
	instructions.at(index) = replacement;
}

void CodeObject::set(ByteVector instructions)
{
	this->instructions = instructions;
}

void CodeObject::emit(OpCode opcode)
{
	ByteVector instruction = { static_cast<byte>(opcode) };
	push(instruction);
}

void CodeObject::emit(OpCode opcode, int operand)
{
	ByteVector instruction = { static_cast<byte>(opcode), static_cast<byte>(operand) };
	push(instruction);
}

void CodeObject::emit(OpCode opcode, int operand_1, int operand_2)
{
	ByteVector instruction = { static_cast<byte>(opcode), static_cast<byte>(operand_1), static_cast<byte>(operand_2) };
	push(instruction);
}

ByteVector CodeObject::instruction_at(int index)
{
	byte opcode = instructions.at(index);
	ByteVector operands = operands_of_opcode_at(index);

	ByteVector instruction{ opcode };
	instruction.insert(
		end(instruction),
		begin(operands),
		end(operands));

	return instruction;
}

ByteVector CodeObject::operands_of_opcode_at(int opcode_index)
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
