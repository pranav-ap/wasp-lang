#include "pch.h"
#include "Bytecode.h"
#include "ObjectSystem.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <variant>

#define GET_OPCODE (OpCode) std::to_integer<int>(instructions.at(index))
#define INC_INDEX index++
#define WIDTH std::setw(number_of_digits + 4)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::cout;
using std::wstring;
using std::to_wstring;

//wstring object_to_wstring(Object_ptr object)
//{
//	return std::visit(overloaded{
//		[&](NumberObject const& obj) { return to_wstring(obj.value); },
//		[&](StringObject const& obj) { return obj.value; },
//		[&](BooleanObject const& obj) { return to_wstring(obj.value); },
//
//		//[&](ListObject const& obj) { return L"List - size " + obj.values.size(); },
//		//[&](TupleObject const& obj) { return L"Tuple - size " + obj.values.size(); },
//
//		//[&](EnumMemberObject const& obj)
//		//{
//		//	wstring member_name;
//
//		//	for (auto const name : obj.member_names)
//		//	{
//		//		member_name += L"::" + name;
//		//	}
//
//		//	return obj.enum_name + L"::" + member_name;
//		//},
//		//[&](MapObject const& obj) { return L"Map - size " + obj.pairs.size(); },
//		//[&](VariantObject const& obj) { return L"VariantObject"; },
//		//[&](ReturnObject const& obj) { return L"ReturnObject"; },
//		//[&](ErrorObject const& obj) { return L"ErrorObject"; },
//		//[&](BuiltInsObject const& obj) { return L"BuiltInsObject"; },
//		//[&](NoneObject const& obj) { return L"NoneObject"; },
//
//		//[&](FunctionObject const& obj) { return L"FunctionObject"; },
//		//[&](GeneratorObject const& obj) { return L"GeneratorObject"; },
//
//		[](auto) { return L" "; }
//		}, *object);
//}

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
			std::cout << WIDTH << index << " ";
			print_instruction(opcode);
			cout << std::endl;

			break;
		}
		case 1:
		{
			INC_INDEX;
			OpCode operand = GET_OPCODE;

			std::cout << WIDTH << index << " ";
			print_instruction(opcode, static_cast<int>(operand));
			cout << std::endl;

			break;
		}
		case 2:
		{
			INC_INDEX;
			OpCode operand_1 = GET_OPCODE;
			INC_INDEX;
			OpCode operand_2 = GET_OPCODE;

			std::cout << WIDTH << index << " ";
			print_instruction(opcode, static_cast<int>(operand_1), static_cast<int>(operand_2));
			cout << std::endl;

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
		cout << "NO_OP";
		break;
	}
	case OpCode::STOP:
	{
		cout << "STOP";
		break;
	}
	case OpCode::PUSH_TO_STACK:
	{
		cout << "PUSH_TO_STACK";
		break;
	}
	case OpCode::POP_FROM_STACK:
	{
		cout << "POP_FROM_STACK";
		break;
	}
	case OpCode::POP_N_FROM_STACK:
	{
		cout << "POP_N_FROM_STACK";
		break;
	}
	case OpCode::UNARY_POSITIVE:
	{
		cout << "UNARY_POSITIVE";
		break;
	}
	case OpCode::UNARY_NEGATIVE:
	{
		cout << "UNARY_NEGATIVE";
		break;
	}
	case OpCode::UNARY_NOT:
	{
		cout << "UNARY_NOT";
		break;
	}
	case OpCode::ASSERT:
	{
		cout << "ASSERT";
		break;
	}
	case OpCode::ADD:
	{
		cout << "ADD";
		break;
	}
	case OpCode::SUBTRACT:
	{
		cout << "SUBTRACT";
		break;
	}
	case OpCode::MULTIPLY:
	{
		cout << "MULTIPLY";
		break;
	}
	case OpCode::DIVISION:
	{
		cout << "DIVISION";
		break;
	}
	case OpCode::REMINDER:
	{
		cout << "REMINDER";
		break;
	}
	case OpCode::POWER:
	{
		cout << "POWER";
		break;
	}
	case OpCode::NOT_EQUAL:
	{
		cout << "NOT_EQUAL";
		break;
	}
	case OpCode::EQUAL:
	{
		cout << "EQUAL";
		break;
	}
	case OpCode::LESSER_THAN:
	{
		cout << "LESSER_THAN";
		break;
	}
	case OpCode::LESSER_THAN_EQUAL:
	{
		cout << "LESSER_THAN_EQUAL";
		break;
	}
	case OpCode::GREATER_THAN:
	{
		cout << "GREATER_THAN";
		break;
	}
	case OpCode::GREATER_THAN_EQUAL:
	{
		cout << "GREATER_THAN_EQUAL";
		break;
	}
	case OpCode::AND:
	{
		cout << "AND";
		break;
	}
	case OpCode::OR:
	{
		cout << "OR";
		break;
	}
	case OpCode::RETURN_VOID:
	{
		cout << "RETURN_VOID";
		break;
	}
	case OpCode::RETURN_VALUE:
	{
		cout << "RETURN_VALUE";
		break;
	}
	case OpCode::YIELD_VOID:
	{
		cout << "YIELD_VOID";
		break;
	}
	case OpCode::YIELD_VALUE:
	{
		cout << "YIELD_VALUE";
		break;
	}
	case OpCode::PUSH_CONSTANT_TRUE:
	{
		cout << "PUSH_CONSTANT_TRUE";
		break;
	}
	case OpCode::PUSH_CONSTANT_FALSE:
	{
		cout << "PUSH_CONSTANT_FALSE";
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
		cout << "PUSH_CONSTANT " << operand;

		break;
	}
	case OpCode::STORE_LOCAL:
	{
		cout << "STORE_LOCAL " << operand;

		break;
	}
	case OpCode::STORE_GLOBAL:
	{
		cout << "STORE_GLOBAL " << operand;
		break;
	}
	case OpCode::LOAD_LOCAL:
	{
		cout << "LOAD_LOCAL " << operand;
		break;
	}
	case OpCode::LOAD_GLOBAL:
	{
		cout << "LOAD_GLOBAL " << operand;
		break;
	}
	case OpCode::LOAD_BUILTIN:
	{
		cout << "LOAD_BUILTIN " << operand;
		break;
	}
	case OpCode::MAKE_LIST:
	{
		cout << "MAKE_LIST " << operand;
		break;
	}
	case OpCode::MAKE_TUPLE:
	{
		cout << "MAKE_TUPLE " << operand;
		break;
	}
	case OpCode::MAKE_MAP:
	{
		cout << "MAKE_MAP " << operand;
		break;
	}

	case OpCode::JUMP:
	{
		cout << "JUMP " << operand;
		break;
	}
	case OpCode::JUMP_IF_FALSE:
	{
		cout << "JUMP_IF_FALSE " << operand;
		break;
	}
	case OpCode::POP_JUMP:
	{
		cout << "POP_JUMP " << operand;
		break;
	}
	case OpCode::POP_JUMP_IF_FALSE:
	{
		cout << "POP_JUMP_IF_FALSE " << operand;
		break;
	}
	case OpCode::LABEL:
	{
		cout << "LABEL " << operand;
		break;
	}

	case OpCode::ITERATE_OVER_LIST:
	{
		cout << "ITERATE_OVER_LIST " << operand;
		break;
	}
	case OpCode::ITERATE_OVER_MAP:
	{
		cout << "ITERATE_OVER_MAP " << operand;
		break;
	}
	case OpCode::ITERATE_OVER_STRING:
	{
		cout << "ITERATE_OVER_STRING " << operand;
		break;
	}
	case OpCode::ITERATE_OVER_IDENTIFIER:
	{
		cout << "ITERATE_OVER_IDENTIFIER " << operand;
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
		cout << "GET_ELEMENT_FROM_LIST " << operand_1 << " " << operand_2;
		break;
	}
	case OpCode::SET_ELEMENT_IN_LIST:
	{
		cout << "SET_ELEMENT_IN_LIST " << operand_1 << " " << operand_2;
		break;
	}
	case OpCode::GET_VALUE_FROM_MAP:
	{
		cout << "GET_VALUE_FROM_MAP " << operand_1 << " " << operand_2;
		break;
	}
	case OpCode::SET_VALUE_FROM_MAP:
	{
		cout << "SET_VALUE_FROM_MAP " << operand_1 << " " << operand_2;
		break;
	}
	case OpCode::GET_PAIR_FROM_MAP:
	{
		cout << "GET_PAIR_FROM_MAP " << operand_1 << " " << operand_2;
		break;
	}
	case OpCode::SET_PAIR_FROM_MAP:
	{
		cout << "SET_PAIR_FROM_MAP " << operand_1 << " " << operand_2;
		break;
	}
	case OpCode::GET_CHAR_FROM_STRING:
	{
		cout << "GET_CHAR_FROM_STRING " << operand_1 << " " << operand_2;
		break;
	}
	case OpCode::SET_CHAR_FROM_STRING:
	{
		cout << "SET_CHAR_FROM_STRING " << operand_1 << " " << operand_2;
		break;
	}
	case OpCode::CALL:
	{
		cout << "CALL " << operand_1 << " " << operand_2;
		break;
	}
	default:
	{
		break;
	}
	}
}

int get_opcode_arity(OpCode opcode)
{
	switch (opcode)
	{
	case OpCode::NO_OP:
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