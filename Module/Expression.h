#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <stack>
#include <memory>

#include "Token.h"
#include "ObjectSystem.h"

class ExpressionVisitor;

struct MODULE_API Expression
{
	virtual Object_ptr interpret(ExpressionVisitor& visitor) = 0;
};

using Expression_ptr = MODULE_API std::shared_ptr<Expression>;
using ExpressionVector = MODULE_API std::vector<Expression_ptr>;
using ExpressionStack = MODULE_API std::stack<Expression_ptr>;

struct MODULE_API StringLiteral : public Expression, public std::enable_shared_from_this<StringLiteral>
{
	std::string value;
	StringLiteral(std::string value) : value(value) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct MODULE_API NumberLiteral : public Expression, public std::enable_shared_from_this<NumberLiteral>
{
	double value;
	NumberLiteral(double value) : value(value) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct MODULE_API BooleanLiteral : public Expression, public std::enable_shared_from_this<BooleanLiteral>
{
	bool value;
	BooleanLiteral(bool value) : value(value) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct MODULE_API VectorLiteral : public Expression, public std::enable_shared_from_this<VectorLiteral>
{
	std::vector<Expression_ptr> expressions;
	VectorLiteral(std::vector<Expression_ptr> expressions) : expressions(expressions) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct MODULE_API UDTLiteral : public Expression, public std::enable_shared_from_this<UDTLiteral>
{
	std::map<std::string, Expression_ptr> pairs;
	UDTLiteral(std::map<std::string, Expression_ptr> pairs) : pairs(pairs) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct MODULE_API VectorMemberAccess : public Expression, public std::enable_shared_from_this<VectorMemberAccess>
{
	std::string name;
	Expression_ptr expression;

	VectorMemberAccess(std::string name, Expression_ptr expression) : name(name), expression(std::move(expression)) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct MODULE_API UDTMemberAccess : public Expression, public std::enable_shared_from_this<UDTMemberAccess>
{
	std::string UDT_name;
	std::string member_name;

	UDTMemberAccess(std::string UDT_name, std::string member_name) : UDT_name(UDT_name), member_name(member_name) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct MODULE_API EnumMemberAccess : public Expression, public std::enable_shared_from_this<EnumMemberAccess>
{
	std::string enum_name;
	std::string member_name;

	EnumMemberAccess(std::string enum_name, std::string member_name) : enum_name(enum_name), member_name(member_name) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct MODULE_API Identifier : public Expression, public std::enable_shared_from_this<Identifier>
{
	std::string name;
	Identifier(std::string name) : name(name) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct MODULE_API FunctionCall : public Expression, public std::enable_shared_from_this<FunctionCall>
{
	std::string name;
	std::vector<Expression_ptr> arguments;

	FunctionCall(std::string name, std::vector<Expression_ptr> arguments) : name(name), arguments(arguments) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct MODULE_API Range : public Expression, public std::enable_shared_from_this<Range>
{
	Expression_ptr left;
	Expression_ptr right;
	bool is_inclusive;

	Range(Expression_ptr left, Expression_ptr right, bool is_inclusive) : left(std::move(left)), right(std::move(right)), is_inclusive(is_inclusive) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct MODULE_API Unary : public Expression, public std::enable_shared_from_this<Unary>
{
	Token_ptr op;
	Expression_ptr operand;

	Unary(Token_ptr op, Expression_ptr operand) : op(std::move(op)), operand(std::move(operand)) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct MODULE_API Binary : public Expression, public std::enable_shared_from_this<Binary>
{
	Expression_ptr left;
	Token_ptr op;
	Expression_ptr right;

	Binary(Expression_ptr left, Token_ptr op, Expression_ptr right) : left(std::move(left)), op(std::move(op)), right(std::move(right)) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

using StringLiteral_ptr = MODULE_API std::shared_ptr<StringLiteral>;
using NumberLiteral_ptr = MODULE_API std::shared_ptr<NumberLiteral>;
using BooleanLiteral_ptr = MODULE_API std::shared_ptr<BooleanLiteral>;
using VectorLiteral_ptr = MODULE_API std::shared_ptr<VectorLiteral>;
using UDTLiteral_ptr = MODULE_API std::shared_ptr<UDTLiteral>;
using VectorMemberAccess_ptr = MODULE_API std::shared_ptr<VectorMemberAccess>;
using UDTMemberAccess_ptr = MODULE_API std::shared_ptr<UDTMemberAccess>;
using EnumMemberAccess_ptr = MODULE_API std::shared_ptr<EnumMemberAccess>;
using Identifier_ptr = MODULE_API std::shared_ptr<Identifier>;
using FunctionCall_ptr = MODULE_API std::shared_ptr<FunctionCall>;
using Range_ptr = MODULE_API std::shared_ptr<Range>;
using Unary_ptr = MODULE_API std::shared_ptr<Unary>;
using Binary_ptr = MODULE_API std::shared_ptr<Binary>;
