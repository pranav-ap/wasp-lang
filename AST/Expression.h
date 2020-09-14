#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#include "Token.h"
#include "Type.h"

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <memory>
#include <variant>

struct VariableDefinitionExpression;
struct TernaryCondition;
struct SpreadExpression;
struct TypePattern;
struct Assignment;
struct ListLiteral;
struct TupleLiteral;
struct SetLiteral;
struct MapLiteral;
struct UDTConstruct;
struct EnumMember;
struct Identifier;
struct Call;
struct Prefix;
struct Infix;
struct Postfix;

using Expression = AST_API std::variant<
	std::monostate,
	int, double, std::wstring, bool,
	ListLiteral, TupleLiteral, SetLiteral,
	MapLiteral, UDTConstruct, EnumMember,
	Identifier, Call,
	Assignment,
	Prefix, Infix, Postfix,
	TypePattern, SpreadExpression,
	TernaryCondition,
	VariableDefinitionExpression
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

struct AST_API UDTConstruct
{
	std::wstring UDT_name;
	ExpressionVector expressions;

	UDTConstruct(std::wstring UDT_name, ExpressionVector expressions)
		: UDT_name(UDT_name), expressions(expressions) {};
};

struct AST_API EnumMember
{
	std::vector<std::wstring> member_chain;

	EnumMember(std::vector<std::wstring> member_chain)
		: member_chain(member_chain) {};
};

struct AST_API Call
{
	std::wstring name;
	ExpressionVector arguments;

	Call(std::wstring name)
		: name(name) {};
	Call(std::wstring name, ExpressionVector arguments)
		: name(name), arguments(arguments) {};
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

struct AST_API TypePattern
{
	Expression_ptr expression;
	Type_ptr type;

	TypePattern(Expression_ptr expression, Type_ptr type)
		: expression(std::move(expression)), type(std::move(type)) {};
};

struct AST_API SpreadExpression
{
	Expression_ptr expression;

	SpreadExpression(Expression_ptr expression)
		: expression(std::move(expression)) {};
};

struct AST_API TernaryCondition
{
	Expression_ptr condition;
	Expression_ptr true_expression;
	std::optional<Expression_ptr> false_expression;

	TernaryCondition(Expression_ptr condition, Expression_ptr true_expression)
		: condition(std::move(condition)), true_expression(std::move(true_expression)), false_expression(std::nullopt) {};

	TernaryCondition(Expression_ptr condition, Expression_ptr true_expression, Expression_ptr false_expression)
		: condition(std::move(condition)), true_expression(std::move(true_expression)), false_expression(std::make_optional(std::move(false_expression))) {};
};

struct AST_API VariableDefinitionExpression
{
	bool is_mutable;
	Expression_ptr expression;

	VariableDefinitionExpression(bool is_mutable, Expression_ptr expression)
		: is_mutable(is_mutable), expression(std::move(expression)) {};
};
