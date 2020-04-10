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
#include <functional>

struct VariableInfo;
struct FunctionInfo;
struct InBuiltFunctionInfo;
struct UDTInfo;
struct EnumInfo;

// Variant Definition

using InfoVariant = ENVIRONMENT_API std::variant<
	std::monostate,
	VariableInfo,
	FunctionInfo,
	InBuiltFunctionInfo,
	UDTInfo,
	EnumInfo
>;

// Defining Info structs

struct ENVIRONMENT_API Info
{
	bool is_public;
	Info(bool is_public) : is_public(is_public) {};
};

struct ENVIRONMENT_API VariableInfo : public Info
{
	bool is_mutable;
	Type_ptr type;
	ObjectVariant_ptr value;

	VariableInfo(bool is_public, bool is_mutable, Type_ptr type, ObjectVariant_ptr value)
		: Info(is_public), is_mutable(is_mutable), type(type), value(std::move(value)) {};
};

struct ENVIRONMENT_API UDTInfo : public Info
{
	std::map<std::string, Type_ptr> member_types;

	UDTInfo(bool is_public, std::map<std::string, Type_ptr> member_types)
		: Info(is_public), member_types(member_types) {};
};

struct ENVIRONMENT_API EnumInfo : public Info
{
	std::vector<std::string> members;

	EnumInfo(bool is_public, std::vector<std::string> members)
		: Info(is_public), members(members) {};
};

struct ENVIRONMENT_API FunctionInfo : public Info
{
	std::vector<std::pair<std::string, Type_ptr>> arguments;
	std::optional<Type_ptr> return_type;
	Block_ptr body;

	FunctionInfo(bool is_public, std::vector<std::pair<std::string, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block_ptr body)
		: Info(is_public), arguments(arguments), return_type(return_type), body(body) {};
};

struct ENVIRONMENT_API InBuiltFunctionInfo : public Info
{
	std::function<ObjectVariant_ptr(std::vector<ObjectVariant_ptr>)> func;

	InBuiltFunctionInfo(std::function<ObjectVariant_ptr(std::vector<ObjectVariant_ptr>)> func)
		: Info(false), func(func) {};
};

using InfoVariant_ptr = ENVIRONMENT_API std::shared_ptr<InfoVariant>;

using VariableInfo_ptr = ENVIRONMENT_API std::shared_ptr<VariableInfo>;
using FunctionInfo_ptr = ENVIRONMENT_API std::shared_ptr<FunctionInfo>;
using InBuiltFunctionInfo_ptr = ENVIRONMENT_API std::shared_ptr<InBuiltFunctionInfo>;
using UDTInfo_ptr = ENVIRONMENT_API std::shared_ptr<UDTInfo>;
using EnumInfo_ptr = ENVIRONMENT_API std::shared_ptr<EnumInfo>;