#pragma once

#ifdef ENVIRONMENT_EXPORTS
#define ENVIRONMENT_API __declspec(dllexport)
#else
#define ENVIRONMENT_API __declspec(dllimport)
#endif

#include "Scope.h"
#include "Info.h"
#include <list>

class ENVIRONMENT_API Environment
{
	std::list<Scope_ptr> scopes;

public:
	Environment();

	VariableInfo_ptr get_variable(std::string name);
	FunctionInfo_ptr get_function(std::string name);
	UDTInfo_ptr get_UDT(std::string name);

	void set_variable(std::string name, VariableInfo_ptr info);
	void set_function(std::string name, FunctionInfo_ptr info);
	void set_UDT(std::string name, UDTInfo_ptr info);

	void create_and_set_variable(
		std::string name,
		bool is_public,
		bool is_mutable,
		Type_ptr type,
		Object_ptr result
	);
	void create_and_set_function(
		std::string name,
		bool is_public,
		std::map<std::string, Type_ptr> arguments,
		std::optional<Type_ptr> return_type,
		Block_ptr body
	);
	void create_and_set_UDT(
		std::string name,
		bool is_public,
		std::map<std::string, Type_ptr> member_types
	);
};

using Environment_ptr = ENVIRONMENT_API std::shared_ptr<Environment>;
