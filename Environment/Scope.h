#pragma once

#ifdef ENVIRONMENT_EXPORTS
#define ENVIRONMENT_API __declspec(dllexport)
#else
#define ENVIRONMENT_API __declspec(dllimport)
#endif

#include "Info.h"
#include <map>
#include <vector>
#include <memory>

struct ENVIRONMENT_API Scope
{
	std::map<std::string, InfoVariant_ptr> store;
};

struct ENVIRONMENT_API ModuleScope : public Scope
{
};

struct ENVIRONMENT_API FunctionScope : public Scope
{
};

struct ENVIRONMENT_API BlockScope : public Scope
{
};

struct ENVIRONMENT_API BranchScope : public BlockScope
{
};

struct ENVIRONMENT_API LoopScope : public BlockScope
{
};

using Scope_ptr = ENVIRONMENT_API std::shared_ptr<Scope>;
using ModuleScope_ptr = ENVIRONMENT_API std::shared_ptr<ModuleScope>;
using FunctionScope_ptr = ENVIRONMENT_API std::shared_ptr<FunctionScope>;
using BlockScope_ptr = ENVIRONMENT_API std::shared_ptr<BlockScope>;
using BranchScope_ptr = ENVIRONMENT_API std::shared_ptr<BranchScope>;
using LoopScope_ptr = ENVIRONMENT_API std::shared_ptr<LoopScope>;
