#pragma once

#include "ObjectSystem.h"
#include "Statement.h"
#include "Type_System.h"
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

using Info = std::variant<
	std::monostate,
	VariableInfo,
	FunctionInfo,
	InBuiltFunctionInfo,
	UDTInfo,
	EnumInfo
>;

using Info_ptr = std::shared_ptr<Info>;
using Arguments = std::vector<std::pair<std::wstring, Type_ptr>>;
using ReturnType = std::optional<Type_ptr>;

// Defining Info structs

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
	std::map<std::wstring, Type_ptr> member_types;
	UDTInfo(bool is_public, std::map<std::wstring, Type_ptr> member_types)
		: BaseInfo(is_public), member_types(member_types) {};
};

struct EnumInfo : public BaseInfo
{
	std::wstring enum_name;
	std::set<std::wstring> members;

	EnumInfo(std::wstring enum_name, bool is_public, std::set<std::wstring> members)
		: BaseInfo(is_public), members(members) {};
};

struct FunctionInfo : public BaseInfo
{
	std::wstring name;
	Arguments arguments;
	ReturnType return_type;
	Block body;

	FunctionInfo(bool is_public, std::wstring name, Arguments arguments, ReturnType return_type, Block body)
		: BaseInfo(is_public), name(name), arguments(arguments), return_type(return_type), body(body) {};
};

struct InBuiltFunctionInfo : public BaseInfo
{
	std::function<Object_ptr(std::vector<Object_ptr>)> func;

	InBuiltFunctionInfo(std::function<Object_ptr(std::vector<Object_ptr>)> func)
		: BaseInfo(false), func(func) {};
};
