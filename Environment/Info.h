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

struct ENVIRONMENT_API Info
{
	bool is_public;
	Info(bool is_public) : is_public(is_public) {};
	virtual void accept() = 0;
};

struct ENVIRONMENT_API VariableInfo : public Info
{
	bool is_mutable;
	Type_ptr type;
	Object_ptr value;

	VariableInfo(bool is_public, bool is_mutable, Type_ptr type, Object_ptr value)
		: Info(is_public), is_mutable(is_mutable), type(type), value(value) {};
	void accept() {};
};

struct ENVIRONMENT_API FunctionInfo : public Info
{
	std::map<std::string, Type_ptr> arguments;
	std::optional<Type_ptr> return_type;
	Block_ptr body;

	FunctionInfo(bool is_public, std::map<std::string, Type_ptr> arguments, std::optional<Type_ptr> return_type, Block_ptr body)
		: Info(is_public), arguments(arguments), return_type(return_type), body(body) {};
	void accept() {};
};

struct ENVIRONMENT_API UDTInfo : public Info
{
	std::map<std::string, Type_ptr> member_types;

	UDTInfo(bool is_public, std::map<std::string, Type_ptr> member_types)
		: Info(is_public), member_types(member_types) {};
	void accept() {};
};

struct ENVIRONMENT_API EnumInfo : public Info
{
	std::vector<std::string> member_names;

	EnumInfo(bool is_public, std::vector<std::string> member_names)
		: Info(is_public), member_names(member_names) {};
	void accept() {};
};

using Info_ptr = ENVIRONMENT_API std::shared_ptr<Info>;

using VariableInfo_ptr = ENVIRONMENT_API std::shared_ptr<VariableInfo>;
using FunctionInfo_ptr = ENVIRONMENT_API std::shared_ptr<FunctionInfo>;
using UDTInfo_ptr = ENVIRONMENT_API std::shared_ptr<UDTInfo>;
using EnumInfo_ptr = ENVIRONMENT_API std::shared_ptr<EnumInfo>;
