#pragma once
#include "Nodes.h"

void Module::add(StatementNode_ptr node)
{
	this->nodes.push_back(node);
}

void Module::print()
{
	for (auto const& node : this->nodes)
	{
		print_statement_node(node, 0);
	}
}