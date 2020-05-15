#pragma once

#include "Token.h"

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <memory>
#include <variant>

struct SequenceLiteral;
struct DictionaryLiteral;
struct MemberAccess;
struct EnumMember;
struct Identifier;
struct FunctionCall;
struct Unary;
struct Binary;

using Expression = std::variant<
	std::monostate,
	double, std::string, bool,
	SequenceLiteral, DictionaryLiteral,
	MemberAccess, EnumMember,
	Identifier, FunctionCall,
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

struct SequenceLiteral : public ExpressionBase
{
	ExpressionVector expressions;
	SequenceLiteral(ExpressionVector expressions)
		: expressions(expressions) {};
};

struct DictionaryLiteral : public ExpressionBase
{
	std::map<Token_ptr, Expression_ptr> pairs;
	DictionaryLiteral(std::map<Token_ptr, Expression_ptr> pairs)
		: pairs(pairs) {};
};

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

struct FunctionCall : public ExpressionBase
{
	std::string name;
	ExpressionVector arguments;

	FunctionCall(std::string name)
		: name(name) {};
	FunctionCall(std::string name, ExpressionVector arguments)
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
