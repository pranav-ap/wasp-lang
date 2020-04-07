#pragma once

#ifdef ENVIRONMENT_EXPORTS
#define ENVIRONMENT_API __declspec(dllexport)
#else
#define ENVIRONMENT_API __declspec(dllimport)
#endif

#include "ObjectVisitor.h"
#include "Scope.h"
#include "Info.h"
#include <list>

class ENVIRONMENT_API Environment
{
	std::list<Scope_ptr> scopes;
	Info_ptr get_info(std::string name);

public:

	Environment();

	// Enter and leave

	void enter_branch_scope();
	void enter_loop_scope();
	void enter_function_scope();

	void leave_scope();

	// Getters

	VariableInfo_ptr get_variable(std::string name);
	FunctionInfo_ptr get_function(std::string name);
	UDTInfo_ptr get_UDT(std::string name);
	EnumInfo_ptr get_enum(std::string name);

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
		bool is_public,
		std::vector<std::pair<std::string, Type_ptr>> arguments,
		std::optional<Type_ptr> return_type,
		Block_ptr body
	);

	void create_UDT(
		std::string name,
		bool is_public,
		std::map<std::string, Type_ptr> member_types
	);

	void create_enum(
		std::string name,
		bool is_public,
		std::vector<std::string> member_names
	);

	// Utils

	bool is_inside_branch_scope();
	bool is_inside_loop_scope();
	bool is_inside_function_scope();
};

using Environment_ptr = ENVIRONMENT_API std::shared_ptr<Environment>;
