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

ByteVector CFGBuilder::assemble()
{
	ByteVector instructions;
	map<int, pair<int, int>> block_locations; // id -> (start, end)

	for (auto const& [id, block] : cfg->basic_blocks)
	{
		int start = instructions.size();
		int end = start + block->code_object->instructions.size() - 1;

		block_locations[id] = { start, end };

		instructions.insert(
			std::end(instructions),
			std::begin(block->code_object->instructions),
			std::end(block->code_object->instructions)
		);
	}

	for (int index = 0; index < instructions.size(); index++)
	{
		const byte opcode = instructions.at(index);
		int arity = get_opcode_arity(opcode);

		switch ((OpCode)opcode)
		{
		case OpCode::JUMP:
		case OpCode::JUMP_IF_FALSE:
		case OpCode::POP_JUMP:
		case OpCode::POP_JUMP_IF_FALSE:
		{
			int label = to_integer<int>(instructions.at(index + (size_t)1));
			int block_id = label_to_node_id[label];

			int block_start_index = block_locations[block_id].first;
			instructions.at(index + (size_t)1) = static_cast<byte>(block_start_index);
			break;
		}

		default:
		{
			break;
		}
		}

		index += arity;
	}

	return instructions;
}

// Operations

void CFGBuilder::split_into_basic_blocks()
{
	int length = instructions.size();
	bool previous_instruction_was_leader = false;

	for (int code_pointer = 0; code_pointer < length; code_pointer++)
	{
		std::byte opcode = instructions.at(code_pointer);
		OpCode op = (OpCode)opcode;

		switch (op)
		{
		case OpCode::START:
		{
			IS_A_LEADER;
			enter_empty_node();
			current_node->type = BlockType::Start;
			cfg->start_node_id = current_node_id;
			emit(opcode);
			break;
		}
		case OpCode::STOP:
		{
			IS_A_LEADER;
			current_node->type = BlockType::Stop;
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
			std::byte operand = instructions.at(++code_pointer);
			emit(opcode, operand);
			mark_node_as_unconditional_jump();
			enter_empty_node();
			break;
		}
		case OpCode::JUMP_IF_FALSE:
		case OpCode::POP_JUMP_IF_FALSE:
		{
			IS_A_LEADER;
			std::byte operand = instructions.at(++code_pointer);
			emit(opcode, operand);
			mark_node_as_conditional();
			enter_empty_node();
			break;
		}
		case OpCode::LABEL:
		{
			std::byte operand = instructions.at(++code_pointer);
			int label = to_integer<int>(operand);

			if (!previous_instruction_was_leader)
			{
				mark_node_as_unconditional();
				enter_empty_node();
			}

			current_node->label = label;
			label_to_node_id[label] = current_node_id;
			IS_A_LEADER;

			break;
		}

		// Non-Leader with arity 1

		case OpCode::NO_OP:
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
		case OpCode::NULLISH_COALESE:
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
		case OpCode::MAKE_ENUM:
		{
			IS_A_FOLLOWER;
			std::byte operand = instructions.at(++code_pointer);
			emit(opcode, operand);
			break;
		}

		// Non-Leader with arity 2

		case OpCode::CALL_FUNCTION:
		case OpCode::CALL_GENERATOR:
		{
			IS_A_FOLLOWER;
			std::byte operand_1 = instructions.at(++code_pointer);
			std::byte operand_2 = instructions.at(++code_pointer);
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
		if (block->type == BlockType::ConditionalJump)
		{
			int to_label = to_integer<int>(block->code_object->instructions.back());

			int true_successor_id = id + 1;
			int false_successor_id = label_to_node_id[to_label];

			cfg->adjacency_list[id] = { true_successor_id, false_successor_id };
		}
		else if (block->type == BlockType::UnconditionalJump)
		{
			int to_label = to_integer<int>(block->code_object->instructions.back());
			int unique_successor_id = label_to_node_id[to_label];

			cfg->adjacency_list[id] = { unique_successor_id, -1 };
		}
		else if (block->type == BlockType::Unconditional)
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
	current_node_id = cfg->basic_blocks.size();
	current_node = make_shared<BasicBlock>();
	cfg->basic_blocks[current_node_id] = current_node;
}

void CFGBuilder::mark_node_as_conditional()
{
	current_node->type = BlockType::ConditionalJump;
}

void CFGBuilder::mark_node_as_unconditional()
{
	current_node->type = BlockType::Unconditional;
}

void CFGBuilder::mark_node_as_unconditional_jump()
{
	current_node->type = BlockType::UnconditionalJump;
}

void CFGBuilder::emit(std::byte opcode)
{
	ByteVector ins{ opcode };
	current_node->push(ins);
}

void CFGBuilder::emit(std::byte opcode, std::byte operand)
{
	ByteVector ins{ opcode , operand };
	current_node->push(ins);
}

void CFGBuilder::emit(std::byte opcode, std::byte operand_1, std::byte operand_2)
{
	ByteVector ins{ opcode , operand_1, operand_2 };
	current_node->push(ins);
}