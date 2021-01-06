#pragma once
#include <memory>
#include <set>

struct LocalScope
{
	std::set<int> local_declarations;

	void add(int id);
	void remove(int id);
};

using LocalScope_ptr = std::shared_ptr<LocalScope>;
