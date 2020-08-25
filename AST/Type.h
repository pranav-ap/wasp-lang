#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

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

using Type = AST_API std::variant<
	std::monostate,
	NumberType, StringType, BooleanType,
	ListType, TupleType,
	UDTType, MapType,
	EnumType,
	VariantType,
	NoneType,
	AnyType
>;

using Type_ptr = AST_API std::shared_ptr<Type>;
using TypeVector = AST_API std::vector<Type_ptr>;

// Type Base

struct AST_API AnyType
{
};

struct AST_API ScalarType : public AnyType
{
};

struct AST_API CompositeType : public AnyType
{
};

struct AST_API NoneType : public AnyType
{
};

// Scalar Types

struct AST_API NumberType : public ScalarType
{
};

struct AST_API StringType : public ScalarType
{
};

struct AST_API BooleanType : public ScalarType
{
};

// Composite Types

struct AST_API ListType : public CompositeType
{
	Type_ptr element_type;
	ListType(Type_ptr element_type) : element_type(std::move(element_type)) {};
};

struct AST_API TupleType : public CompositeType
{
	TypeVector element_types;
	TupleType(TypeVector element_types) : element_types(element_types) {};
};

struct AST_API MapType : public CompositeType
{
	Type_ptr key_type;
	Type_ptr value_type;

	MapType(Type_ptr key_type, Type_ptr value_type)
		: key_type(std::move(key_type)), value_type(std::move(value_type)) {};
};

struct AST_API UDTType : public CompositeType
{
	std::wstring name;
	UDTType(std::wstring name) : name(name) {};
};

struct AST_API EnumType : public CompositeType
{
	std::wstring enum_name;
	EnumType(std::wstring enum_name) : enum_name(enum_name) {};
};

struct AST_API VariantType : public CompositeType
{
	TypeVector types;
	VariantType(TypeVector types) : types(types) {};
};
