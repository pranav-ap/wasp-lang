#pragma once
#include "Bytecode.h"
#include "BasicBlock.h"
#include <memory>

class CFGBuilder;
using CFG_ptr = std::shared_ptr<CFGBuilder>;

class CFGBuilder
{
	BasicBlock_ptr start_node;
	BasicBlock_ptr current_node;

public:
	void add_node(BasicBlock_ptr node);
	void extend(BasicBlock_ptr local_start_node, BasicBlock_ptr local_end_node);

	BasicBlock_ptr execute(Bytecode_ptr bytecode);
};
