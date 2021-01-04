#pragma once
#include "pch.h"
#include "DispatchTables.h"
#include "Assertion.h"
#include <string>

using std::wstring;

InBuiltFunction_VisitorType get_inbuilt_function_visitor(wstring module_name, wstring function_name)
{
	ASSERT(
		builtins_dispatch_table.contains(module_name),
		"This inbuilt function does not exist in module"
	);

	ASSERT(
		builtins_dispatch_table.at(module_name).contains(function_name),
		"This builtin bmodule does not exist"
	);

	return builtins_dispatch_table.at(module_name).at(function_name);
}