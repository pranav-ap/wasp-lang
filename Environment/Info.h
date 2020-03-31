#pragma once

#ifdef ENVIRONMENT_EXPORTS
#define ENVIRONMENT_API __declspec(dllexport)
#else
#define ENVIRONMENT_API __declspec(dllimport)
#endif

#include "ObjectSystem.h"
#include "Statement.h"
#include "Types.h"
#include <map>
#include <memory>

struct ENVIRONMENT_API VariableInfo
{
	bool is_public;
	bool is_mutable;
	Type_ptr type;
	Object_ptr value;

	VariableInfo(bool is_public, bool is_mutable, Type_ptr type, Object_ptr value)
		: is_public(is_public), is_mutable(is_mutable), type(type), value(value) {};
};

struct ENVIRONMENT_API FunctionInfo
{
	bool is_public;
	std::map<std::string, Type_ptr> arguments;
	std::optional<Type_ptr> return_type;
	Block_ptr body;

	FunctionInfo(bool is_public, std::map<std::string, Type_ptr> arguments, std::optional<Type_ptr> return_type, Block_ptr body)
		: is_public(is_public), arguments(arguments), return_type(return_type), body(body) {};
};

struct ENVIRONMENT_API UDTInfo
{
	bool is_public;
	std::map<std::string, Type_ptr> member_types;

	UDTInfo(bool is_public, std::map<std::string, Type_ptr> member_types)
		: is_public(is_public), member_types(member_types) {};
};

using VariableInfo_ptr = ENVIRONMENT_API std::shared_ptr<VariableInfo>;
using FunctionInfo_ptr = ENVIRONMENT_API std::shared_ptr<FunctionInfo>;
using UDTInfo_ptr = ENVIRONMENT_API std::shared_ptr<UDTInfo>;
