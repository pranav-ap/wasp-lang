#include "pch.h"
#include "CodeSection.h"
#include "Assertion.h"
#include <string>
#include <algorithm>

using std::byte;
using std::vector;
using std::make_shared;

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
