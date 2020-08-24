#pragma once

#include "pch.h"
#include "MemorySystem.h"
#include "Assertion.h"
#include <string>
#include <iostream>
#include <iomanip>

using std::byte;
using std::cout;
using std::wcout;
using std::endl;
using std::wstring;
using std::to_wstring;
using std::to_string;
using std::vector;
using std::setw;

void MemorySystem::emit(OpCode opcode)
{
	vector<byte> instruction;
	instruction.push_back(static_cast<byte>(opcode));

	bytecode->push(instruction);
}

void MemorySystem::emit(OpCode opcode, int operand)
{
	vector<byte> instruction;
	instruction.push_back(static_cast<byte>(opcode));
	instruction.push_back(static_cast<byte>(operand));

	bytecode->push(instruction);
}

void MemorySystem::emit(OpCode opcode, int operand_1, int operand_2)
{
	vector<byte> instruction;
	instruction.push_back(static_cast<byte>(opcode));
	instruction.push_back(static_cast<byte>(operand_1));
	instruction.push_back(static_cast<byte>(operand_2));

	bytecode->push(instruction);
}

// Constant Pool

void MemorySystem::add_to_constant_pool(int id, Object_ptr value)
{
	constant_pool.insert({ id, value });
}

void MemorySystem::bind_name_to_id(int id, std::wstring name)
{
	id_to_name[id] = name;
}

int MemorySystem::find_string_constant(wstring text)
{
	auto result = find_if(
		constant_pool.begin(),
		constant_pool.end(),
		[text](const auto& p) {
			if (holds_alternative<StringObject>(*p.second))
			{
				StringObject* x = get_if<StringObject>(&*p.second);
				return x->value == text;
			}

			return false;
		});

	if (result != constant_pool.end())
	{
		return result->first;
	}

	return -1;
}

int MemorySystem::find_number_constant(int number)
{
	auto result = find_if(
		constant_pool.begin(),
		constant_pool.end(),
		[number](const auto& p) {
			if (holds_alternative<NumberObject>(*p.second))
			{
				NumberObject* x = get_if<NumberObject>(&*p.second);
				return x->value == number;
			}

			return false;
		});

	if (result != constant_pool.end())
	{
		return result->first;
	}

	return -1;
}

// Utils

const Bytecode_ptr MemorySystem::get_bytecode()
{
	return bytecode;
}

std::map<int, std::wstring> MemorySystem::get_id_to_printable_map()
{
	std::map<int, std::wstring> id_to_printable = id_to_name;

	for (auto const& [id, value] : constant_pool)
	{
		auto printable = stringify_object(value);
		id_to_printable[id] = printable;
	}

	return id_to_printable;
}

