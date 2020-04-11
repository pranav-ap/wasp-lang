#pragma once

#include "ObjectSystem.h"
#include "Statement.h"
#include "Types.h"
#include <map>
#include <vector>
#include <memory>
#include <functional>

struct VariableInfo;
struct FunctionInfo;
struct InBuiltFunctionInfo;
struct UDTInfo;
struct EnumInfo;

// Variant Definition

using InfoVariant = std::variant<
	std::monostate,
	VariableInfo,
	FunctionInfo,
	InBuiltFunctionInfo,
	UDTInfo,
	EnumInfo
>;

// Defining Info structs

struct Info
{
	bool is_public;
	Info(bool is_public) : is_public(is_public) {};
};

struct VariableInfo : public Info
{
	bool is_mutable;
	TypeVariant_ptr type;
	ObjectVariant_ptr value;

	VariableInfo(bool is_public, bool is_mutable, TypeVariant_ptr type, ObjectVariant_ptr value)
		: Info(is_public), is_mutable(is_mutable), type(type), value(std::move(value)) {};
};

struct UDTInfo : public Info
{
	std::map<std::string, TypeVariant_ptr> member_types;
	UDTInfo(bool is_public, std::map<std::string, TypeVariant_ptr> member_types)
		: Info(is_public), member_types(member_types) {};
};

struct EnumInfo : public Info
{
	std::vector<std::string> members;
	EnumInfo(bool is_public, std::vector<std::string> members)
		: Info(is_public), members(members) {};
};

struct FunctionInfo : public Info
{
	std::vector<std::pair<std::string, TypeVariant_ptr>> arguments;
	std::optional<TypeVariant_ptr> return_type;
	Block_ptr body;

	FunctionInfo(bool is_public, std::vector<std::pair<std::string, TypeVariant_ptr>> arguments, std::optional<TypeVariant_ptr> return_type, Block_ptr body)
		: Info(is_public), arguments(arguments), return_type(return_type), body(body) {};
};

struct InBuiltFunctionInfo : public Info
{
	std::function<ObjectVariant_ptr(std::vector<ObjectVariant_ptr>)> func;

	InBuiltFunctionInfo(std::function<ObjectVariant_ptr(std::vector<ObjectVariant_ptr>)> func)
		: Info(false), func(func) {};
};

using InfoVariant_ptr = std::shared_ptr<InfoVariant>;
