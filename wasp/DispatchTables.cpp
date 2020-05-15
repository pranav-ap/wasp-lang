#pragma once

#include "DispatchTables.h"
#include "Assertion.h"
#include <string>

using std::string;

InBuiltFunction_VisitorType get_inbuilt_function_visitor(string module_name, string function_name)
{
	ASSERT(
		module_dispatch_table.contains(module_name),
		"There is no inbuilt module called " + module_name
	);

	ASSERT(
		module_dispatch_table.at(module_name).contains(function_name),
		"There is no inbuilt function " + function_name + " in module " + module_name
	);

	return module_dispatch_table.at(module_name).at(function_name);
}