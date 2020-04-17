#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

#include "Statement.h"
#include <vector>

struct MODULE_API Module
{
	Block nodes;
	void add(Statement_ptr node);
};
