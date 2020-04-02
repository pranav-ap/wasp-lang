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

	~Scope()
	{
		store.clear();
	}
};

using Scope_ptr = ENVIRONMENT_API std::shared_ptr<Scope>;
