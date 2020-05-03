#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
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
struct DictionaryLiteral;
struct MemberAccess;
struct EnumMember;
struct Identifier;
struct FunctionCall;
struct Unary;
struct Binary;

using Expression = MODULE_API std::variant<
	std::monostate,
	double, std::string, bool,
	ListLiteral, TupleLiteral,
	DictionaryLiteral,
	MemberAccess, EnumMember,
	Identifier, FunctionCall,
	Unary, Binary
>;

using Expression_ptr = MODULE_API std::shared_ptr<Expression>;
using ExpressionVector = MODULE_API std::vector<Expression_ptr>;
using ExpressionStack = MODULE_API std::stack<Expression_ptr>;

struct MODULE_API ExpressionBase
{
};

struct MODULE_API Identifier : public ExpressionBase
{
	std::string name;
	Identifier(std::string name) : name(name) {};
};

struct MODULE_API ListLiteral : public ExpressionBase
{
	ExpressionVector expressions;
	ListLiteral(ExpressionVector expressions)
		: expressions(expressions) {};
};

struct MODULE_API TupleLiteral : public ExpressionBase
{
	ExpressionVector expressions;
	TupleLiteral(ExpressionVector expressions)
		: expressions(expressions) {};
};

struct MODULE_API DictionaryLiteral : public ExpressionBase
{
	std::map<Token_ptr, Expression_ptr> pairs;
	DictionaryLiteral(std::map<Token_ptr, Expression_ptr> pairs)
		: pairs(pairs) {};
};

struct MODULE_API MemberAccess : public ExpressionBase
{
	std::string name;
	Expression_ptr expression;

	MemberAccess(std::string name, Expression_ptr expression)
		: name(name), expression(std::move(expression)) {};
};

struct MODULE_API EnumMember : public ExpressionBase
{
	std::string enum_name;
	std::string member_name;

	EnumMember(std::string enum_name, std::string member_name)
		: enum_name(enum_name), member_name(member_name) {};
};

struct MODULE_API FunctionCall : public ExpressionBase
{
	std::string name;
	ExpressionVector arguments;

	FunctionCall(std::string name)
		: name(name) {};
	FunctionCall(std::string name, ExpressionVector arguments)
		: name(name), arguments(arguments) {};
};

struct MODULE_API Unary : public ExpressionBase
{
	Token_ptr op;
	Expression_ptr operand;

	Unary(Token_ptr op, Expression_ptr operand)
		: op(std::move(op)), operand(std::move(operand)) {};
};

struct MODULE_API Binary : public ExpressionBase
{
	Expression_ptr left;
	Token_ptr op;
	Expression_ptr right;

	Binary(Expression_ptr left, Token_ptr op, Expression_ptr right)
		: left(std::move(left)), op(std::move(op)), right(std::move(right)) {};
};
