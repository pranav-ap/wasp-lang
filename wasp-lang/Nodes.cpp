#pragma once
#include "Nodes.h"

void Module::add(StatementNode_ptr node)
{
	this->nodes.push_back(node);
}