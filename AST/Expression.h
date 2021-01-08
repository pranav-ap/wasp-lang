#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#include "Token.h"
#include "TypeNode.h"
#include "AnnotatedNode.h"
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <memory>
#include <variant>

struct Spread;
struct TernaryCondition;
struct TypePattern;
struct UntypedAssignment;
struct TypedAssignment;
struct ListLiteral;
struct TupleLiteral;
struct SetLiteral;
struct MapLiteral;
struct Identifier;
struct Prefix;
struct Infix;
struct Postfix;
struct EnumMember;
struct Call;

using Expression = AST_API std::variant<
	std::monostate,
	int, double, std::wstring, bool,
	ListLiteral, TupleLiteral, SetLiteral, MapLiteral, Identifier, 
	UntypedAssignment, TypedAssignment, Spread, Prefix, Infix, Postfix, TypePattern, 
	TernaryCondition, EnumMember, Call
>;

using Expression_ptr = AST_API std::shared_ptr<Expression>;
using ExpressionVector = AST_API std::vector<Expression_ptr>;
using ExpressionStack = AST_API std::stack<Expression_ptr>;

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

struct AST_API UntypedAssignment : public Assignment
{
	UntypedAssignment(Expression_ptr lhs_expression, Expression_ptr rhs_expression)
		: Assignment(lhs_expression, rhs_expression) {};
};

struct AST_API TypedAssignment : public Assignment
{
	TypeNode_ptr type_node;

	TypedAssignment(Expression_ptr lhs_expression, Expression_ptr rhs_expression, TypeNode_ptr type_node)
		: Assignment(lhs_expression, rhs_expression), type_node(std::move(type_node)) {};
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

struct AST_API TernaryCondition : public AnnotatedNode
{
	Expression_ptr condition;
	Expression_ptr true_expression;
	Expression_ptr false_expression;

	TernaryCondition(Expression_ptr condition, Expression_ptr true_expression, Expression_ptr false_expression)
		: condition(std::move(condition)), 
		true_expression(std::move(true_expression)),
		false_expression(std::move(false_expression)) {};
};

struct AST_API Spread
{
	Expression_ptr expression;

	Spread(Expression_ptr expression)
		: expression(std::move(expression)) {};
};

struct AST_API TypePattern
{
	Expression_ptr expression;
	TypeNode_ptr type_node;

	TypePattern(Expression_ptr expression, TypeNode_ptr type_node)
		: expression(std::move(expression)), type_node(std::move(type_node)) {};
};

struct AST_API EnumMember
{
	std::vector<std::wstring> member_chain;
	std::wstring chain_string;

	EnumMember(std::wstring chain_string, std::vector<std::wstring> member_chain)
		: chain_string(chain_string), member_chain(member_chain) {};
};

struct AST_API Call 
{
	bool is_builtin;
	std::wstring name;
	ExpressionVector arguments;

	Call(std::wstring name) 
		: name(name), is_builtin(false) {};

	Call(std::wstring name, ExpressionVector arguments) 
		: name(name), is_builtin(false), arguments(arguments) {};
};
