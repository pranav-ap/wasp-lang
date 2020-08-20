#pragma once
#include "CFG_Node.h"
#include <memory>

class CFG;
using CFG_ptr = std::shared_ptr<CFG>;

class CFG
{
	CFG_Node_ptr start_node;
	CFG_Node_ptr current_node;

public:
	void add_node(CFG_Node_ptr node);
	void add_node(CFG_Node_ptr node, bool true_path);

	void add_local_cfg(CFG_Node_ptr local_start_node, CFG_Node_ptr local_end_node);
	void add_local_cfg(CFG_Node_ptr local_start_node, CFG_Node_ptr local_end_node, bool true_path);

	void emit(Instruction instruction);
};
