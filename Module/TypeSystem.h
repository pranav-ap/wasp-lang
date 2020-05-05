#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

#include "pch.h"
#include <string>
#include <memory>
#include <vector>
#include <variant>

struct AnyType;
struct NumberType;
struct StringType;
struct BooleanType;
struct ListType;
struct TupleType;
struct UDTType;
struct MapType;
struct EnumType;
struct VariantType;
struct FunctionType;
struct NoneType;

using Type = MODULE_API std::variant<
	std::monostate,
	NumberType, StringType, BooleanType,
	ListType, TupleType,
	UDTType, MapType,
	EnumType,
	VariantType,
	FunctionType,
	NoneType,
	AnyType
>;

using Type_ptr = MODULE_API std::shared_ptr<Type>;
using type_vector = MODULE_API std::vector<Type_ptr>;

// Type Base

struct MODULE_API AnyType
{
};

struct MODULE_API ScalarType : public AnyType
{
};

struct MODULE_API CompositeType : public AnyType
{
};

struct MODULE_API FunctionType : public AnyType
{
	type_vector argument_types;
	FunctionType(type_vector argument_types) : argument_types(argument_types) {};
};

struct MODULE_API NoneType : public AnyType
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
	ListType(Type_ptr element_type) : element_type(std::move(element_type)) {};
};

struct MODULE_API TupleType : public CompositeType
{
	type_vector element_types;
	TupleType(type_vector element_types) : element_types(element_types) {};
};

struct MODULE_API MapType : public CompositeType
{
	Type_ptr key_type;
	Type_ptr value_type;

	MapType(Type_ptr key_type, Type_ptr value_type)
		: key_type(std::move(key_type)), value_type(std::move(value_type)) {};
};

struct MODULE_API UDTType : public CompositeType
{
	std::string name;
	UDTType(std::string name) : name(name) {};
};

struct MODULE_API EnumType : public CompositeType
{
	std::string enum_name;
	EnumType(std::string enum_name) : enum_name(enum_name) {};
};

struct MODULE_API VariantType : public CompositeType
{
	type_vector types;
	VariantType(type_vector types) : types(types) {};
};
