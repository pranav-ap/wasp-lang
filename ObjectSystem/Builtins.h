#pragma once

#ifdef OBJECTSYSTEM_EXPORTS
#define OBJECTSYSTEM_API __declspec(dllexport)
#else
#define OBJECTSYSTEM_API __declspec(dllimport)
#endif

#include "Objects.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>

namespace io
{
	OBJECTSYSTEM_API Object_ptr echo(std::vector<Object_ptr> arguments);
}

using Builtins_ObjectTable = std::map<std::wstring, Object_ptr>;
using Builtins_ModuleTable = std::map<std::wstring, Builtins_ObjectTable>;

class OBJECTSYSTEM_API BuiltinsManager
{
	Builtins_ModuleTable type_table;
	Builtins_ModuleTable module_table;

public:
	BuiltinsManager();

	void set_native_type(std::wstring module_name, std::wstring name, Object_ptr type);
	Object_ptr get_native_type(std::wstring module_name, std::wstring name);

	Object_ptr get_native_object(std::wstring module_name, std::wstring name);

	bool exists(std::wstring module_name, std::wstring name);
};

using BuiltinsManager_ptr = OBJECTSYSTEM_API std::shared_ptr<BuiltinsManager>;
