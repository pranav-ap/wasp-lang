#pragma once
#include "Bytecode.h"
#include "BasicBlock.h"
#include <memory>
#include <vector>

class CFGBuilder
{
	Bytecode_ptr bytecode;

	BasicBlock_ptr start_node;
	BasicBlock_ptr current_node;

	std::vector<BasicBlock_ptr> basic_blocks;

	bool previous_instruction_was_leader;

	void split_into_basic_blocks();

	void enter_empty_node();

	// Emit

	void emit(std::byte opcode);
	void emit(std::byte opcode, std::byte operand);
	void emit(std::byte opcode, std::byte operand_1, std::byte operand_2);

public:
	CFGBuilder() :
		previous_instruction_was_leader(false) {};

	BasicBlock_ptr execute(Bytecode_ptr bytecode);
};

using CFGBuilder_ptr = std::shared_ptr<CFGBuilder>;
