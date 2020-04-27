#pragma once

#include "Info.h"
#include <map>
#include <memory>

struct Scope
{
	std::map<std::string, Info_ptr> store;
};

using Scope_ptr = std::shared_ptr<Scope>;

struct GlobalScope : public Scope
{
};

struct ModuleScope : public Scope
{
};

struct FunctionScope : public Scope
{
};

struct BlockScope : public Scope
{
};

struct BranchScope : public BlockScope
{
};

struct LoopScope : public BlockScope
{
};
