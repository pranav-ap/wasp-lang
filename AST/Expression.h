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

struct IfTernaryBranch;
struct ElseTernaryBranch;
struct TypePattern;
struct LetExpression;
struct ConstExpression;
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
struct Call;
struct EnumMember;
struct Spread;
struct TypeOf;
struct Is;
struct As;

using Expression = AST_API std::variant<
	std::monostate,
	int, double, std::wstring, bool,
	ListLiteral, TupleLiteral, SetLiteral, MapLiteral, Identifier,
	LetExpression, ConstExpression,
	UntypedAssignment, TypedAssignment, 
	Prefix, Infix, Postfix, 
	TypePattern,
	IfTernaryBranch, ElseTernaryBranch,
	Call, EnumMember, Spread, TypeOf, Is, As
>;

using Expression_ptr = AST_API std::shared_ptr<Expression>;
using ExpressionVector = AST_API std::vector<Expression_ptr>;
using ExpressionStack = AST_API std::stack<Expression_ptr>;

struct AST_API Identifier
{
	std::wstring name;
	Identifier(std::wstring name) : name(name) {};
};

struct AST_API LetExpression
{
	Expression_ptr assignment;

	LetExpression(Expression_ptr assignment)
		: assignment(assignment) {};
};

struct AST_API ConstExpression
{
	Expression_ptr assignment;

	ConstExpression(Expression_ptr assignment)
		: assignment(assignment) {};
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

struct AST_API IfTernaryBranch : public AnnotatedNode
{
	Expression_ptr test;
	Expression_ptr true_expression;
	Expression_ptr alternative; // IfTernaryBranch or ElseTernaryBranch

	IfTernaryBranch(Expression_ptr test, Expression_ptr true_expression, Expression_ptr alternative)
		: true_expression(true_expression), test(test), alternative(alternative) {};
};

struct AST_API ElseTernaryBranch : public AnnotatedNode
{
	Expression_ptr expression;

	ElseTernaryBranch(Expression_ptr expression) 
		: expression(expression) {};
};

struct AST_API TypePattern
{
	Expression_ptr expression;
	TypeNode_ptr type_node;

	TypePattern(Expression_ptr expression, TypeNode_ptr type_node)
		: expression(std::move(expression)), type_node(std::move(type_node)) {};
};

struct AST_API Call
{
	bool is_builtin;
	std::wstring name;
	ExpressionVector arguments;

	Call(std::wstring name)
		: name(name), is_builtin(false) {};

	Call(std::wstring name, ExpressionVector arguments)
		: name(name), arguments(arguments), is_builtin(false) {};
};

struct AST_API EnumMember
{
	std::vector<std::wstring> chain;
	std::wstring chain_str;

	EnumMember(std::vector<std::wstring> chain, std::wstring chain_str)
		: chain(chain), chain_str(chain_str) {};
};

struct AST_API Spread
{
	bool is_rvalue;
	Expression_ptr expression;

	Spread(Expression_ptr expression)
		: expression(std::move(expression)), is_rvalue(true) {};
};

struct AST_API TypeOf
{
	std::wstring name;
	Expression_ptr expression;

	TypeOf(Expression_ptr expression)
		: name(L""), expression(std::move(expression)) {};

	TypeOf(std::wstring name, Expression_ptr expression)
		: name(name), expression(std::move(expression)) {};
};

struct AST_API Is
{
	std::wstring left_name;
	std::wstring right_name;

	Expression_ptr left;
	TypeNode_ptr right;

	Is(Expression_ptr left, TypeNode_ptr right)
		: left(left), right(right) {};
};

struct AST_API As
{
	std::wstring right_name;

	Expression_ptr left;
	TypeNode_ptr right;

	As(Expression_ptr left, TypeNode_ptr right)
		: left(left), right(right) {};
};

// Matching

struct AST_API MatchExpression : public AnnotatedNode
{
	Expression_ptr test;
	std::vector<CaseTest> case_tests;
	std::vector<Expression_ptr> case_expressions;

	MatchExpression(Expression_ptr test,
		std::vector<CaseTest> case_tests,
		std::vector<Expression_ptr> case_blocks)
		: test(test),
		case_tests(case_tests),
		case_expressions(case_expressions) {};
};

struct AST_API CaseTest
{
	Expression_ptr top_expr;
	std::wstring as_name;
	Expression_ptr where_expr;

	CaseTest(Expression_ptr top_expr,
		std::wstring as_name,
		Expression_ptr where_expr)
		: top_expr(top_expr), 
			as_name(as_name),
			where_expr(where_expr) {};
};

struct AST_API CaseExpression
{
	Expression_ptr expression;

	CaseExpression(Expression_ptr expression)
		: expression(expression) {};
};

