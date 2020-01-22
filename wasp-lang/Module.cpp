#pragma once
#include <iostream>
#include "Module.h"

using std::cout;
using std::endl;

std::vector<StatementNode_ptr>& Module::get_nodes()
{
	return this->nodes;
}

void Module::add(StatementNode_ptr node)
{
	this->nodes.push_back(node);
}

void Module::print()
{
	cout << "\n Syntax Tree" << endl;

	for (auto const& node : this->nodes)
	{
		cout << endl;
		print_statement_node(node, 4);
	}
}