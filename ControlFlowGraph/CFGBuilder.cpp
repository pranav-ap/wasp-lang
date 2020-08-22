#pragma once
#include "pch.h"
#include "CFG.h"
#include "BasicBlock.h"
#include "Assertion.h"
#include "CFGBuilder.h"

#include <memory>

#define IS_A_LEADER previous_instruction_was_leader = true
#define IS_A_FOLLOWER previous_instruction_was_leader = false

using std::make_shared;
using std::move;

CFG_ptr CFGBuilder::execute(Bytecode_ptr bytecode)
{
	this->bytecode = bytecode;
	cfg = make_shared<CFG>(bytecode->constant_pool, bytecode->id_to_name);
	next_node_id = 0;

	split_into_basic_blocks();

	return move(cfg);
}

void CFGBuilder::split_into_basic_blocks()
{
	int length = bytecode->instructions.size();
	bool previous_instruction_was_leader = false;

	for (int code_pointer = 0; code_pointer < length; code_pointer++)
	{
		std::byte opcode = bytecode->instructions.at(code_pointer);
		OpCode op = (OpCode)opcode;

		switch (op)
		{
		case OpCode::START:
		{
			IS_A_LEADER;
			enter_empty_node();
			cfg->start_node = current_node;
			emit(opcode);
			break;
		}
		case OpCode::STOP:
		{
			IS_A_LEADER;
			mark_node_as_unconditional();
			emit(opcode);

			return;
		}
		case OpCode::RETURN_VOID:
		case OpCode::RETURN_VALUE:
		case OpCode::YIELD_VOID:
		case OpCode::YIELD_VALUE:
		{
			IS_A_LEADER;
			emit(opcode);
			mark_node_as_unconditional();
			enter_empty_node();
			break;
		}
		case OpCode::JUMP:
		case OpCode::JUMP_IF_FALSE:
		case OpCode::POP_JUMP:
		case OpCode::POP_JUMP_IF_FALSE:
		{
			IS_A_LEADER;
			std::byte operand = bytecode->instructions.at(++code_pointer);
			emit(opcode, operand);
			mark_node_as_conditional();
			enter_empty_node();
			break;
		}
		case OpCode::LABEL:
		{
			std::byte operand = bytecode->instructions.at(++code_pointer);
			int label = to_integer<int>(operand);

			if (!previous_instruction_was_leader)
			{
				mark_node_as_unconditional();
				enter_empty_node();
			}

			emit(opcode, operand);
			cfg->label_to_node_id[label] = next_node_id - 1;

			IS_A_LEADER;

			break;
		}

		// Non-Leader with arity 1

		case OpCode::NO_OP:
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
		case OpCode::PUSH_CONSTANT_TRUE:
		case OpCode::PUSH_CONSTANT_FALSE:
		{
			IS_A_FOLLOWER;
			emit(opcode);
			break;
		}

		// Non-Leader with arity 1

		case OpCode::PUSH_CONSTANT:
		case OpCode::STORE_LOCAL:
		case OpCode::STORE_GLOBAL:
		case OpCode::LOAD_LOCAL:
		case OpCode::LOAD_GLOBAL:
		case OpCode::LOAD_BUILTIN:
		case OpCode::MAKE_LIST:
		case OpCode::MAKE_TUPLE:
		case OpCode::MAKE_MAP:
		case OpCode::ITERATE_OVER_LIST:
		case OpCode::ITERATE_OVER_MAP:
		case OpCode::ITERATE_OVER_STRING:
		case OpCode::ITERATE_OVER_IDENTIFIER:
		{
			IS_A_FOLLOWER;
			std::byte operand = bytecode->instructions.at(++code_pointer);
			emit(opcode, operand);
			break;
		}

		// Non-Leader with arity 2

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
			IS_A_FOLLOWER;
			std::byte operand_1 = bytecode->instructions.at(++code_pointer);
			std::byte operand_2 = bytecode->instructions.at(++code_pointer);
			emit(opcode, operand_1, operand_2);
			break;
		}
		default:
		{
			IS_A_FOLLOWER;
			break;
		}
		}
	}
}

void CFGBuilder::connect_basic_blocks()
{
}

// Utils

void CFGBuilder::enter_empty_node()
{
	current_node = make_shared<BasicBlock>();
	cfg->basic_blocks[next_node_id++] = current_node;
}

void CFGBuilder::mark_node_as_conditional()
{
	current_node->type = BlockType::Conditional;
}

void CFGBuilder::mark_node_as_unconditional()
{
	current_node->type = BlockType::Unconditional;
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