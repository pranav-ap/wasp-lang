#pragma once
#include "Module.h"
#include <memory>

void Module::add(Statement_ptr node)
{
	this->nodes.push_back(std::move(node));
}