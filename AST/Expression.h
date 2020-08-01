#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#include "Token.h"

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <memory>
#include <variant>

struct ListLiteral;
struct TupleLiteral;
struct MapLiteral;
struct UDTConstruct;
struct UDTMemberAccess;
struct EnumMember;
struct Identifier;
struct Call;
struct Unary;
struct Binary;

using Expression = AST_API std::variant<
	std::monostate,
	double, std::wstring, bool,
	ListLiteral, TupleLiteral,
	MapLiteral, UDTConstruct,
	UDTMemberAccess, EnumMember,
	Identifier, Call,
	Unary, Binary
>;

using Expression_ptr = AST_API std::shared_ptr<Expression>;
using ExpressionVector = AST_API std::vector<Expression_ptr>;
using ExpressionStack = AST_API std::stack<Expression_ptr>;

struct AST_API Identifier
{
	std::wstring name;
	Identifier(std::wstring name) : name(name) {};
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

struct AST_API MapLiteral
{
	std::map<Expression_ptr, Expression_ptr> pairs;

	MapLiteral(std::map<Expression_ptr, Expression_ptr> pairs)
		: pairs(pairs) {};
};

// UDT

struct AST_API UDTConstruct
{
	std::wstring UDT_name;
	ExpressionVector expressions;

	UDTConstruct(std::wstring UDT_name, ExpressionVector expressions)
		: UDT_name(UDT_name), expressions(expressions) {};
};

struct AST_API UDTMemberAccess
{
	ExpressionVector chain;

	UDTMemberAccess(ExpressionVector chain)
		: chain(chain) {};
};

struct AST_API EnumMember
{
	std::wstring enum_name;
	std::vector<std::wstring> member_chain;

	EnumMember(std::wstring enum_name, std::vector<std::wstring> member_chain)
		: enum_name(enum_name), member_chain(member_chain) {};
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

struct AST_API Unary
{
	Token_ptr op;
	Expression_ptr operand;

	Unary(Token_ptr op, Expression_ptr operand)
		: op(std::move(op)), operand(std::move(operand)) {};
};

struct AST_API Binary
{
	Expression_ptr left;
	Token_ptr op;
	Expression_ptr right;

	Binary(Expression_ptr left, Token_ptr op, Expression_ptr right)
		: left(std::move(left)), op(std::move(op)), right(std::move(right)) {};
};
