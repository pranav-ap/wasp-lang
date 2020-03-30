#pragma once

#ifdef PARSER_EXPORTS
#define PARSER_API __declspec(dllexport)
#else
#define PARSER_API __declspec(dllimport)
#endif

#include <vector>
#include "Statement.h"

class PARSER_API Module
{
	std::vector<Statement_ptr> nodes;

public:
	std::vector<Statement_ptr>& get_statements();
	void add(Statement_ptr node);
};

PARSER_API std::ostream& operator<<(std::ostream& os, const Module& mod);
