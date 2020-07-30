#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#include "Statement.h"
#include <vector>

struct AST_API AST
{
	std::vector<Statement_ptr> nodes;
	void add(Statement_ptr node);
};

using AST_ptr = AST_API std::shared_ptr<AST>;
