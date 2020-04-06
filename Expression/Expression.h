#pragma once

#ifdef EXPRESSION_EXPORTS
#define EXPRESSION_API __declspec(dllexport)
#else
#define EXPRESSION_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <stack>
#include <memory>

#include "Token.h"
#include "ObjectSystem.h"

class ExpressionVisitor;

struct EXPRESSION_API Expression
{
	virtual Object_ptr interpret(ExpressionVisitor& visitor) = 0;
};

using Expression_ptr = EXPRESSION_API std::shared_ptr<Expression>;
using ExpressionVector = EXPRESSION_API std::vector<Expression_ptr>;
using ExpressionStack = EXPRESSION_API std::stack<Expression_ptr>;

struct EXPRESSION_API StringLiteral : public Expression, public std::enable_shared_from_this<StringLiteral>
{
	std::string value;
	StringLiteral(std::string value) : value(value) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct EXPRESSION_API NumberLiteral : public Expression, public std::enable_shared_from_this<NumberLiteral>
{
	double value;
	NumberLiteral(double value) : value(value) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct EXPRESSION_API BooleanLiteral : public Expression, public std::enable_shared_from_this<BooleanLiteral>
{
	bool value;
	BooleanLiteral(bool value) : value(value) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct EXPRESSION_API VectorLiteral : public Expression, public std::enable_shared_from_this<VectorLiteral>
{
	std::vector<Expression_ptr> expressions;
	VectorLiteral(std::vector<Expression_ptr> expressions) : expressions(expressions) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct EXPRESSION_API UDTLiteral : public Expression, public std::enable_shared_from_this<UDTLiteral>
{
	std::map<std::string, Expression_ptr> pairs;
	UDTLiteral(std::map<std::string, Expression_ptr> pairs) : pairs(pairs) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct EXPRESSION_API VectorMemberAccess : public Expression, public std::enable_shared_from_this<VectorMemberAccess>
{
	std::string name;
	Expression_ptr expression;

	VectorMemberAccess(std::string name, Expression_ptr expression) : name(name), expression(std::move(expression)) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct EXPRESSION_API UDTMemberAccess : public Expression, public std::enable_shared_from_this<UDTMemberAccess>
{
	std::string UDT_name;
	std::string member_name;

	UDTMemberAccess(std::string UDT_name, std::string member_name) : UDT_name(UDT_name), member_name(member_name) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct EXPRESSION_API EnumMemberAccess : public Expression, public std::enable_shared_from_this<EnumMemberAccess>
{
	std::string enum_name;
	std::string member_name;

	EnumMemberAccess(std::string enum_name, std::string member_name) : enum_name(enum_name), member_name(member_name) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct EXPRESSION_API Identifier : public Expression, public std::enable_shared_from_this<Identifier>
{
	std::string name;
	Identifier(std::string name) : name(name) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct EXPRESSION_API FunctionCall : public Expression, public std::enable_shared_from_this<FunctionCall>
{
	std::string name;
	std::vector<Expression_ptr> arguments;

	FunctionCall(std::string name, std::vector<Expression_ptr> arguments) : name(name), arguments(arguments) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct EXPRESSION_API Range : public Expression, public std::enable_shared_from_this<Range>
{
	Expression_ptr left;
	Expression_ptr right;
	bool is_inclusive;

	Range(Expression_ptr left, Expression_ptr right, bool is_inclusive) : left(std::move(left)), right(std::move(right)), is_inclusive(is_inclusive) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct EXPRESSION_API Unary : public Expression, public std::enable_shared_from_this<Unary>
{
	Token_ptr op;
	Expression_ptr operand;

	Unary(Token_ptr op, Expression_ptr operand) : op(std::move(op)), operand(std::move(operand)) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

struct EXPRESSION_API Binary : public Expression, public std::enable_shared_from_this<Binary>
{
	Expression_ptr left;
	Token_ptr op;
	Expression_ptr right;

	Binary(Expression_ptr left, Token_ptr op, Expression_ptr right) : left(std::move(left)), op(std::move(op)), right(std::move(right)) {};
	Object_ptr interpret(ExpressionVisitor& visitor);
};

using StringLiteral_ptr = EXPRESSION_API std::shared_ptr<StringLiteral>;
using NumberLiteral_ptr = EXPRESSION_API std::shared_ptr<NumberLiteral>;
using BooleanLiteral_ptr = EXPRESSION_API std::shared_ptr<BooleanLiteral>;
using VectorLiteral_ptr = EXPRESSION_API std::shared_ptr<VectorLiteral>;
using UDTLiteral_ptr = EXPRESSION_API std::shared_ptr<UDTLiteral>;
using VectorMemberAccess_ptr = EXPRESSION_API std::shared_ptr<VectorMemberAccess>;
using UDTMemberAccess_ptr = EXPRESSION_API std::shared_ptr<UDTMemberAccess>;
using EnumMemberAccess_ptr = EXPRESSION_API std::shared_ptr<EnumMemberAccess>;
using Identifier_ptr = EXPRESSION_API std::shared_ptr<Identifier>;
using FunctionCall_ptr = EXPRESSION_API std::shared_ptr<FunctionCall>;
using Range_ptr = EXPRESSION_API std::shared_ptr<Range>;
using Unary_ptr = EXPRESSION_API std::shared_ptr<Unary>;
using Binary_ptr = EXPRESSION_API std::shared_ptr<Binary>;

// Printers

EXPRESSION_API std::ostream& operator<<(std::ostream& os, const StringLiteral_ptr exp);
EXPRESSION_API std::ostream& operator<<(std::ostream& os, const NumberLiteral_ptr exp);
EXPRESSION_API std::ostream& operator<<(std::ostream& os, const BooleanLiteral_ptr exp);
EXPRESSION_API std::ostream& operator<<(std::ostream& os, const VectorLiteral_ptr exp);
EXPRESSION_API std::ostream& operator<<(std::ostream& os, const UDTLiteral_ptr exp);
EXPRESSION_API std::ostream& operator<<(std::ostream& os, const Identifier_ptr exp);
EXPRESSION_API std::ostream& operator<<(std::ostream& os, const Unary_ptr exp);
EXPRESSION_API std::ostream& operator<<(std::ostream& os, const Binary_ptr exp);
EXPRESSION_API std::ostream& operator<<(std::ostream& os, const VectorMemberAccess_ptr exp);
EXPRESSION_API std::ostream& operator<<(std::ostream& os, const UDTMemberAccess_ptr exp);
EXPRESSION_API std::ostream& operator<<(std::ostream& os, const EnumMemberAccess_ptr exp);
EXPRESSION_API std::ostream& operator<<(std::ostream& os, const FunctionCall_ptr exp);
EXPRESSION_API std::ostream& operator<<(std::ostream& os, const Range_ptr exp);
