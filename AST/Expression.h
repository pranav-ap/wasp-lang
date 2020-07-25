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

struct ExpressionBase
{
};

struct AST_API Identifier : public ExpressionBase
{
	std::wstring name;
	Identifier(std::wstring name) : name(name) {};
};

struct AST_API SequenceLiteral : public ExpressionBase
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

struct AST_API MapLiteral : public ExpressionBase
{
	std::map<Token_ptr, Expression_ptr> pairs;

	MapLiteral(std::map<Token_ptr, Expression_ptr> pairs)
		: pairs(pairs) {};
};

// UDT

struct AST_API UDTConstruct : public ExpressionBase
{
	std::wstring UDT_name;
	ExpressionVector expressions;

	UDTConstruct(std::wstring UDT_name, ExpressionVector expressions)
		: UDT_name(UDT_name), expressions(expressions) {};
};

struct AST_API UDTMemberAccess : public ExpressionBase
{
	ExpressionVector chain;

	UDTMemberAccess(ExpressionVector chain)
		: chain(chain) {};
};

struct AST_API EnumMember : public ExpressionBase
{
	std::wstring enum_name;
	std::wstring member_name;

	EnumMember(std::wstring enum_name, std::wstring member_name)
		: enum_name(enum_name), member_name(member_name) {};
};

struct AST_API Call : public ExpressionBase
{
	std::wstring name;
	ExpressionVector arguments;

	Call(std::wstring name)
		: name(name) {};
	Call(std::wstring name, ExpressionVector arguments)
		: name(name), arguments(arguments) {};
};

struct AST_API Unary : public ExpressionBase
{
	Token_ptr op;
	Expression_ptr operand;

	Unary(Token_ptr op, Expression_ptr operand)
		: op(std::move(op)), operand(std::move(operand)) {};
};

struct AST_API Binary : public ExpressionBase
{
	Expression_ptr left;
	Token_ptr op;
	Expression_ptr right;

	Binary(Expression_ptr left, Token_ptr op, Expression_ptr right)
		: left(std::move(left)), op(std::move(op)), right(std::move(right)) {};
};
