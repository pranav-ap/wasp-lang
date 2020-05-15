#pragma once

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

using Type = std::variant<
	std::monostate,
	NumberType, StringType, BooleanType,
	ListType, TupleType,
	UDTType, MapType,
	EnumType,
	VariantType,
	NoneType,
	AnyType
>;

using Type_ptr = std::shared_ptr<Type>;
using type_vector = std::vector<Type_ptr>;

// Type Base

struct AnyType
{
};

struct ScalarType : public AnyType
{
};

struct CompositeType : public AnyType
{
};

struct NoneType : public AnyType
{
};

// Scalar Types

struct NumberType : public ScalarType
{
};

struct StringType : public ScalarType
{
};

struct BooleanType : public ScalarType
{
};

// Composite Types

struct ListType : public CompositeType
{
	Type_ptr element_type;
	ListType(Type_ptr element_type) : element_type(std::move(element_type)) {};
};

struct TupleType : public CompositeType
{
	type_vector element_types;
	TupleType(type_vector element_types) : element_types(element_types) {};
};

struct MapType : public CompositeType
{
	Type_ptr key_type;
	Type_ptr value_type;

	MapType(Type_ptr key_type, Type_ptr value_type)
		: key_type(std::move(key_type)), value_type(std::move(value_type)) {};
};

struct UDTType : public CompositeType
{
	std::string name;
	UDTType(std::string name) : name(name) {};
};

struct EnumType : public CompositeType
{
	std::string enum_name;
	EnumType(std::string enum_name) : enum_name(enum_name) {};
};

struct VariantType : public CompositeType
{
	type_vector types;
	VariantType(type_vector types) : types(types) {};
};
