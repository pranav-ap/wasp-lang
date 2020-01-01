#pragma once
#include <vector>
#include "StatementNodes.h"

class Module
{
	std::vector<StatementNode_ptr> nodes;

public:
	void add(StatementNode_ptr node);
	void print();
};
