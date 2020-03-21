#pragma once
#include <iostream>
#include "Module.h"

using std::cout;
using std::endl;

std::vector<Statement_ptr>& Module::get_statements()
{
	return this->nodes;
}

void Module::add(Statement_ptr node)
{
	this->nodes.emplace_back(node);
}

void Module::print()
{
	cout << "\n Syntax Tree" << endl;

	for (auto const& node : this->nodes)
	{
		cout << endl;
		node->print(4);
	}
}