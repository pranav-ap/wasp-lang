#pragma once

#ifdef ENVIRONMENT_EXPORTS
#define ENVIRONMENT_API __declspec(dllexport)
#else
#define ENVIRONMENT_API __declspec(dllimport)
#endif

#include "ObjectSystem.h"
#include "Scope.h"
#include "Info.h"
#include <list>
#include <vector>
#include <set>
#include <string>
#include <functional>

class ENVIRONMENT_API Environment
{
	int curent_scope_level;
	std::list<Scope_ptr> scopes;

public:

	Environment();

	// Info Getters

	Info_ptr get_info(std::wstring name);

	VariableInfo* get_variable_info(std::wstring name);
	UDTInfo* get_UDT_info(std::wstring name);
	EnumInfo* get_enum_info(std::wstring name);

	InBuiltFunctionInfo* get_inbuilt_function_info_if_exists(std::wstring name);

	// Variable Getters

	ListObject* get_mutable_list_variable(std::wstring name);
	TupleObject* get_mutable_tuple_variable(std::wstring name);
	MapObject* get_mutable_UDT_variable(std::wstring name);
	MapObject* get_mutable_map_variable(std::wstring name);

	// Setters

	void set_variable(std::wstring name, Object_ptr value);
	void set_element(std::wstring name, int index, Object_ptr value);

	// Creators

	void create_variable(
		std::wstring name,
		bool is_public,
		bool is_mutable,
		Type_ptr type,
		Object_ptr result
	);

	void create_function(
		std::wstring name,
		std::wstring signature,
		bool is_public,
		std::vector<std::pair<std::wstring, Type_ptr>> arguments,
		std::optional<Type_ptr> return_type,
		Block body
	);

	void create_UDT(
		std::wstring name,
		bool is_public,
		std::map<std::wstring, Type_ptr> member_types
	);

	void create_enum(
		std::wstring name,
		bool is_public,
		std::set<std::wstring> member_names
	);

	void import_builtin(
		std::wstring name,
		std::function<Object_ptr(std::vector<Object_ptr>)> func
	);

	// Enter and leave

	void enter_global_scope();
	void enter_module_scope();
	void enter_branch_scope();
	void enter_loop_scope();
	void enter_function_scope();

	void leave_scope();

	// is inside ?

	bool is_inside_module_scope();
	bool is_inside_branch_scope();
	bool is_inside_loop_scope();
	bool is_inside_function_scope();
};

using Environment_ptr = ENVIRONMENT_API std::shared_ptr<Environment>;
