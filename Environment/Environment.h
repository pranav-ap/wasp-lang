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
	std::list<Scope_ptr> scopes;

public:

	Environment();

	// Info Getters

	Info_ptr get_info(std::string name);

	VariableInfo* get_variable_info(std::string name);
	UDTInfo* get_UDT_info(std::string name);
	EnumInfo* get_enum_info(std::string name);

	// Variable Getters

	ListObject* get_mutable_list_variable(std::string name);
	TupleObject* get_mutable_tuple_variable(std::string name);
	DictionaryObject* get_mutable_UDT_variable(std::string name);
	DictionaryObject* get_mutable_map_variable(std::string name);

	// Setters

	void set_variable(std::string name, Object_ptr value);
	void set_element(std::string name, int index, Object_ptr value);

	// Creators

	void create_variable(
		std::string name,
		bool is_public,
		bool is_mutable,
		Type_ptr type,
		Object_ptr result
	);

	void create_function(
		std::string name,
		std::string signature,
		bool is_public,
		std::vector<std::pair<std::string, Type_ptr>> arguments,
		std::optional<Type_ptr> return_type,
		Block body
	);

	void create_UDT(
		std::string name,
		bool is_public,
		std::map<std::string, Type_ptr> member_types
	);

	void create_enum(
		std::string name,
		bool is_public,
		std::set<std::string> member_names
	);

	void import_builtin(
		std::string name,
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