void MemorySystem::print()
{
	int length = bytecode->instructions.size();
	int width = to_string(length).size() + 2;

	for (int index = 0; index < length; index++)
	{
		ByteVector instruction = instruction_at(index);

		int arity = instruction.size() - 1;

		switch (arity)
		{
		case 0:
		{
			wcout << setw(width) << index << " " << stringify_instruction(instruction.at(0)) << std::endl;
			break;
		}
		case 1:
		{
			wcout << setw(width) << index << " " << stringify_instruction(instruction.at(0), instruction.at(1)) << std::endl;
			index++;
			break;
		}
		case 2:
		{
			wcout << setw(width) << index << " " << stringify_instruction(instruction.at(0), instruction.at(1), instruction.at(2)) << std::endl;
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

wstring MemorySystem::stringify_instruction_at(int index)
{
	ByteVector instruction = instruction_at(index);
	int instruction_length = instruction.size();

	switch (instruction_length)
	{
	case 0:
	{
		return stringify_instruction(instruction.at(0));
	}
	case 1:
	{
		return stringify_instruction(instruction.at(0), instruction.at(1));
	}
	case 2:
	{
		return stringify_instruction(instruction.at(0), instruction.at(1), instruction.at(2));
	}
	default:
	{
		wstring empty = L"";
		return empty;
	}
	}
}

wstring MemorySystem::stringify_instruction(byte opcode)
{
	switch ((OpCode)opcode)
	{
	case OpCode::NO_OP:
	{
		return L"NO_OP";
	}
	case OpCode::START:
	{
		return L"START";
	}
	case OpCode::STOP:
	{
		return L"STOP";
	}
	case OpCode::PUSH_TO_STACK:
	{
		return L"PUSH_TO_STACK";
	}
	case OpCode::POP_FROM_STACK:
	{
		return L"POP_FROM_STACK";
	}
	case OpCode::POP_N_FROM_STACK:
	{
		return L"POP_N_FROM_STACK";
	}
	case OpCode::UNARY_POSITIVE:
	{
		return L"UNARY_POSITIVE";
	}
	case OpCode::UNARY_NEGATIVE:
	{
		return L"UNARY_NEGATIVE";
	}
	case OpCode::UNARY_NOT:
	{
		return L"UNARY_NOT";
	}
	case OpCode::ASSERT:
	{
		return L"ASSERT";
	}
	case OpCode::ADD:
	{
		return L"ADD";
	}
	case OpCode::SUBTRACT:
	{
		return L"SUBTRACT";
	}
	case OpCode::MULTIPLY:
	{
		return L"MULTIPLY";
	}
	case OpCode::DIVISION:
	{
		return L"DIVISION";
	}
	case OpCode::REMINDER:
	{
		return L"REMINDER";
	}
	case OpCode::POWER:
	{
		return L"POWER";
	}
	case OpCode::NOT_EQUAL:
	{
		return L"NOT_EQUAL";
	}
	case OpCode::EQUAL:
	{
		return L"EQUAL";
	}
	case OpCode::LESSER_THAN:
	{
		return L"LESSER_THAN";
	}
	case OpCode::LESSER_THAN_EQUAL:
	{
		return L"LESSER_THAN_EQUAL";
	}
	case OpCode::GREATER_THAN:
	{
		return L"GREATER_THAN";
	}
	case OpCode::GREATER_THAN_EQUAL:
	{
		return L"GREATER_THAN_EQUAL";
	}
	case OpCode::AND:
	{
		return L"AND";
	}
	case OpCode::OR:
	{
		return L"OR";
	}
	case OpCode::RETURN_VOID:
	{
		return L"RETURN_VOID";
	}
	case OpCode::RETURN_VALUE:
	{
		return L"RETURN_VALUE";
	}
	case OpCode::YIELD_VOID:
	{
		return L"YIELD_VOID";
	}
	case OpCode::YIELD_VALUE:
	{
		return L"YIELD_VALUE";
	}
	case OpCode::PUSH_CONSTANT_TRUE:
	{
		return L"PUSH_CONSTANT_TRUE";
	}
	case OpCode::PUSH_CONSTANT_FALSE:
	{
		return L"PUSH_CONSTANT_FALSE";
	}
	default:
	{
		wstring empty = L"";
		return empty;
	}
	}
}

wstring MemorySystem::stringify_instruction(byte opcode, byte operand)
{
	int operand_int = to_integer<int>(operand);
	wstring operand_str = to_wstring(operand_int);

	wstring readable_str = get_name_or_value(operand_int);

	if (readable_str.size() != 0)
	{
		readable_str = L" (" + readable_str + L")";
	}

	switch ((OpCode)opcode)
	{
	case OpCode::PUSH_CONSTANT:
	{
		return L"PUSH_CONSTANT " + operand_str + readable_str;
	}
	case OpCode::STORE_LOCAL:
	{
		return L"STORE_LOCAL " + operand_str + readable_str;
	}
	case OpCode::STORE_GLOBAL:
	{
		return L"STORE_GLOBAL " + operand_str + readable_str;
	}
	case OpCode::LOAD_LOCAL:
	{
		return L"LOAD_LOCAL " + operand_str + readable_str;
	}
	case OpCode::LOAD_GLOBAL:
	{
		return L"LOAD_GLOBAL " + operand_str + readable_str;
	}
	case OpCode::LOAD_BUILTIN:
	{
		return L"LOAD_BUILTIN " + operand_str;
	}
	case OpCode::MAKE_LIST:
	{
		return L"MAKE_LIST " + operand_str;
	}
	case OpCode::MAKE_TUPLE:
	{
		return L"MAKE_TUPLE " + operand_str;
	}
	case OpCode::MAKE_MAP:
	{
		return L"MAKE_MAP " + operand_str;
	}
	case OpCode::JUMP:
	{
		return L"JUMP " + operand_str;
	}
	case OpCode::JUMP_IF_FALSE:
	{
		return L"JUMP_IF_FALSE " + operand_str;
	}
	case OpCode::POP_JUMP:
	{
		return L"POP_JUMP " + operand_str;
	}
	case OpCode::POP_JUMP_IF_FALSE:
	{
		return L"POP_JUMP_IF_FALSE " + operand_str;
	}
	case OpCode::LABEL:
	{
		return L"LABEL " + operand_str;
	}
	case OpCode::ITERATE_OVER_LIST:
	{
		return L"ITERATE_OVER_LIST " + operand_str;
	}
	case OpCode::ITERATE_OVER_MAP:
	{
		return L"ITERATE_OVER_MAP " + operand_str;
	}
	case OpCode::ITERATE_OVER_STRING:
	{
		return L"ITERATE_OVER_STRING " + operand_str;
	}
	case OpCode::ITERATE_OVER_IDENTIFIER:
	{
		return L"ITERATE_OVER_IDENTIFIER " + operand_str;
	}
	default:
	{
		wstring empty = L"";
		return empty;
	}
	}
}

wstring MemorySystem::stringify_instruction(byte opcode, byte operand_1, byte operand_2)
{
	int operand_1_int = to_integer<int>(operand_1);
	wstring operand_1_str = to_wstring(operand_1_int);

	int operand_2_int = to_integer<int>(operand_2);
	wstring operand_2_str = to_wstring(operand_2_int);

	switch ((OpCode)opcode)
	{
	case OpCode::GET_ELEMENT_FROM_LIST:
	{
		return L"GET_ELEMENT_FROM_LIST " + operand_1_str + L" " + operand_2_str;
	}
	case OpCode::SET_ELEMENT_IN_LIST:
	{
		return L"SET_ELEMENT_IN_LIST " + operand_1_str + L" " + operand_2_str;
	}
	case OpCode::GET_VALUE_FROM_MAP:
	{
		return L"GET_VALUE_FROM_MAP " + operand_1_str + L" " + operand_2_str;
	}
	case OpCode::SET_VALUE_FROM_MAP:
	{
		return L"SET_VALUE_FROM_MAP " + operand_1_str + L" " + operand_2_str;
	}
	case OpCode::GET_PAIR_FROM_MAP:
	{
		return L"GET_PAIR_FROM_MAP " + operand_1_str + L" " + operand_2_str;
	}
	case OpCode::SET_PAIR_FROM_MAP:
	{
		return L"SET_PAIR_FROM_MAP " + operand_1_str + L" " + operand_2_str;
	}
	case OpCode::GET_CHAR_FROM_STRING:
	{
		return L"GET_CHAR_FROM_STRING " + operand_1_str + L" " + operand_2_str;
	}
	case OpCode::SET_CHAR_FROM_STRING:
	{
		return L"SET_CHAR_FROM_STRING " + operand_1_str + L" " + operand_2_str;
	}
	case OpCode::CALL:
	{
		return L"CALL " + operand_1_str + L" " + operand_2_str;
	}
	default:
	{
		wstring empty = L"";
		return empty;
	}
	}
}

ByteVector MemorySystem::instruction_at(int index)
{
	byte opcode = bytecode->instructions.at(index);
	ByteVector operands = operands_of(index);

	ByteVector instruction{ opcode };
	instruction.insert(
		end(instruction),
		begin(operands),
		end(operands));

	return instruction;
}

ByteVector MemorySystem::operands_of(int opcode_index)
{
	byte opcode = bytecode->instructions.at(opcode_index);
	int arity = Bytecode::get_opcode_arity(opcode);

	switch (arity)
	{
	case 0:
	{
		return {};
	}
	case 1:
	{
		byte operand = bytecode->instructions.at(++opcode_index);
		return { operand };
	}
	case 2:
	{
		byte operand_1 = bytecode->instructions.at(++opcode_index);
		byte operand_2 = bytecode->instructions.at(++opcode_index);
		return { operand_1, operand_2 };
	}
	default:
	{
		return {};
	}
	}
}

wstring MemorySystem::get_name_or_value(int id)
{
	if (constant_pool.contains(id))
	{
		Object_ptr value = constant_pool.at(id);
		return stringify_object(value);
	}
	else if (id_to_name.contains(id))
	{
		return id_to_name.at(id);
	}

	wstring empty = L"";
	return empty;
}