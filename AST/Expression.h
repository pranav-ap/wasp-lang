#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#include "Token.h"
#include "AnnotatedNode.h"
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <memory>
#include <variant>

struct Spread;
struct TernaryCondition;
struct TagPattern;
struct Assignment;
struct ListLiteral;
struct TupleLiteral;
struct SetLiteral;
struct MapLiteral;
struct NewObject;
struct EnumMember;
struct Identifier;
struct Call;
struct Prefix;
struct Infix;
struct Postfix;
struct MemberAccess;

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
struct MapTypeNode;
struct EnumTypeNode;
struct VariantTypeNode;
struct NoneTypeNode;
struct FunctionTypeNode;
struct GeneratorTypeNode;
struct FunctionMemberTypeNode;
struct GeneratorMemberTypeNode;
struct TypeIdentifierTypeNode;

using Expression = AST_API std::variant<
	std::monostate,
	int, double, std::wstring, bool,
	ListLiteral, TupleLiteral, SetLiteral,
	MapLiteral, NewObject, EnumMember,
	Identifier, Call,
	Assignment, 
	Spread,	Prefix, Infix, Postfix,
	TagPattern, TernaryCondition,
	MemberAccess,

	AnyTypeNode,
	IntLiteralTypeNode, FloatLiteralTypeNode, StringLiteralTypeNode, BooleanLiteralTypeNode,
	IntTypeNode, FloatTypeNode, StringTypeNode, BooleanTypeNode,
	ListTypeNode, TupleTypeNode, SetTypeNode, MapTypeNode,
	TypeIdentifierTypeNode,
	VariantTypeNode, NoneTypeNode,
	FunctionTypeNode, GeneratorTypeNode,
	FunctionMemberTypeNode, GeneratorMemberTypeNode
>;

using Expression_ptr = AST_API std::shared_ptr<Expression>;
using ExpressionVector = AST_API std::vector<Expression_ptr>;
using ExpressionStack = AST_API std::stack<Expression_ptr>;
using StringVector = std::vector<std::wstring>;

struct AST_API Identifier
{
	std::wstring name;
	Identifier(std::wstring name) : name(name) {};
};

struct AST_API Assignment
{
	Expression_ptr lhs_expression;
	Expression_ptr rhs_expression;

	Assignment(Expression_ptr lhs_expression, Expression_ptr rhs_expression)
		: lhs_expression(lhs_expression), rhs_expression(rhs_expression) {};
};

struct AST_API SequenceLiteral
{
	ExpressionVector expressions;
	SequenceLiteral(ExpressionVector expressions)
		: expressions(expressions) {};
};

struct AST_API ListLiteral : public SequenceLiteral
{
	ListLiteral(ExpressionVector expressions)
		: SequenceLiteral(expressions) {};
};

struct AST_API TupleLiteral : public SequenceLiteral
{
	TupleLiteral(ExpressionVector expressions)
		: SequenceLiteral(expressions) {};
};

struct AST_API SetLiteral : public SequenceLiteral
{
	SetLiteral(ExpressionVector expressions)
		: SequenceLiteral(expressions) {};
};

struct AST_API MapLiteral
{
	std::map<Expression_ptr, Expression_ptr> pairs;

	MapLiteral(std::map<Expression_ptr, Expression_ptr> pairs)
		: pairs(pairs) {};
};

struct AST_API NewObject
{
	std::wstring type_name;
	ExpressionVector expressions;

	NewObject(std::wstring type_name, ExpressionVector expressions)
		: type_name(type_name), expressions(expressions) {};
};

struct AST_API EnumMember
{
	std::vector<std::wstring> member_chain;

	EnumMember(std::vector<std::wstring> member_chain)
		: member_chain(member_chain) {};
};

struct AST_API MemberAccess
{
	Expression_ptr left;
	Token_ptr op;
	Expression_ptr right;

	MemberAccess(Expression_ptr left, Token_ptr op, Expression_ptr right)
		: left(std::move(left)), op(std::move(op)), right(std::move(right)) {};
};

struct AST_API Call
{
	std::wstring name;
	ExpressionVector arguments;

	Call(std::wstring name) : name(name) {};
	Call(std::wstring name, ExpressionVector arguments) : name(name), arguments(arguments) {};
};

struct AST_API Prefix
{
	Token_ptr op;
	Expression_ptr operand;

	Prefix(Token_ptr op, Expression_ptr operand)
		: op(std::move(op)), operand(std::move(operand)) {};
};

