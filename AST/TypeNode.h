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

struct AnyTypeNode;
struct IntLiteralTypeNode;
struct FloatLiteralTypeNode;
struct StringLiteralTypeNode;
struct BooleanLiteralTypeNode;
struct IntTypeNode;
struct FloatTypeNode;
struct StringTypeNode;
struct BooleanTypeNode;
struct ListTypeNode;
struct TupleTypeNode;
struct SetTypeNode;
struct ClassTypeNode;
struct AliasTypeNode;
struct InterfaceTypeNode;
struct MapTypeNode;
struct EnumTypeNode;
struct VariantTypeNode;
struct NoneTypeNode;
struct FunctionTypeNode;
struct GeneratorTypeNode;
struct FunctionMemberTypeNode;
struct GeneratorMemberTypeNode;
struct OperatorTypeNode;
struct TypeIdentifierNode;

using TypeNode = AST_API std::variant<
	std::monostate,
	AnyTypeNode,
	IntLiteralTypeNode, FloatLiteralTypeNode, StringLiteralTypeNode, BooleanLiteralTypeNode,
	IntTypeNode, FloatTypeNode, StringTypeNode, BooleanTypeNode,
	ListTypeNode, TupleTypeNode, SetTypeNode,
	ClassTypeNode, AliasTypeNode, InterfaceTypeNode, MapTypeNode,
	EnumTypeNode, TypeIdentifierNode,
	VariantTypeNode, NoneTypeNode,
	FunctionTypeNode, GeneratorTypeNode,
	FunctionMemberTypeNode, GeneratorMemberTypeNode,
	OperatorTypeNode
>;

using TypeNode_ptr = AST_API std::shared_ptr<TypeNode>;
using TypeNodeVector = AST_API std::vector<TypeNode_ptr>;
using StringVector = std::vector<std::wstring>;

// TypeNode Base

struct AST_API AnyTypeNode
{
};

struct AST_API ScalarTypeNode : public AnyTypeNode
{
};

struct AST_API LiteralTypeNode : public AnyTypeNode
{
};

struct AST_API CompositeTypeNode : public AnyTypeNode
{
};

struct AST_API NoneTypeNode : public AnyTypeNode
{
};

struct AST_API CallableTypeNode : public AnyTypeNode
{
	TypeNodeVector input_types;
	std::optional<TypeNode_ptr> return_type;

	CallableTypeNode(TypeNodeVector input_types, std::optional<TypeNode_ptr> return_type)
		: input_types(input_types), return_type(return_type) {};
};

// Parser does not know whether type identifier is class, enum or interface

struct AST_API TypeIdentifierNode : public AnyTypeNode
{
	std::wstring name;
	TypeIdentifierNode(std::wstring name) : name(name) {};
};

// Scalar Types

struct AST_API IntTypeNode : public ScalarTypeNode
{
};

struct AST_API FloatTypeNode : public ScalarTypeNode
{
};

struct AST_API StringTypeNode : public ScalarTypeNode
{
};

struct AST_API BooleanTypeNode : public ScalarTypeNode
{
};

// Literal Types

struct AST_API IntLiteralTypeNode : public LiteralTypeNode
{
	int value;
	IntLiteralTypeNode(int value) : value(value) {};
};

struct AST_API FloatLiteralTypeNode : public LiteralTypeNode
{
	double value;
	FloatLiteralTypeNode(double value) : value(value) {};
};

struct AST_API StringLiteralTypeNode : public LiteralTypeNode
{
	std::wstring value;
	StringLiteralTypeNode(std::wstring value) : value(value) {};
};

struct AST_API BooleanLiteralTypeNode : public LiteralTypeNode
{
	bool value;
	BooleanLiteralTypeNode(bool value) : value(value) {};
};

// Composite Types

struct AST_API ListTypeNode : public CompositeTypeNode
{
	TypeNode_ptr element_type;
	ListTypeNode(TypeNode_ptr element_type) : element_type(std::move(element_type)) {};
};

struct AST_API TupleTypeNode : public CompositeTypeNode
{
	TypeNodeVector element_types;
	TupleTypeNode(TypeNodeVector element_types) : element_types(element_types) {};
};

