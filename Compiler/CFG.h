#pragma once
#include "BasicBlock.h"
#include <memory>

class CFG;
using CFG_ptr = std::shared_ptr<CFG>;

class CFG
{
	int next_id;
public:
	CFG_ptr build_control_flow_graph();
};
