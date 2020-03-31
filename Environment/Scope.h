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
	std::map<std::string, VariableInfo_ptr> variable_store;
	std::map<std::string, FunctionInfo_ptr> fn_store;
	std::map<std::string, UDTInfo_ptr> type_store;

	~Scope()
	{
		variable_store.clear();
		fn_store.clear();
		type_store.clear();
	}
};

using Scope_ptr = ENVIRONMENT_API std::shared_ptr<Scope>;