struct AST_API SetTypeNode : public CompositeTypeNode
{
	TypeNodeVector element_types;
	SetTypeNode(TypeNodeVector element_types) : element_types(element_types) {};
};

struct AST_API MapTypeNode : public CompositeTypeNode
{
	TypeNode_ptr key_type;
	TypeNode_ptr value_type;

	MapTypeNode(TypeNode_ptr key_type, TypeNode_ptr value_type)
		: key_type(std::move(key_type)), value_type(std::move(value_type)) {};
};

struct AST_API AliasTypeNode : public CompositeTypeNode
{
	std::wstring name;
	TypeNode_ptr type;

	AliasTypeNode(std::wstring name, TypeNode_ptr type)
		: name(name), type(type) {};
};

struct AST_API UserDefinedTypeNode : public CompositeTypeNode
{
	std::wstring name;

	StringVector interfaces;
	StringVector base_types;

	std::map<std::wstring, TypeNode_ptr> members;
	std::map<std::wstring, bool> is_public_member;

	UserDefinedTypeNode(std::wstring name, StringVector interfaces, StringVector base_types, std::map<std::wstring, TypeNode_ptr> members, std::map<std::wstring, bool> is_public_member)
		: name(name), interfaces(interfaces), base_types(base_types), members(members), is_public_member(is_public_member) {};
};

struct AST_API ClassTypeNode : public UserDefinedTypeNode
{
	ClassTypeNode(std::wstring name, StringVector interfaces, StringVector base_types, std::map<std::wstring, TypeNode_ptr> members, std::map<std::wstring, bool> is_public_member)
		: UserDefinedTypeNode(name, interfaces, base_types, members, is_public_member) {};
};

struct AST_API InterfaceTypeNode : public UserDefinedTypeNode
{
	InterfaceTypeNode(std::wstring name, StringVector interfaces, StringVector base_types, std::map<std::wstring, TypeNode_ptr> members, std::map<std::wstring, bool> is_public_member)
		: UserDefinedTypeNode(name, interfaces, base_types, members, is_public_member) {};
};

struct AST_API EnumTypeNode : public CompositeTypeNode
{
	std::wstring enum_name;
	std::map<std::wstring, int> members;

	EnumTypeNode(std::wstring enum_name, std::map<std::wstring, int> members)
		: enum_name(enum_name), members(members) {};
};

struct AST_API VariantTypeNode : public CompositeTypeNode
{
	TypeNodeVector types;
	VariantTypeNode(TypeNodeVector types) : types(types) {};
};

// Callable TypeNode

struct AST_API FunctionTypeNode : public CallableTypeNode
{
	FunctionTypeNode(TypeNodeVector input_types, std::optional<TypeNode_ptr> return_type)
		: CallableTypeNode(input_types, return_type) {};
};

struct AST_API GeneratorTypeNode : public CallableTypeNode
{
	GeneratorTypeNode(TypeNodeVector input_types, std::optional<TypeNode_ptr> return_type)
		: CallableTypeNode(input_types, return_type) {};
};

struct AST_API FunctionMemberTypeNode : public CallableTypeNode
{
	std::wstring type_name;

	FunctionMemberTypeNode(std::wstring type_name, TypeNodeVector input_types, std::optional<TypeNode_ptr> return_type)
		: CallableTypeNode(input_types, return_type), type_name(type_name) {};
};

struct AST_API GeneratorMemberTypeNode : public CallableTypeNode
{
	std::wstring type_name;

	GeneratorMemberTypeNode(std::wstring type_name, TypeNodeVector input_types, std::optional<TypeNode_ptr> return_type)
		: CallableTypeNode(input_types, return_type), type_name(type_name) {};
};

enum class AST_API OperatorPositionNode
{
	Infix,
	Prefix,
	Postfix
};

struct AST_API OperatorTypeNode : public CallableTypeNode
{
	OperatorPositionNode position;

	OperatorTypeNode(OperatorPositionNode position, TypeNodeVector input_types, std::optional<TypeNode_ptr> return_type)
		: CallableTypeNode(input_types, return_type), position(position) {};
};
