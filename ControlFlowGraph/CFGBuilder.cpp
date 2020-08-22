#pragma once
#include "pch.h"
#include "CFG.h"
#include "BasicBlock.h"
#include "Assertion.h"
#include "CFGBuilder.h"

#include <memory>

using std::make_shared;
using std::to_integer;
using std::move;

void CFGBuilder::split_into_basic_blocks()
{
	int length = bytecode->instructions.size();

	for (int code_pointer = 0; code_pointer < length; code_pointer++)
	{
		std::byte opcode = bytecode->instructions.at(code_pointer);
		OpCode op = (OpCode)opcode;

		switch (op)
		{
		case OpCode::START:
		{
			enter_empty_node();
			start_node = current_node;
			emit(opcode);
			break;
		}
		case OpCode::STOP:
		{
			emit(opcode);
			return;
		}
		case OpCode::RETURN_VOID:
		case OpCode::RETURN_VALUE:
		case OpCode::YIELD_VOID:
		case OpCode::YIELD_VALUE:
		{
			emit(opcode);
			enter_empty_node();
			break;
		}
		case OpCode::JUMP:
		case OpCode::JUMP_IF_FALSE:
		case OpCode::POP_JUMP:
		case OpCode::POP_JUMP_IF_FALSE:
		{
			std::byte operand = bytecode->instructions.at(++code_pointer);
			emit(opcode, operand);
			enter_empty_node();
			break;
		}
		case OpCode::LABEL:
		{
			std::byte operand = bytecode->instructions.at(++code_pointer);
			int label = to_integer<int>(operand);

			enter_empty_node();
			emit(opcode, operand);
			current_node->label = label;

			break;
		}
		default:
		{
		}
		}
	}
}

void CFGBuilder::enter_empty_node()
{
	basic_blocks.push_back(current_node);
	current_node = make_shared<BasicBlock>();
}

void CFGBuilder::emit(std::byte opcode)
{
	Instruction ins{ opcode };
	current_node->push(ins);
}

void CFGBuilder::emit(std::byte opcode, std::byte operand)
{
	Instruction ins{ opcode , operand };
	current_node->push(ins);
}

void CFGBuilder::emit(std::byte opcode, std::byte operand_1, std::byte operand_2)
{
	Instruction ins{ opcode , operand_1, operand_2 };
	current_node->push(ins);
}

BasicBlock_ptr CFGBuilder::execute(Bytecode_ptr bytecode)
{
	bytecode = bytecode;
	previous_instruction_was_leader = false;

	split_into_basic_blocks();

	return start_node;
}