struct AST_API Infix
{
	Expression_ptr left;
	Token_ptr op;
	Expression_ptr right;

	Infix(Expression_ptr left, Token_ptr op, Expression_ptr right)
		: left(std::move(left)), op(std::move(op)), right(std::move(right)) {};
};

struct AST_API Postfix
{
	Token_ptr op;
	Expression_ptr operand;

	Postfix(Expression_ptr operand, Token_ptr op)
		: operand(std::move(operand)), op(std::move(op)) {};
};

struct AST_API TagPattern
{
	Expression_ptr expression;
	Expression_ptr tag;

	TagPattern(Expression_ptr expression, Expression_ptr tag)
		: expression(std::move(expression)), tag(std::move(tag)) {};
};

struct AST_API TernaryCondition : public AnnotatedNode
{
	Expression_ptr condition;
	Expression_ptr true_expression;
	Expression_ptr false_expression;

	TernaryCondition(Expression_ptr condition, Expression_ptr true_expression, Expression_ptr false_expression)
		: condition(std::move(std::make_shared<Expression>(condition))), 
		true_expression(std::move(std::make_shared<Expression>(true_expression))),
		false_expression(std::move(std::make_shared<Expression>(false_expression))) {};
};

struct AST_API Spread
{
	Expression_ptr expression;

	Spread(Expression_ptr expression)
		: expression(std::move(expression)) {};
};


// Type nodes

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
	ExpressionVector input_types;
	std::optional<Expression_ptr> return_type;

	CallableTypeNode(ExpressionVector input_types, std::optional<Expression_ptr> return_type)
		: input_types(input_types), return_type(return_type) {};
};

// Parser does not know whether type identifier is class, enum or interface

struct AST_API TypeIdentifierTypeNode : public AnyTypeNode
{
	std::wstring name;
	TypeIdentifierTypeNode(std::wstring name) : name(name) {};
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
	Expression_ptr element_type;
	ListTypeNode(Expression_ptr element_type) : element_type(std::move(element_type)) {};
};

struct AST_API TupleTypeNode : public CompositeTypeNode
{
	ExpressionVector element_types;
	TupleTypeNode(ExpressionVector element_types) : element_types(element_types) {};
};

struct AST_API SetTypeNode : public CompositeTypeNode
{
	ExpressionVector element_types;
	SetTypeNode(ExpressionVector element_types) : element_types(element_types) {};
};

struct AST_API MapTypeNode : public CompositeTypeNode
{
	Expression_ptr key_type;
	Expression_ptr value_type;

	MapTypeNode(Expression_ptr key_type, Expression_ptr value_type)
		: key_type(std::move(key_type)), value_type(std::move(value_type)) {};
};

struct AST_API UserDefinedTypeNode : public CompositeTypeNode
{
	std::wstring name;

	StringVector interfaces;
	StringVector base_types;

	std::map<std::wstring, Expression_ptr> members;
	std::map<std::wstring, bool> is_public_member;

	UserDefinedTypeNode(std::wstring name, StringVector interfaces, StringVector base_types, std::map<std::wstring, Expression_ptr> members, std::map<std::wstring, bool> is_public_member)
		: name(name), interfaces(interfaces), base_types(base_types), members(members), is_public_member(is_public_member) {};
};

struct AST_API VariantTypeNode : public CompositeTypeNode
{
	ExpressionVector types;
	VariantTypeNode(ExpressionVector types) : types(types) {};
};

// Callable Expression

struct AST_API FunctionTypeNode : public CallableTypeNode
{
	FunctionTypeNode(ExpressionVector input_types, std::optional<Expression_ptr> return_type)
		: CallableTypeNode(input_types, return_type) {};
};

struct AST_API GeneratorTypeNode : public CallableTypeNode
{
	GeneratorTypeNode(ExpressionVector input_types, std::optional<Expression_ptr> return_type)
		: CallableTypeNode(input_types, return_type) {};
};

struct AST_API FunctionMemberTypeNode : public CallableTypeNode
{
	std::wstring type_name;

	FunctionMemberTypeNode(std::wstring type_name, ExpressionVector input_types, std::optional<Expression_ptr> return_type)
		: CallableTypeNode(input_types, return_type), type_name(type_name) {};
};

struct AST_API GeneratorMemberTypeNode : public CallableTypeNode
{
	std::wstring type_name;

	GeneratorMemberTypeNode(std::wstring type_name, ExpressionVector input_types, std::optional<Expression_ptr> return_type)
		: CallableTypeNode(input_types, return_type), type_name(type_name) {};
};