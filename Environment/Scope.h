#pragma once

#include "Info.h"
#include <map>
#include <memory>

struct Scope
{
	std::map<std::string, InfoVariant_ptr> store;
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
using ModuleScope_ptr = std::shared_ptr<ModuleScope>;
using FunctionScope_ptr = std::shared_ptr<FunctionScope>;
using BlockScope_ptr = std::shared_ptr<BlockScope>;
using BranchScope_ptr = std::shared_ptr<BranchScope>;
using LoopScope_ptr = std::shared_ptr<LoopScope>;
