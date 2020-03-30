#pragma once
#include "pch.h"
#include <iostream>
#include <memory>
#include "Module.h"

using std::cout;
using std::endl;
using std::move;

std::vector<Statement_ptr>& Module::get_statements()
{
	return this->nodes;
}

void Module::add(Statement_ptr node)
{
	this->nodes.push_back(move(node));
}

PARSER_API std::ostream& operator<<(std::ostream& os, Module& mod)
{
	for (auto const& statement : mod.get_statements())
	{
		//os << statement << endl;
	}

	return os;
}