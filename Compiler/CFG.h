#pragma once
#include "CFG_Node.h"
#include <memory>

class CFG;
using CFG_ptr = std::shared_ptr<CFG>;

class CFG
{
	CFGNode_ptr start_node;
	CFGNode_ptr current_node;

public:
	void add_node(CFGNode_ptr node);
	void extend(CFGNode_ptr local_start_node, CFGNode_ptr local_end_node);
};
