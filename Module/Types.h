#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

#include "pch.h"
#include "ObjectSystem.h"
#include <string>
#include <memory>
#include <variant>
#include <vector>

struct NumberType;
struct StringType;
struct BooleanType;
struct ListType;
struct TupleType;
struct UDTType;
struct EnumType;
struct MapType;
struct OptionalType;
struct VariantType;
struct AnyType;

using Type = MODULE_API std::variant<
	std::monostate,
	NumberType, StringType, BooleanType,
	ListType, TupleType,
	UDTType, MapType,
	EnumType,
	OptionalType, VariantType,
	AnyType
>;

using Type_ptr = MODULE_API std::shared_ptr<Type>;

// Type Base

struct MODULE_API BaseType
{
};

struct MODULE_API ScalarType : public BaseType
{
};

struct MODULE_API CompositeType : public BaseType
{
};

struct MODULE_API AnyType : public BaseType
{
};

// Scalar Types

struct MODULE_API NumberType : public ScalarType
{
};

struct MODULE_API StringType : public ScalarType
{
};

struct MODULE_API BooleanType : public ScalarType
{
};

// Composite Types

struct MODULE_API ListType : public CompositeType
{
	Type_ptr element_type;
	ListType(Type_ptr element_type)
		: element_type(std::move(element_type)) {};
};

struct MODULE_API TupleType : public CompositeType
{
	std::vector<Type_ptr> element_types;
	TupleType(std::vector<Type_ptr> element_types)
		: element_types(element_types) {};
};

struct MODULE_API MapType : public CompositeType
{
	Type_ptr key_type;
	Type_ptr value_type;

	MapType(Type_ptr from_type, Type_ptr to_type)
		: key_type(std::move(from_type)),
		value_type(std::move(to_type)) {};
};

struct MODULE_API UDTType : public CompositeType
{
	std::string name;
	UDTType(std::string name) : name(name) {};
};

struct MODULE_API EnumType : public CompositeType
{
	std::string enum_name;
	EnumType(std::string enum_name)
		: enum_name(enum_name) {};
};

struct MODULE_API OptionalType : public CompositeType
{
	Type_ptr type;
	OptionalType(Type_ptr type) : type(std::move(type)) {};
};

struct MODULE_API VariantType : public CompositeType
{
	std::vector<Type_ptr> types;
	VariantType(std::vector<Type_ptr> types) : types(types) {};
};

std::string MODULE_API get_type_string(Type_ptr type);
