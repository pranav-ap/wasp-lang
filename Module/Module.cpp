#pragma once
#include "pch.h"
#include "Module.h"
#include <iostream>
#include <memory>
#include <vector>

using std::cout;
using std::endl;
using std::move;
using std::vector;

vector<Statement_ptr>& Module::get_statements()
{
	return this->nodes;
}

void Module::add(Statement_ptr node)
{
	this->nodes.push_back(move(node));
}

MODULE_API std::ostream& operator<<(std::ostream& os, Module& mod)
{
	for (auto const& statement : mod.get_statements())
	{
		//os << statement << endl;
	}

	return os;
}