#pragma once
#include <vector>
#include <memory>
#include "StatementNodes.h"

class Module
{
	std::vector<StatementNode_ptr> nodes;

public:
	void add(StatementNode_ptr node);
};
