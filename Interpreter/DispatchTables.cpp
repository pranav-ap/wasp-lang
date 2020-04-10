#pragma once

#include "pch.h"
#include "DispatchTables.h"
#include "spdlog.h"
#include <string>

using std::string;

InBuiltFunction_VisitorType get_inbuilt_function_visitor(string module_name, string function_name)
{
	if (module_dispatch_table.contains(module_name))
	{
		if (module_dispatch_table.at(module_name).contains(function_name))
		{
			return module_dispatch_table.at(module_name).at(function_name);
		}

		spdlog::error("There is no inbuilt function {} in module {}", function_name, module_name);
	}

	spdlog::error("There is no inbuilt module called {} ", module_name);
}