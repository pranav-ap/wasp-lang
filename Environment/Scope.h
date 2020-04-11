#pragma once

#include "Info.h"
#include <map>
#include <memory>

struct Scope
{
	std::map<std::string, InfoVariant_ptr> store;
};

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

using Scope_ptr = std::shared_ptr<Scope>;