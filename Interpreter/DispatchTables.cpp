#pragma once

#include "pch.h"
#include "DispatchTables.h"
#include "logger.h"

InBuiltFunction_VisitorType get_inbuilt_function_visitor(
	std::string module_name,
	std::string function_name)
{
	FATAL_IF_FALSE(
		module_dispatch_table.contains(module_name),
		"There is no inbuilt module called " + module_name
	);

	FATAL_IF_FALSE(
		module_dispatch_table.at(module_name).contains(function_name),
		"There is no inbuilt function " + function_name + " in module " + module_name
	);

	return module_dispatch_table.at(module_name).at(function_name);
}