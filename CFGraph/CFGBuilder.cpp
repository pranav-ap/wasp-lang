#pragma once
#include "pch.h"
#include "CFG.h"
#include "BasicBlock.h"
#include "Assertion.h"
#include "CFGBuilder.h"

#include <memory>
#include <map>

#define IS_A_LEADER previous_instruction_was_leader = true
#define IS_A_FOLLOWER previous_instruction_was_leader = false

using std::make_shared;
using std::move;
using std::map;
using std::pair;
using std::byte;
using std::to_integer;

// API

CFG_ptr CFGBuilder::create()
{
	split_into_basic_blocks();
	connect_basic_blocks();

	return cfg;
}

// Operations

void CFGBuilder::split_into_basic_blocks()
{
	bool previous_instruction_was_leader = false;
	int length = unrefined_code_object->instructions.size();

	for (int code_pointer = 0; code_pointer < length; code_pointer++)
	{
		std::byte opcode = unrefined_code_object->instructions.at(code_pointer);
		OpCode op = (OpCode)opcode;

		switch (op)
		{
		case OpCode::START:
		case OpCode::FUNCTION_START:
		{
			IS_A_LEADER;
			enter_empty_node(BlockType::Start); // type will get overwritten when we reach next leader
			emit(opcode);
			cfg->start_block_id = code_pointer;
			enter_empty_node();
			break;
		}
		case OpCode::STOP:
		case OpCode::FUNCTION_STOP:
		{
			IS_A_LEADER;
			current_basic_block->type = BlockType::Stop;
			cfg->end_block_id = code_pointer;
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
		case OpCode::POP_JUMP:
		{
			IS_A_LEADER;
			std::byte operand = unrefined_code_object->instructions.at(++code_pointer);
			emit(opcode, operand);
			mark_node_as_unconditional_jump();
			enter_empty_node();
			break;
		}
		case OpCode::JUMP_IF_FALSE:
		case OpCode::POP_JUMP_IF_FALSE:
		{
			IS_A_LEADER;
			std::byte operand = unrefined_code_object->instructions.at(++code_pointer);
			emit(opcode, operand);
			mark_node_as_conditional();
			enter_empty_node();
			break;
		}
		case OpCode::LABEL:
		{
			std::byte operand = unrefined_code_object->instructions.at(++code_pointer);
			int label = to_integer<int>(operand);

			if (!previous_instruction_was_leader)
			{
				mark_node_as_unconditional();
				enter_empty_node();
			}

			IS_A_LEADER;
			cfg->label_to_block_id[label] = current_block_id;

			break;
		}

		// Non-Leader with arity 0

		case OpCode::NO_OP:
		case OpCode::POP_FROM_STACK:
		case OpCode::UNARY_NEGATIVE:
		case OpCode::UNARY_NOT:
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
		case OpCode::NULLISH_COALESE:
		case OpCode::PUSH_CONSTANT_TRUE:
		case OpCode::PUSH_CONSTANT_FALSE:
		case OpCode::MAKE_ITERABLE:			
		{
			IS_A_FOLLOWER;
			emit(opcode);
			break;
		}

		// Non-Leader with arity 1

		case OpCode::PUSH_CONSTANT:
		case OpCode::STORE_LOCAL:
		case OpCode::LOAD_LOCAL:
		case OpCode::MAKE_LIST:
		case OpCode::MAKE_TUPLE:
		case OpCode::MAKE_MAP:
		case OpCode::GET_NEXT_OR_JUMP:
		{
			IS_A_FOLLOWER;
			std::byte operand = unrefined_code_object->instructions.at(++code_pointer);
			emit(opcode, operand);
			break;
		}

		// Non-Leader with arity 2

		case OpCode::CALL_FUNCTION:
		case OpCode::CALL_GENERATOR:
		{
			IS_A_FOLLOWER;
			std::byte operand_1 = unrefined_code_object->instructions.at(++code_pointer);
			std::byte operand_2 = unrefined_code_object->instructions.at(++code_pointer);
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
	for (auto const& [id, block] : cfg->basic_blocks)
	{
		if (block->type == BlockType::JumpOnFalse)
		{
			int true_successor_id = id + 1;

			int to_label = to_integer<int>(block->code_object->instructions.back());
			int false_successor_id = cfg->label_to_block_id[to_label];

			cfg->adjacency_list[id] = { true_successor_id, false_successor_id };
		}
		else if (block->type == BlockType::JustJump)
		{
			int to_label = to_integer<int>(block->code_object->instructions.back());
			int unique_successor_id = cfg->label_to_block_id[to_label];

			cfg->adjacency_list[id] = { unique_successor_id, -1 };
		}
		else if (block->type == BlockType::ConnectToFollowingBlock)
		{
			int unique_successor_id = id + 1;
			cfg->adjacency_list[id] = { unique_successor_id, -1 };
		}
		else if (block->type == BlockType::Stop)
		{
			cfg->adjacency_list[id] = { -1, -1 };
			break;
		}
	}
}

// Utils

void CFGBuilder::enter_empty_node()
{
	current_block_id = cfg->basic_blocks.size();
	current_basic_block = make_shared<BasicBlock>();
	cfg->basic_blocks[current_block_id] = current_basic_block;
}

void CFGBuilder::enter_empty_node(BlockType type)
{
	current_block_id = cfg->basic_blocks.size();
	current_basic_block = make_shared<BasicBlock>(type);
	cfg->basic_blocks[current_block_id] = current_basic_block;
}

void CFGBuilder::mark_node_as_conditional()
{
	current_basic_block->type = BlockType::JumpOnFalse;
}

void CFGBuilder::mark_node_as_unconditional()
{
	current_basic_block->type = BlockType::ConnectToFollowingBlock;
}

void CFGBuilder::mark_node_as_unconditional_jump()
{
	current_basic_block->type = BlockType::JustJump;
}

void CFGBuilder::emit(std::byte opcode)
{
	ByteVector ins{ opcode };
	current_basic_block->code_object->push(ins);
}

void CFGBuilder::emit(std::byte opcode, std::byte operand)
{
	ByteVector ins{ opcode , operand };
	current_basic_block->code_object->push(ins);
}

void CFGBuilder::emit(std::byte opcode, std::byte operand_1, std::byte operand_2)
{
	ByteVector ins{ opcode , operand_1, operand_2 };
	current_basic_block->code_object->push(ins);
}
