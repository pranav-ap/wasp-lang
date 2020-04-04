#pragma once
#include "pch.h"
#include "Module.h"
#include <memory>

using std::move;

void Module::add(Statement_ptr node)
{
	this->nodes.push_back(move(node));
}