#pragma once

#ifdef CONTROLFLOWGRAPH_EXPORTS
#define CONTROLFLOWGRAPH_API __declspec(dllexport)
#else
#define CONTROLFLOWGRAPH_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "BasicBlock.h"
#include <memory>
#include <string>
#include <map>

class CONTROLFLOWGRAPH_API CFG
{
	std::wstring stringify_instruction(std::byte opcode);
	std::wstring stringify_instruction(std::byte opcode, std::byte operand);
	std::wstring stringify_instruction(std::byte opcode, std::byte operand_1, std::byte operand_2);

	BasicBlock_ptr current_block;

	ByteVector instruction_at(int index);
	ByteVector operands_of(int opcode_index);

public:
	BasicBlock_ptr start_node;

	std::map<int, std::pair<int, int>> adjacency_list; // id -> (id, id)
	std::map<int, BasicBlock_ptr> node_id_to_basic_blocks;
	std::map<int, int> label_to_node_id;

	std::map<int, std::wstring> id_to_text;

	void print();
};

using CFG_ptr = CONTROLFLOWGRAPH_API std::shared_ptr<CFG>;
