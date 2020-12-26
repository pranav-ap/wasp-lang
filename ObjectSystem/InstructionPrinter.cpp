#pragma once
#include "pch.h"
#include "InstructionPrinter.h"
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
#define OPERAND_WIDTH 12
#define OPERAND_WIDTH_2 14

using std::byte;
using std::vector;
using std::move;
using std::wcout;
using std::endl;
using std::setw;
using std::wstring;
using std::to_wstring;
using std::to_string;
using std::make_shared;

std::wstring InstructionPrinter::stringify_instruction(std::byte opcode, std::byte operand)
{
	int operand_int = to_integer<int>(operand);

	std::wstringstream str_stream;
	str_stream << std::left << setw(OPCODE_WIDTH) << stringify_opcode(opcode) << L" " << operand_int;

	switch ((OpCode)opcode)
	{
	case OpCode::PUSH_CONSTANT:
	case OpCode::MAKE_LIST:
	case OpCode::MAKE_TUPLE:
	case OpCode::MAKE_MAP:
	case OpCode::MAKE_SET:
	case OpCode::JUMP:
	case OpCode::JUMP_IF_FALSE:
	case OpCode::POP_JUMP:
	case OpCode::POP_JUMP_IF_FALSE:
	case OpCode::LABEL:
	case OpCode::ITERATE_OVER:
	{
		wstring comment = stringify_object(object_store->get(operand_int));
		str_stream << std::right << setw(OPERAND_WIDTH_2) << L" (" << comment << L")";

		return str_stream.str();
	}
	case OpCode::STORE_LOCAL:
	case OpCode::STORE_GLOBAL:
	case OpCode::LOAD_LOCAL:
	case OpCode::LOAD_GLOBAL:
	case OpCode::LOAD_BUILTIN:
	{
		wstring comment = object_store->name_map.at(operand_int);
		str_stream << std::right << setw(OPERAND_WIDTH_2) << L" (" << comment << L")";

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
	case OpCode::CALL_FUNCTION:
	case OpCode::CALL_GENERATOR:
	{
		wstring name = object_store->name_map.at(operand_1_int);
		str_stream << std::right << setw(OPERAND_WIDTH) << L" (" << name << L" , " << operand_2_int << L")";

		return str_stream.str();
	}
	default:
	{
		wstring empty = L"";
		return empty;
	}
	}
}

void InstructionPrinter::print(CodeObject_ptr code_object)
{
	int length = code_object->length();
	int index_width = to_string(length).size() + 2;

	for (int index = 0; index < length; index++)
	{
		ByteVector instruction = code_object->instruction_at(index);

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
