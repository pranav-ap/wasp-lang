#pragma once

#include "ObjectSystem.h"
#include "Statement.h"
#include "TypeSystem.h"
#include <map>
#include <set>
#include <tuple>
#include <vector>
#include <memory>
#include <functional>

struct VariableInfo;
struct FunctionInfo;
struct InBuiltFunctionInfo;
struct UDTInfo;
struct EnumInfo;
struct AliasInfo;

using Info = std::variant<
	std::monostate,
	VariableInfo,
	FunctionInfo,
	InBuiltFunctionInfo,
	UDTInfo,
	EnumInfo,
	AliasInfo
>;

using Info_ptr = std::shared_ptr<Info>;
using Arguments = std::vector<std::pair<std::string, Type_ptr>>;
using ReturnType = std::optional<Type_ptr>;

struct BaseInfo
{
	bool is_public;
	BaseInfo(bool is_public) : is_public(is_public) {};
};

struct VariableInfo : public BaseInfo
{
	bool is_mutable;
	Type_ptr type;
	Object_ptr value;

	VariableInfo(bool is_public, bool is_mutable, Type_ptr type, Object_ptr value)
		: BaseInfo(is_public), is_mutable(is_mutable), type(std::move(type)), value(std::move(value)) {};
};

struct UDTInfo : public BaseInfo
{
	std::map<std::string, Type_ptr> member_types;
	UDTInfo(bool is_public, std::map<std::string, Type_ptr> member_types)
		: BaseInfo(is_public), member_types(member_types) {};
};

struct AliasInfo : public BaseInfo
{
	Type_ptr type;
	AliasInfo(bool is_public, Type_ptr type)
		: BaseInfo(is_public), type(type) {};
};

struct EnumInfo : public BaseInfo
{
	std::string enum_name;
	std::set<std::string> members;

	EnumInfo(std::string enum_name, bool is_public, std::set<std::string> members)
		: BaseInfo(is_public), members(members) {};
};

struct FunctionInfo : public BaseInfo
{
	Arguments arguments;
	ReturnType return_type;
	Block body;

	FunctionInfo(bool is_public, Arguments arguments, ReturnType return_type, Block body)
		: BaseInfo(is_public), arguments(arguments), return_type(return_type), body(body) {};
};

struct InBuiltFunctionInfo : public BaseInfo
{
	std::function<Object_ptr(std::vector<Object_ptr>)> func;

	InBuiltFunctionInfo(std::function<Object_ptr(std::vector<Object_ptr>)> func)
		: BaseInfo(false), func(func) {};
};
