#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

#include "Token.h"
#include "ObjectSystem.h"

#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <variant>

struct VectorLiteral;
struct DictionaryLiteral;
struct MemberAccess;
struct Identifier;
struct FunctionCall;
struct Unary;
struct Binary;

using Expression = MODULE_API std::variant<
	std::monostate,
	double, std::string, bool,
	VectorLiteral, DictionaryLiteral,
	MemberAccess,
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

struct MODULE_API VectorLiteral : public ExpressionBase
{
	ExpressionVector expressions;
	VectorLiteral() {};
	VectorLiteral(std::vector<Expression_ptr> expressions)
		: expressions(expressions) {};
};

struct MODULE_API DictionaryLiteral : public ExpressionBase
{
	std::map<Token_ptr, Expression_ptr> pairs;
	DictionaryLiteral() {};
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

struct MODULE_API FunctionCall : public ExpressionBase
{
	std::string name;
	ExpressionVector arguments;

	FunctionCall(std::string name)
		: name(name) {};
	FunctionCall(std::string name, std::vector<Expression_ptr> arguments)
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
