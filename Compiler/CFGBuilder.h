#pragma once

#include "BasicBlock.h"
#include "CFG.h"
#include <memory>
#include <map>

class CFGBuilder
{
	CodeObject_ptr code_object;
	ObjectStore_ptr object_store;

	CFG_ptr cfg;
	BasicBlock_ptr current_node;

	int current_node_id;
	std::map<int, int> label_to_node_id;

	void split_into_basic_blocks();
	void connect_basic_blocks();

	void enter_empty_node();
	void mark_node_as_conditional();
	void mark_node_as_unconditional();
	void mark_node_as_unconditional_jump();

	// Emit

	void emit(std::byte opcode);
	void emit(std::byte opcode, std::byte operand);
	void emit(std::byte opcode, std::byte operand_1, std::byte operand_2);

public:
	CFGBuilder()
		: current_node_id(0) {};

	CFGBuilder(CodeObject_ptr code_object, ObjectStore_ptr object_store) 
		: current_node_id(0), 
		code_object(code_object), 
		object_store(object_store), 
		cfg(std::make_shared<CFG>(object_store)) {};

	CFG_ptr create();
	ByteVector assemble();
};

using CFGBuilder_ptr = std::unique_ptr<CFGBuilder>;
