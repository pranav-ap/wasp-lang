#pragma once
#include "pch.h"
#include "OpCode.h"

OBJECTSYSTEM_API int get_opcode_arity(std::byte opcode)
{
	return get_opcode_arity((OpCode)opcode);
}

OBJECTSYSTEM_API int get_opcode_arity(OpCode opcode)
{
	switch (opcode)
	{
	case OpCode::NO_OP:
	case OpCode::START:
	case OpCode::STOP:
	case OpCode::PUSH_TO_STACK:
	case OpCode::POP_FROM_STACK:
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
	case OpCode::ITERATE_OVER:
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
	{
		return 1;
	}

	case OpCode::MAKE_ENUM:
	case OpCode::CALL_FUNCTION:
	case OpCode::CALL_GENERATOR:
	{
		return 2;
	}

	default:
	{
		return -1;
	}
	}
}

OBJECTSYSTEM_API std::wstring stringify_opcode(std::byte opcode)
{
	return stringify_opcode((OpCode)opcode);
}

OBJECTSYSTEM_API std::wstring stringify_opcode(OpCode opcode)
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
	case OpCode::NULLISH_COALESE:
	{
		return L"NULLISH_COALESE";
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
	case OpCode::PUSH_CONSTANT:
	{
		return L"PUSH_CONSTANT";
	}
	case OpCode::STORE_LOCAL:
	{
		return L"STORE_LOCAL";
	}
	case OpCode::STORE_GLOBAL:
	{
		return L"STORE_GLOBAL";
	}
	case OpCode::LOAD_LOCAL:
	{
		return L"LOAD_LOCAL";
	}
	case OpCode::LOAD_GLOBAL:
	{
		return L"LOAD_GLOBAL";
	}
	case OpCode::LOAD_BUILTIN:
	{
		return L"LOAD_BUILTIN";
	}
	case OpCode::MAKE_LIST:
	{
		return L"MAKE_LIST";
	}
	case OpCode::MAKE_TUPLE:
	{
		return L"MAKE_TUPLE";
	}
	case OpCode::MAKE_MAP:
	{
		return L"MAKE_MAP";
	}
	case OpCode::MAKE_ENUM:
	{
		return L"MAKE_ENUM";
	}
	case OpCode::JUMP:
	{
		return L"JUMP";
	}
	case OpCode::JUMP_IF_FALSE:
	{
		return L"JUMP_IF_FALSE";
	}
	case OpCode::POP_JUMP:
	{
		return L"POP_JUMP";
	}
	case OpCode::POP_JUMP_IF_FALSE:
	{
		return L"POP_JUMP_IF_FALSE";
	}
	case OpCode::LABEL:
	{
		return L"LABEL";
	}
	case OpCode::ITERATE_OVER:
	{
		return L"ITERATE_OVER";
	}
	case OpCode::CALL_FUNCTION:
	{
		return L"CALL_FUNCTION";
	}
	case OpCode::CALL_GENERATOR:
	{
		return L"CALL_GENERATOR";
	}
	default:
	{
		std::wstring empty = L"";
		return empty;
	}
	}
}