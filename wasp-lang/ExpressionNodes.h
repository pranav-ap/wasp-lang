#pragma once
#include <string>
#include <vector>
#include <memory>

#include "Token.h"
#include "Types.h"

class Expression
{
public:
	virtual void print(int level) = 0;
};

using Expression_ptr = std::shared_ptr<Expression>;

class StringLiteral : public Expression
{
	std::string value;

public:
	StringLiteral(std::string value) : value(value) {};
	void print(int level);
};

class NumberLiteral : public Expression
{
	double value;

public:
	NumberLiteral(double value) : value(value) {};
	void print(int level);
};

class BooleanLiteral : public Expression
{
	bool value;

public:
	BooleanLiteral(bool value) : value(value) {};
	void print(int level);
};

class TupleLiteral : public Expression
{
	std::vector<Expression_ptr> expressions;

public:
	TupleLiteral(std::vector<Expression_ptr> expressions) : expressions(expressions) {};
	void print(int level);
};

class VectorLiteral : public Expression
{
	std::vector<Expression_ptr> expressions;

public:
	VectorLiteral(std::vector<Expression_ptr> expressions) : expressions(expressions) {};
	void print(int level);
};

class MapLiteral : public Expression
{
	std::vector<std::pair<Expression_ptr, Expression_ptr>> pairs;

public:
	MapLiteral(std::vector<std::pair<Expression_ptr, Expression_ptr>> pairs) : pairs(pairs) {};
	void print(int level);
};

class RecordLiteral : public Expression
{
	std::vector<std::pair<std::string, Expression_ptr>> pairs;

public:
	RecordLiteral(std::vector<std::pair<std::string, Expression_ptr>> pairs) : pairs(pairs) {};
	void print(int level);
};

class MemberAccess : public Expression
{
	std::string name;
	Expression_ptr expression;

public:
	MemberAccess(std::string name, Expression_ptr expression) : name(name), expression(std::move(expression)) {};
	void print(int level);
};

class Identifier : public Expression
{
	std::string name;

public:
	Identifier(std::string name) : name(name) {};
	void print(int level);
};

class FunctionCall : public Expression
{
	std::string name;
	std::vector<Expression_ptr> arguments;

public:
	FunctionCall(std::string name, std::vector<Expression_ptr> arguments) : name(name), arguments(arguments) {};
	void print(int level);
};

class InclusiveRange : public Expression
{
	Expression_ptr left;
	Expression_ptr right;

public:
	InclusiveRange(Expression_ptr left, Expression_ptr right) : left(std::move(left)), right(std::move(right)) {};
	void print(int level);
};

class ExclusiveRange : public Expression
{
	Expression_ptr left;
	Expression_ptr right;

public:
	ExclusiveRange(Expression_ptr left, Expression_ptr right) : left(std::move(left)), right(std::move(right)) {};
	void print(int level);
};

class Unary : public Expression
{
	Token_ptr op;
	Expression_ptr operand;

public:
	Unary(Token_ptr op, Expression_ptr operand) : op(std::move(op)), operand(std::move(operand)) {};
	void print(int level);
};

class Binary : public Expression
{
	Expression_ptr left;
	Token_ptr op;
	Expression_ptr right;

public:
	Binary(Expression_ptr left, Token_ptr op, Expression_ptr right) : left(std::move(left)), op(std::move(op)), right(std::move(right)) {};
	void print(int level);
};
