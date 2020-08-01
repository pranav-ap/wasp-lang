#include "pch.h"
#include "Statement.h"

void Module::add_node(Statement_ptr node)
{
	statements.push_back(node);
}