#pragma once
#include "pch.h"
#include "AST.h"
#include <memory>

void AST::add(Statement_ptr node)
{
	nodes.push_back(std::move(node));
}