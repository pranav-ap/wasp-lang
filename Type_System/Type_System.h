#pragma once

#define TYPESYSTEM_API __declspec(dllexport)

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
struct NoneType;

using Type = TYPESYSTEM_API std::variant<
	std::monostate,
	NumberType, StringType, BooleanType,
	ListType, TupleType,
	UDTType, MapType,
	EnumType,
	VariantType,
	NoneType,
	AnyType
>;

using Type_ptr = TYPESYSTEM_API std::shared_ptr<Type>;
using TypeVector = TYPESYSTEM_API std::vector<Type_ptr>;

// Type Base

struct TYPESYSTEM_API AnyType
{
};

struct TYPESYSTEM_API ScalarType : public AnyType
{
};

struct TYPESYSTEM_API CompositeType : public AnyType
{
};

struct TYPESYSTEM_API NoneType : public AnyType
{
};

// Scalar Types

struct TYPESYSTEM_API NumberType : public ScalarType
{
};

struct TYPESYSTEM_API StringType : public ScalarType
{
};

struct TYPESYSTEM_API BooleanType : public ScalarType
{
};

// Composite Types

struct TYPESYSTEM_API ListType : public CompositeType
{
	Type_ptr element_type;
	ListType(Type_ptr element_type) : element_type(std::move(element_type)) {};
};

struct TYPESYSTEM_API TupleType : public CompositeType
{
	TypeVector element_types;
	TupleType(TypeVector element_types) : element_types(element_types) {};
};

struct TYPESYSTEM_API MapType : public CompositeType
{
	Type_ptr key_type;
	Type_ptr value_type;

	MapType(Type_ptr key_type, Type_ptr value_type)
		: key_type(std::move(key_type)), value_type(std::move(value_type)) {};
};

struct TYPESYSTEM_API UDTType : public CompositeType
{
	std::string name;
	UDTType(std::string name) : name(name) {};
};

struct TYPESYSTEM_API EnumType : public CompositeType
{
	std::string enum_name;
	EnumType(std::string enum_name) : enum_name(enum_name) {};
};

struct TYPESYSTEM_API VariantType : public CompositeType
{
	TypeVector types;
	VariantType(TypeVector types) : types(types) {};
};
