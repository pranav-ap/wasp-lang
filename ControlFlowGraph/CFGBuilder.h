#pragma once

#ifdef CONTROLFLOWGRAPH_EXPORTS
#define CONTROLFLOWGRAPH_API __declspec(dllexport)
#else
#define CONTROLFLOWGRAPH_API __declspec(dllimport)
#endif

#include "Bytecode.h"
#include "BasicBlock.h"
#include "CFG.h"
#include <memory>
#include <map>

class CONTROLFLOWGRAPH_API CFGBuilder
{
	Bytecode_ptr bytecode;
	CFG_ptr cfg;

	BasicBlock_ptr current_node;

	void split_into_basic_blocks();
	void connect_basic_blocks();

	void enter_empty_node();
	void mark_node_as_conditional();
	void mark_node_as_unconditional();

	// Emit

	void emit(std::byte opcode);
	void emit(std::byte opcode, std::byte operand);
	void emit(std::byte opcode, std::byte operand_1, std::byte operand_2);

public:
	CFGBuilder() {};
	CFG_ptr execute(Bytecode_ptr bytecode);
};

using CFGBuilder_ptr = CONTROLFLOWGRAPH_API std::unique_ptr<CFGBuilder>;
