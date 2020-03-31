#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

#include <vector>
#include "Statement.h"

class MODULE_API Module
{
	std::vector<Statement_ptr> nodes;

public:
	std::vector<Statement_ptr>& get_statements();
	void add(Statement_ptr node);
};

MODULE_API std::ostream& operator<<(std::ostream& os, const Module& mod);
