#pragma once

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
struct UDTMemberAccess;
struct EnumMember;
struct Identifier;
struct Call;
struct Unary;
struct Binary;

using Expression = std::variant<
	std::monostate,
	double, std::wstring, bool,
	ListLiteral, TupleLiteral,
	MapLiteral, EnumMember,
	Identifier, Call,
	Unary, Binary
>;

using Expression_ptr = std::shared_ptr<Expression>;
using ExpressionVector = std::vector<Expression_ptr>;
using ExpressionStack = std::stack<Expression_ptr>;

struct ExpressionBase
{
};

struct Identifier : public ExpressionBase
{
	std::wstring name;
	Identifier(std::wstring name) : name(name) {};
};

// SequenceLiteral

struct SequenceLiteral : public ExpressionBase
{
	ExpressionVector expressions;
	SequenceLiteral(ExpressionVector expressions)
		: expressions(expressions) {};
};

struct ListLiteral : public SequenceLiteral
{
	ListLiteral(ExpressionVector expressions)
		: SequenceLiteral(expressions) {};
};

struct TupleLiteral : public SequenceLiteral
{
	TupleLiteral(ExpressionVector expressions)
		: SequenceLiteral(expressions) {};
};

struct MapLiteral : public ExpressionBase
{
	std::map<Token_ptr, Expression_ptr> pairs;

	MapLiteral(std::map<Token_ptr, Expression_ptr> pairs)
		: pairs(pairs) {};
};

// Member

struct EnumMember : public ExpressionBase
{
	std::wstring enum_name;
	std::wstring member_name;

	EnumMember(std::wstring enum_name, std::wstring member_name)
		: enum_name(enum_name), member_name(member_name) {};
};

struct Call : public ExpressionBase
{
	std::wstring name;
	ExpressionVector arguments;

	Call(std::wstring name)
		: name(name) {};
	Call(std::wstring name, ExpressionVector arguments)
		: name(name), arguments(arguments) {};
};

struct Unary : public ExpressionBase
{
	Token_ptr op;
	Expression_ptr operand;

	Unary(Token_ptr op, Expression_ptr operand)
		: op(std::move(op)), operand(std::move(operand)) {};
};

struct Binary : public ExpressionBase
{
	Expression_ptr left;
	Token_ptr op;
	Expression_ptr right;

	Binary(Expression_ptr left, Token_ptr op, Expression_ptr right)
		: left(std::move(left)), op(std::move(op)), right(std::move(right)) {};
};
