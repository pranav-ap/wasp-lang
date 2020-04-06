#pragma once

#ifdef ENVIRONMENT_EXPORTS
#define ENVIRONMENT_API __declspec(dllexport)
#else
#define ENVIRONMENT_API __declspec(dllimport)
#endif

#include "Info.h"
#include <map>
#include <memory>

struct ENVIRONMENT_API Scope
{
	std::map<std::string, Info_ptr> store;
};

struct ENVIRONMENT_API ModuleScope : public Scope
{
};

struct ENVIRONMENT_API BlockScope : public Scope
{
};

struct ENVIRONMENT_API FunctionScope : public Scope
{
};

using Scope_ptr = ENVIRONMENT_API std::shared_ptr<Scope>;
using ModuleScope_ptr = ENVIRONMENT_API std::shared_ptr<ModuleScope>;
using BlockScope_ptr = ENVIRONMENT_API std::shared_ptr<BlockScope>;
using FunctionScope_ptr = ENVIRONMENT_API std::shared_ptr<FunctionScope>;
