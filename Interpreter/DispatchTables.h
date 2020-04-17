#pragma once

#include "Builtins.h"
#include "ObjectSystem.h"
#include <string>
#include <vector>
#include <map>
#include <functional>

using InBuiltFunction_VisitorType = std::function<Object_ptr(std::vector<Object_ptr>)>;

using InBuiltFunction_DispatchTableType = std::map<
	std::string,
	InBuiltFunction_VisitorType
>;

using InBuiltModule_DispatchTableType = std::map<
	std::string,
	InBuiltFunction_DispatchTableType
>;

// FUNCTION NAME -> FUNCTION VISITOR

const InBuiltFunction_DispatchTableType io_dispatch_table = {
	{ "echo", io::echo_visit },
	{ "ask", io::ask_visit }
};

const InBuiltFunction_DispatchTableType core_dispatch_table = {
	{ "size", core::size_visit },
};

// MODULE NAME -> TABLE

const InBuiltModule_DispatchTableType module_dispatch_table = {
	{ "io", io_dispatch_table },
	{ "core", core_dispatch_table }
};

InBuiltFunction_VisitorType get_inbuilt_function_visitor(std::string module_name, std::string function_name);
