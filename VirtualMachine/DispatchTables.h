#pragma once

#include "Builtins.h"
#include "Objects.h"
#include <string>
#include <vector>
#include <map>
#include <functional>

using InBuiltFunction_VisitorType = std::function<Object_ptr(std::vector<Object_ptr>)>;

using InBuiltFunction_DispatchTableType = std::map<std::wstring, InBuiltFunction_VisitorType>;
using InBuiltModule_DispatchTableType = std::map<std::wstring, InBuiltFunction_DispatchTableType>;

// FUNCTION NAME -> FUNCTION VISITOR

const InBuiltFunction_DispatchTableType core_dispatch_table = {
	{ L"echo", io::echo_visit }
};

const InBuiltFunction_DispatchTableType io_dispatch_table = {
	{ L"echo", io::echo_visit }
};

// MODULE NAME -> TABLE

const InBuiltModule_DispatchTableType builtins_dispatch_table = {
	{ L"core", core_dispatch_table },
	{ L"io", io_dispatch_table }
};

InBuiltFunction_VisitorType get_inbuilt_function_visitor(std::wstring module_name, std::wstring function_fullname);
