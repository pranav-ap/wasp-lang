#pragma once

#ifdef CONTROLFLOWGRAPH_EXPORTS
#define CONTROLFLOWGRAPH_API __declspec(dllexport)
#else
#define CONTROLFLOWGRAPH_API __declspec(dllimport)
#endif

#include "Bytecode.h"
#include "BasicBlock.h"
#include "CFG.h"
#include "MemorySystem.h"
#include <memory>
#include <map>

class CONTROLFLOWGRAPH_API CFGBuilder
{
	MemorySystem_ptr memory;
	CFG_ptr cfg;

	BasicBlock_ptr current_node;
	int current_node_id;

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
	CFGBuilder() : current_node_id(0), memory(std::make_shared<MemorySystem>()) {};
	CFGBuilder(MemorySystem_ptr memory) : current_node_id(0), memory(memory) {};
	CFG_ptr execute();

	ByteVector assemble();
};

using CFGBuilder_ptr = CONTROLFLOWGRAPH_API std::unique_ptr<CFGBuilder>;
