#pragma once
#include "BasicBlock.h"
#include "ObjectStore.h"
#include "CFG.h"
#include <memory>
#include <map>

#ifdef CFGRAPH_EXPORTS
#define CFGRAPH_API __declspec(dllexport)
#else
#define CFGRAPH_API __declspec(dllimport)
#endif

class CFGRAPH_API CFGBuilder
{
	CFG_ptr cfg;
	CodeObject_ptr unrefined_code_object;
	ObjectStore_ptr object_store;
	BasicBlock_ptr current_basic_block;

	int current_block_id;
	std::map<int, int> block_id_to_location;

	void split_into_basic_blocks();
	void connect_basic_blocks();

	void enter_empty_node();
	void enter_empty_node(BlockType type);

	// Mark

	void mark_node_as_conditional();
	void mark_node_as_unconditional();
	void mark_node_as_unconditional_jump();

	// Emit

	void emit(std::byte opcode);
	void emit(std::byte opcode, std::byte operand);
	void emit(std::byte opcode, std::byte operand_1, std::byte operand_2);

public:
	CFGBuilder(ObjectStore_ptr object_store, CodeObject_ptr unrefined_code_object)
		: current_block_id(0),
		object_store(object_store),
		unrefined_code_object(unrefined_code_object),
		cfg(std::make_shared<CFG>(object_store)) {};

	CFG_ptr create();
};

using CFGBuilder_ptr = CFGRAPH_API std::unique_ptr<CFGBuilder>;
