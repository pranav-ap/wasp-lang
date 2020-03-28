#pragma once

#ifdef INTERPRETER_EXPORTS
#define INTERPRETER_API __declspec(dllexport)
#else
#define INTERPRETER_API __declspec(dllimport)
#endif

#include "Module.h"
#include "Statement.h"
#include "ObjectSystem.h"
#include <stack>
#include <map>
#include <memory>

// Info

struct INTERPRETER_API VariableInfo
{
	bool is_public;
	bool is_mutable;
	Type_ptr type;
	Object_ptr value;

	VariableInfo(bool is_public, bool is_mutable, Type_ptr type, Object_ptr value)
		: is_public(is_public), is_mutable(is_mutable), type(type), value(value) {};
};

struct INTERPRETER_API FunctionInfo
{
	bool is_public;
	std::map<std::string, Type_ptr> arguments;
	std::optional<Type_ptr> return_type;
	Block_ptr body;

	FunctionInfo(bool is_public, std::map<std::string, Type_ptr> arguments, std::optional<Type_ptr> return_type, Block_ptr body)
		: is_public(is_public), arguments(arguments), return_type(return_type), body(body) {};
};

struct INTERPRETER_API UDTInfo
{
	bool is_public;
	std::map<std::string, Type_ptr> member_types;

	UDTInfo(bool is_public, std::map<std::string, Type_ptr> member_types)
		: is_public(is_public), member_types(member_types) {};
};

using VariableInfo_ptr = INTERPRETER_API std::shared_ptr<VariableInfo>;
using FunctionInfo_ptr = INTERPRETER_API std::shared_ptr<FunctionInfo>;
using UDTInfo_ptr = INTERPRETER_API std::shared_ptr<UDTInfo>;

// Environment

struct INTERPRETER_API Environment
{
	std::map<std::string, VariableInfo_ptr> variable_store;
	std::map<std::string, FunctionInfo_ptr> fn_store;
	std::map<std::string, UDTInfo_ptr> type_store;

	~Environment()
	{
		variable_store.clear();
		fn_store.clear();
		type_store.clear();
	}
};

using Environment_ptr = INTERPRETER_API std::shared_ptr<Environment>;
