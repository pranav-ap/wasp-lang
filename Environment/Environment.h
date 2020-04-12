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

	// Enter and leave

	void enter_global_scope();
	void enter_module_scope();
	void enter_branch_scope();
	void enter_loop_scope();
	void enter_function_scope();

	void leave_scope();

	// Info Getters

	InfoVariant_ptr get_info(std::string name);

	VariableInfo& get_variable_info(std::string name);
	UDTInfo& get_UDT_info(std::string name);
	EnumInfo& get_enum_info(std::string name);

	// Variable Getters

	VectorObject& get_mutable_vector_variable(std::string name);
	UDTObject& get_mutable_UDT_variable(std::string name);

	// Setters

	void set_variable(std::string name, ObjectVariant_ptr value);
	void set_element(std::string name, int index, ObjectVariant_ptr value);

	// Creators

	void create_variable(
		std::string name,
		bool is_public,
		bool is_mutable,
		TypeVariant_ptr type,
		ObjectVariant_ptr result
	);

	void create_function(
		std::string name,
		bool is_public,
		std::vector<std::pair<std::string, TypeVariant_ptr>> arguments,
		std::optional<TypeVariant_ptr> return_type,
		Block_ptr body
	);

	void create_UDT(
		std::string name,
		bool is_public,
		std::map<std::string, TypeVariant_ptr> member_types
	);

	void create_enum(
		std::string name,
		bool is_public,
		std::set<std::string> member_names
	);

	void import_builtin(
		std::string name,
		std::function<ObjectVariant_ptr(std::vector<ObjectVariant_ptr>)> func
	);

	// Utils

	bool is_inside_module_scope();
	bool is_inside_branch_scope();
	bool is_inside_loop_scope();
	bool is_inside_function_scope();
};

using Environment_ptr = ENVIRONMENT_API std::shared_ptr<Environment>;
