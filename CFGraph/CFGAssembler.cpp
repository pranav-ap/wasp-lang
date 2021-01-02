#pragma once
#include "pch.h"
#include "CFGAssembler.h"
#include "Assertion.h"
#include "OpCode.h"
#include <memory>

using std::map;
using std::pair;

CodeObject_ptr CFGAssembler::assemble(CFG_ptr cfg)
{
	ByteVector instructions;
	map<int, pair<int, int>> block_locations; // id -> (start, end)

	for (auto const& [id, block] : cfg->basic_blocks)
	{
		int start = instructions.size();
		// - end value is inclusive since we perform -1
		// - it is not used currently
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
		const std::byte opcode = instructions.at(index);
		int arity = get_opcode_arity(opcode);

		switch ((OpCode)opcode)
		{
		case OpCode::JUMP:
		case OpCode::POP_JUMP:
		{
			int to_label = to_integer<int>(instructions.at(index + (size_t)1));
			int false_block_id = cfg->label_to_block_id[to_label];
			int false_block_start_index = block_locations[false_block_id].first;

			instructions.at(index + (size_t)1) = static_cast<std::byte>(false_block_start_index);
			break;
		}
		case OpCode::GET_NEXT_OR_JUMP:
		{
			int label = to_integer<int>(instructions.at(index + (size_t)1));
			int block_id = cfg->label_to_block_id[label];
			int block_start_index = block_locations[block_id].first;

			instructions.at(index + (size_t)1) = static_cast<std::byte>(block_start_index);
			break;
		}
		case OpCode::JUMP_IF_FALSE:
		case OpCode::POP_JUMP_IF_FALSE:
		{
			int to_label = to_integer<int>(instructions.at(index + (size_t)1));
			int false_block_id = cfg->label_to_block_id[to_label];						
			int false_block_start_index = block_locations[false_block_id].first;

			instructions.at(index + (size_t)1) = static_cast<std::byte>(false_block_start_index);
			break;
		}
		}

		index += arity;
	}

	CodeObject_ptr refined_code_object = std::make_shared<CodeObject>();
	refined_code_object->set(instructions);
	
	return refined_code_object;
}
