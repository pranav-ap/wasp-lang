#pragma once
#include <vector>
#include "StatementNodes.h"

class Module
{
	std::vector<StatementNode_ptr> nodes;

public:
	std::vector<StatementNode_ptr>& get_nodes();
	void add(StatementNode_ptr node);
	void print();
};
