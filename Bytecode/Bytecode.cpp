#include "pch.h"
#include "Bytecode.h"
#include "ObjectSystem.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <variant>

#define GET_OPCODE (OpCode) std::to_integer<int>(instructions.at(index))
#define INC_INDEX index++

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::wcout;
using std::wstring;
using std::to_wstring;

void Bytecode::print()
{
	int length = instructions.size();
	int number_of_digits = std::to_string(length).size();

	for (int index = 0; index < length; index++)
	{
		OpCode opcode = GET_OPCODE;

		int arity = get_opcode_arity(opcode);

		switch (arity)
		{
		case 0:
		{
			wcout << index << " ";
			print_instruction(opcode);
			wcout << std::endl;

			break;
		}
		case 1:
		{
			INC_INDEX;
			OpCode operand = GET_OPCODE;

			wcout << index << " ";
			print_instruction(opcode, static_cast<int>(operand));
			wcout << std::endl;

			break;
		}
		case 2:
		{
			INC_INDEX;
			OpCode operand_1 = GET_OPCODE;
			INC_INDEX;
			OpCode operand_2 = GET_OPCODE;

			wcout << index << " ";
			print_instruction(opcode, static_cast<int>(operand_1), static_cast<int>(operand_2));
			wcout << std::endl;

			break;
		}
		default:
		{
			break;
		}
		}
	}
}

void Bytecode::print_instruction(OpCode opcode)
{
	switch ((OpCode)opcode)
	{
	case OpCode::NO_OP:
	{
		wcout << L"NO_OP";
		break;
	}
	case OpCode::START:
	{
		wcout << L"START";
		break;
	}
	case OpCode::STOP:
	{
		wcout << L"STOP";
		break;
	}
	case OpCode::PUSH_TO_STACK:
	{
		wcout << L"PUSH_TO_STACK";
		break;
	}
	case OpCode::POP_FROM_STACK:
	{
		wcout << L"POP_FROM_STACK";
		break;
	}
	case OpCode::POP_N_FROM_STACK:
	{
		wcout << L"POP_N_FROM_STACK";
		break;
	}
	case OpCode::UNARY_POSITIVE:
	{
		wcout << L"UNARY_POSITIVE";
		break;
	}
	case OpCode::UNARY_NEGATIVE:
	{
		wcout << L"UNARY_NEGATIVE";
		break;
	}
	case OpCode::UNARY_NOT:
	{
		wcout << L"UNARY_NOT";
		break;
	}
	case OpCode::ASSERT:
	{
		wcout << L"ASSERT";
		break;
	}
	case OpCode::ADD:
	{
		wcout << L"ADD";
		break;
	}
	case OpCode::SUBTRACT:
	{
		wcout << L"SUBTRACT";
		break;
	}
	case OpCode::MULTIPLY:
	{
		wcout << L"MULTIPLY";
		break;
	}
	case OpCode::DIVISION:
	{
		wcout << L"DIVISION";
		break;
	}
	case OpCode::REMINDER:
	{
		wcout << L"REMINDER";
		break;
	}
	case OpCode::POWER:
	{
		wcout << L"POWER";
		break;
	}
	case OpCode::NOT_EQUAL:
	{
		wcout << L"NOT_EQUAL";
		break;
	}
	case OpCode::EQUAL:
	{
		wcout << L"EQUAL";
		break;
	}
	case OpCode::LESSER_THAN:
	{
		wcout << L"LESSER_THAN";
		break;
	}
	case OpCode::LESSER_THAN_EQUAL:
	{
		wcout << L"LESSER_THAN_EQUAL";
		break;
	}
	case OpCode::GREATER_THAN:
	{
		wcout << L"GREATER_THAN";
		break;
	}
	case OpCode::GREATER_THAN_EQUAL:
	{
		wcout << L"GREATER_THAN_EQUAL";
		break;
	}
	case OpCode::AND:
	{
		wcout << L"AND";
		break;
	}
	case OpCode::OR:
	{
		wcout << L"OR";
		break;
	}
	case OpCode::RETURN_VOID:
	{
		wcout << L"RETURN_VOID";
		break;
	}
	case OpCode::RETURN_VALUE:
	{
		wcout << L"RETURN_VALUE";
		break;
	}
	case OpCode::YIELD_VOID:
	{
		wcout << L"YIELD_VOID";
		break;
	}
	case OpCode::YIELD_VALUE:
	{
		wcout << L"YIELD_VALUE";
		break;
	}
	case OpCode::PUSH_CONSTANT_TRUE:
	{
		wcout << L"PUSH_CONSTANT_TRUE";
		break;
	}
	case OpCode::PUSH_CONSTANT_FALSE:
	{
		wcout << L"PUSH_CONSTANT_FALSE";
		break;
	}
	default:
	{
		break;
	}
	}
}

