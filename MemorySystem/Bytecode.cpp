#pragma once

#include "pch.h"
#include "Bytecode.h"

void Bytecode::push(std::vector<std::byte> instruction)
{
	instructions.insert(
		std::end(instructions),
		std::begin(instruction),
		std::end(instruction)
	);
}

void Bytecode::replace(int index, std::byte replacement)
{
	instructions.at(index) = replacement;
}

int Bytecode::get_opcode_arity(std::byte opcode)
{
	return get_opcode_arity((OpCode)opcode);
}

int Bytecode::get_opcode_arity(OpCode opcode)
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