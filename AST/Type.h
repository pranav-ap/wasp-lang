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
#include <map>
#include <optional>

#define MAKE_TYPE(x) std::make_shared<Type>(x)

struct AnyType;
struct IntLiteralType;
struct FloatLiteralType;
struct StringLiteralType;
struct BooleanLiteralType;
struct IntType;
struct FloatType;
struct StringType;
struct BooleanType;
struct ListType;
struct TupleType;
struct SetType;
struct ClassType;
struct AliasType;
struct InterfaceType;
struct MapType;
struct EnumType;
struct VariantType;
struct NoneType;
struct FunctionType;
struct GeneratorType;
struct FunctionMemberType;
struct GeneratorMemberType;
struct OperatorType;
struct TypeIdentifier;

using Type = AST_API std::variant<
	std::monostate,
	AnyType,
	IntLiteralType, FloatLiteralType, StringLiteralType, BooleanLiteralType,
	IntType, FloatType, StringType, BooleanType,
	ListType, TupleType, SetType,
	ClassType, AliasType, InterfaceType, MapType,
	EnumType, TypeIdentifier,
	VariantType, NoneType,
	FunctionType, GeneratorType,
	FunctionMemberType, GeneratorMemberType,
	OperatorType
>;

using Type_ptr = AST_API std::shared_ptr<Type>;
using TypeVector = AST_API std::vector<Type_ptr>;
using StringVector = std::vector<std::wstring>;

// Type Base

struct AST_API AnyType
{
};

struct AST_API ScalarType : public AnyType
{
};

struct AST_API LiteralType : public AnyType
{
};

struct AST_API CompositeType : public AnyType
{
};

struct AST_API NoneType : public AnyType
{
};

struct AST_API CallableType : public AnyType
{
	TypeVector input_types;
	Type_ptr return_type;

	CallableType(TypeVector input_types, std::optional<Type_ptr> opt_return_type) : input_types(input_types)
	{
		return_type = opt_return_type.has_value() ? opt_return_type.value() : MAKE_TYPE(NoneType());
	};
};

// Parser does not know whether type identifier is class, enum or interface

struct AST_API TypeIdentifier : public AnyType
{
	std::wstring name;
	TypeIdentifier(std::wstring name) : name(name) {};
};

// Scalar Types

struct AST_API IntType : public ScalarType
{
};

struct AST_API FloatType : public ScalarType
{
};

struct AST_API StringType : public ScalarType
{
};

struct AST_API BooleanType : public ScalarType
{
};

// Literal Types

struct AST_API IntLiteralType : public LiteralType
{
	int value;
	IntLiteralType(int value) : value(value) {};
};

struct AST_API FloatLiteralType : public LiteralType
{
	double value;
	FloatLiteralType(double value) : value(value) {};
};

struct AST_API StringLiteralType : public LiteralType
{
	std::wstring value;
	StringLiteralType(std::wstring value) : value(value) {};
};

struct AST_API BooleanLiteralType : public LiteralType
{
	bool value;
	BooleanLiteralType(bool value) : value(value) {};
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

struct AST_API SetType : public CompositeType
{
	TypeVector element_types;
	SetType(TypeVector element_types) : element_types(element_types) {};
};

struct AST_API MapType : public CompositeType
{
	Type_ptr key_type;
	Type_ptr value_type;

	MapType(Type_ptr key_type, Type_ptr value_type)
		: key_type(std::move(key_type)), value_type(std::move(value_type)) {};
};

struct AST_API AliasType : public CompositeType
{
	std::wstring name;
	Type_ptr type;

	AliasType(std::wstring name, Type_ptr type)
		: name(name), type(type) {};
};

struct AST_API UserDefinedType : public CompositeType
{
	std::wstring name;

	StringVector interfaces;
	StringVector base_types;

	std::map<std::wstring, Type_ptr> members;
	std::map<std::wstring, bool> is_public_member;

	UserDefinedType(std::wstring name, StringVector interfaces, StringVector base_types, std::map<std::wstring, Type_ptr> members, std::map<std::wstring, bool> is_public_member)
		: name(name), interfaces(interfaces), base_types(base_types), members(members), is_public_member(is_public_member) {};
};

struct AST_API ClassType : public UserDefinedType
{
	ClassType(std::wstring name, StringVector interfaces, StringVector base_types, std::map<std::wstring, Type_ptr> members, std::map<std::wstring, bool> is_public_member)
		: UserDefinedType(name, interfaces, base_types, members, is_public_member) {};
};

struct AST_API InterfaceType : public UserDefinedType
{
	InterfaceType(std::wstring name, StringVector interfaces, StringVector base_types, std::map<std::wstring, Type_ptr> members, std::map<std::wstring, bool> is_public_member)
		: UserDefinedType(name, interfaces, base_types, members, is_public_member) {};
};

struct AST_API EnumType : public CompositeType
{
	std::wstring enum_name;
	std::map<std::wstring, int> members;

	EnumType(std::wstring enum_name, std::map<std::wstring, int> members)
		: enum_name(enum_name), members(members) {};
};

struct AST_API VariantType : public CompositeType
{
	TypeVector types;
	VariantType(TypeVector types) : types(types) {};
};

// Callable Type

struct AST_API FunctionType : public CallableType
{
	FunctionType(TypeVector input_types, std::optional<Type_ptr> return_type)
		: CallableType(input_types, return_type) {};
};

struct AST_API GeneratorType : public CallableType
{
	GeneratorType(TypeVector input_types, std::optional<Type_ptr> return_type)
		: CallableType(input_types, return_type) {};
};

struct AST_API FunctionMemberType : public CallableType
{
	std::wstring type_name;

	FunctionMemberType(std::wstring type_name, TypeVector input_types, std::optional<Type_ptr> return_type)
		: CallableType(input_types, return_type), type_name(type_name) {};
};

struct AST_API GeneratorMemberType : public CallableType
{
	std::wstring type_name;

	GeneratorMemberType(std::wstring type_name, TypeVector input_types, std::optional<Type_ptr> return_type)
		: CallableType(input_types, return_type), type_name(type_name) {};
};

enum class AST_API OperatorPosition
{
	Infix,
	Prefix,
	Postfix
};

struct AST_API OperatorType : public CallableType
{
	OperatorPosition position;

	OperatorType(OperatorPosition position, TypeVector input_types, std::optional<Type_ptr> return_type)
		: CallableType(input_types, return_type), position(position) {};
};

std::wstring AST_API stringify_type(Type_ptr type);
std::wstring AST_API stringify_type(std::vector<Type_ptr> types);