void Bytecode::print_instruction(OpCode opcode, int operand)
{
	switch ((OpCode)opcode)
	{
	case OpCode::PUSH_CONSTANT:
	{
		wcout << L"PUSH_CONSTANT " << operand << L" (" << id_to_name.at(operand) << L")";

		break;
	}
	case OpCode::STORE_LOCAL:
	{
		wcout << L"STORE_LOCAL " << operand << L" (" << id_to_name.at(operand) << L")";

		break;
	}
	case OpCode::STORE_GLOBAL:
	{
		wcout << L"STORE_GLOBAL " << operand << L" (" << id_to_name.at(operand) << L")";
		break;
	}
	case OpCode::LOAD_LOCAL:
	{
		wcout << L"LOAD_LOCAL " << operand << L" (" << id_to_name.at(operand) << L")";
		break;
	}
	case OpCode::LOAD_GLOBAL:
	{
		wcout << L"LOAD_GLOBAL " << operand << L" (" << id_to_name.at(operand) << L")";
		break;
	}
	case OpCode::LOAD_BUILTIN:
	{
		wcout << L"LOAD_BUILTIN " << operand;
		break;
	}
	case OpCode::MAKE_LIST:
	{
		wcout << L"MAKE_LIST " << operand;
		break;
	}
	case OpCode::MAKE_TUPLE:
	{
		wcout << L"MAKE_TUPLE " << operand;
		break;
	}
	case OpCode::MAKE_MAP:
	{
		wcout << L"MAKE_MAP " << operand;
		break;
	}
	case OpCode::JUMP:
	{
		wcout << L"JUMP " << operand;
		break;
	}
	case OpCode::JUMP_IF_FALSE:
	{
		wcout << L"JUMP_IF_FALSE " << operand;
		break;
	}
	case OpCode::POP_JUMP:
	{
		wcout << L"POP_JUMP " << operand;
		break;
	}
	case OpCode::POP_JUMP_IF_FALSE:
	{
		wcout << L"POP_JUMP_IF_FALSE " << operand;
		break;
	}
	case OpCode::LABEL:
	{
		wcout << L"LABEL " << operand;
		break;
	}

	case OpCode::ITERATE_OVER_LIST:
	{
		wcout << L"ITERATE_OVER_LIST " << operand;
		break;
	}
	case OpCode::ITERATE_OVER_MAP:
	{
		wcout << L"ITERATE_OVER_MAP " << operand;
		break;
	}
	case OpCode::ITERATE_OVER_STRING:
	{
		wcout << L"ITERATE_OVER_STRING " << operand;
		break;
	}
	case OpCode::ITERATE_OVER_IDENTIFIER:
	{
		wcout << L"ITERATE_OVER_IDENTIFIER " << operand;
		break;
	}
	default:
	{
		break;
	}
	}
}

