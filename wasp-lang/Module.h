#pragma once
#include <vector>
#include "StatementNodes.h"

class Module
{
	std::vector<Statement_ptr> nodes;

public:
	std::vector<Statement_ptr>& get_statements();
	void add(Statement_ptr node);
	void print();
};
