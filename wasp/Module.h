#pragma once

#include "Statement.h"
#include <vector>

struct Module
{
	Block nodes;
	void add(Statement_ptr node);
};
