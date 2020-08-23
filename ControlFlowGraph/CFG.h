#pragma once

#ifdef CONTROLFLOWGRAPH_EXPORTS
#define CONTROLFLOWGRAPH_API __declspec(dllexport)
#else
#define CONTROLFLOWGRAPH_API __declspec(dllimport)
#endif

#include "BasicBlock.h"
#include <memory>
#include <map>
#include <vector>
#include <utility>

struct CONTROLFLOWGRAPH_API CFG
{
	BasicBlock_ptr start_node;

	// block id -> (true path, false path)
	// block id -> (-1, only path)
	std::map<int, std::pair<int, int>> adjacency_list;

	std::map<int, BasicBlock_ptr> node_id_to_basic_blocks;
	std::map<int, BasicBlock_ptr> label_to_basic_blocks;

	std::map<int, Object_ptr> constant_pool;

	// Print

	std::map<int, std::wstring> id_to_name;
	void print();

	CFG() = default;

	CFG(std::map<int, Object_ptr> constant_pool,
		std::map<int, std::wstring> id_to_name)
		: constant_pool(constant_pool),
		id_to_name(id_to_name) {};

private:
	void print_instruction(OpCode opcode);
	void print_instruction(OpCode opcode, int operand);
	void print_instruction(OpCode opcode, int operand_1, int operand_2);
};

using CFG_ptr = CONTROLFLOWGRAPH_API std::shared_ptr<CFG>;