void Bytecode::print_instruction(OpCode opcode, int operand_1, int operand_2)
{
	switch ((OpCode)opcode)
	{
	case OpCode::GET_ELEMENT_FROM_LIST:
	{
		wcout << L"GET_ELEMENT_FROM_LIST " << operand_1 << L" " << operand_2;
		break;
	}
	case OpCode::SET_ELEMENT_IN_LIST:
	{
		wcout << L"SET_ELEMENT_IN_LIST " << operand_1 << L" " << operand_2;
		break;
	}
	case OpCode::GET_VALUE_FROM_MAP:
	{
		wcout << L"GET_VALUE_FROM_MAP " << operand_1 << L" " << operand_2;
		break;
	}
	case OpCode::SET_VALUE_FROM_MAP:
	{
		wcout << L"SET_VALUE_FROM_MAP " << operand_1 << L" " << operand_2;
		break;
	}
	case OpCode::GET_PAIR_FROM_MAP:
	{
		wcout << L"GET_PAIR_FROM_MAP " << operand_1 << L" " << operand_2;
		break;
	}
	case OpCode::SET_PAIR_FROM_MAP:
	{
		wcout << L"SET_PAIR_FROM_MAP " << operand_1 << L" " << operand_2;
		break;
	}
	case OpCode::GET_CHAR_FROM_STRING:
	{
		wcout << L"GET_CHAR_FROM_STRING " << operand_1 << L" " << operand_2;
		break;
	}
	case OpCode::SET_CHAR_FROM_STRING:
	{
		wcout << L"SET_CHAR_FROM_STRING " << operand_1 << L" " << operand_2;
		break;
	}
	case OpCode::CALL:
	{
		wcout << L"CALL " << operand_1 << L" " << operand_2;
		break;
	}
	default:
	{
		break;
	}
	}
}

int get_opcode_arity(std::byte opcode)
{
	return get_opcode_arity((OpCode)opcode);
}

int get_opcode_arity(OpCode opcode)
{
	switch (opcode)
	{
	case OpCode::NO_OP:
	case OpCode::START:
	case OpCode::STOP:
	case OpCode::PUSH_TO_STACK:
	case OpCode::POP_FROM_STACK:
	case OpCode::POP_N_FROM_STACK:
	case OpCode::UNARY_POSITIVE:
	case OpCode::UNARY_NEGATIVE:
	case OpCode::UNARY_NOT:
	case OpCode::ASSERT:
	case OpCode::ADD:
	case OpCode::SUBTRACT:
	case OpCode::MULTIPLY:
	case OpCode::DIVISION:
	case OpCode::REMINDER:
	case OpCode::POWER:
	case OpCode::NOT_EQUAL:
	case OpCode::EQUAL:
	case OpCode::LESSER_THAN:
	case OpCode::LESSER_THAN_EQUAL:
	case OpCode::GREATER_THAN:
	case OpCode::GREATER_THAN_EQUAL:
	case OpCode::AND:
	case OpCode::OR:
	case OpCode::RETURN_VOID:
	case OpCode::RETURN_VALUE:
	case OpCode::YIELD_VOID:
	case OpCode::YIELD_VALUE:
	case OpCode::PUSH_CONSTANT_TRUE:
	case OpCode::PUSH_CONSTANT_FALSE:
	{
		return 0;
	}

	case OpCode::PUSH_CONSTANT:
	case OpCode::STORE_LOCAL:
	case OpCode::STORE_GLOBAL:
	case OpCode::LOAD_LOCAL:
	case OpCode::LOAD_GLOBAL:
	case OpCode::LOAD_BUILTIN:
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
		return 1;
	}

	case OpCode::GET_ELEMENT_FROM_LIST:
	case OpCode::SET_ELEMENT_IN_LIST:
	case OpCode::GET_VALUE_FROM_MAP:
	case OpCode::SET_VALUE_FROM_MAP:
	case OpCode::GET_PAIR_FROM_MAP:
	case OpCode::SET_PAIR_FROM_MAP:
	case OpCode::GET_CHAR_FROM_STRING:
	case OpCode::SET_CHAR_FROM_STRING:
	case OpCode::CALL:
	{
		return 2;
	}

	default:
	{
		return -1;
	}
	}
}