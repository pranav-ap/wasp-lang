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
struct UDTLiteral;
struct MemberAccess;
struct EnumMember;
struct Identifier;
struct Call;
struct Unary;
struct Binary;

using Expression = std::variant<
	std::monostate,
	double, std::string, bool,
	ListLiteral, TupleLiteral,
	MapLiteral, UDTLiteral,
	MemberAccess, EnumMember,
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
	std::string name;
	Identifier(std::string name) : name(name) {};
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

// DictionaryLiteral

struct DictionaryLiteral : public ExpressionBase
{
	std::map<Token_ptr, Expression_ptr> pairs;
	DictionaryLiteral(std::map<Token_ptr, Expression_ptr> pairs)
		: pairs(pairs) {};
};

struct MapLiteral : public DictionaryLiteral
{
	MapLiteral(std::map<Token_ptr, Expression_ptr> pairs)
		: DictionaryLiteral(pairs) {};
};

struct UDTLiteral : public DictionaryLiteral
{
	UDTLiteral(std::map<Token_ptr, Expression_ptr> pairs)
		: DictionaryLiteral(pairs) {};
};

// Member

struct MemberAccess : public ExpressionBase
{
	Expression_ptr container;
	Expression_ptr access_expression;

	MemberAccess(Expression_ptr container, Expression_ptr access_expression)
		: container(std::move(container)), access_expression(std::move(access_expression)) {};
};

struct EnumMember : public ExpressionBase
{
	std::string enum_name;
	std::string member_name;

	EnumMember(std::string enum_name, std::string member_name)
		: enum_name(enum_name), member_name(member_name) {};
};

struct Call : public ExpressionBase
{
	std::string name;
	ExpressionVector arguments;

	Call(std::string name)
		: name(name) {};
	Call(std::string name, ExpressionVector arguments)
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
