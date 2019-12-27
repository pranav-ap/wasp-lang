#pragma once
#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <variant>

class StringLiteral;
class NumberLiteral;
class BooleanLiteral;
class TupleLiteral;
class VectorLiteral;
class MapLiteral;
class RecordLiteral;
class MemberAccess;
class RecordMemberAccess;
class Identifier;
class FunctionCall;
class Unary;
class Binary;

using ExpressionNode = std::variant<
	std::monostate,
	StringLiteral, NumberLiteral, BooleanLiteral,
	TupleLiteral, VectorLiteral, MapLiteral, RecordLiteral,
	MemberAccess, RecordMemberAccess, Identifier, FunctionCall,
	Unary, Binary
>;

class Expression {};

class StringLiteral : public Expression
{
	std::string value;
public:
	StringLiteral(std::string value) : value(value) {};
};

class NumberLiteral : public Expression
{
	double value;
public:
	NumberLiteral(double value) : value(value) {};
};

class BooleanLiteral : public Expression
{
	bool value;
public:
	BooleanLiteral(bool value) : value(value) {};
};

class TupleLiteral : public Expression
{
	std::vector<std::shared_ptr<ExpressionNode>> expressions;
public:
	TupleLiteral(std::vector<std::shared_ptr<ExpressionNode>> expressions) : expressions(expressions) {};
};

class VectorLiteral : public Expression
{
	std::vector<std::shared_ptr<ExpressionNode>> expressions;
public:
	VectorLiteral(std::vector<std::shared_ptr<ExpressionNode>> expressions) : expressions(expressions) {};
};

class MapLiteral : public Expression
{
	std::vector<std::pair<Scalar, std::shared_ptr<ExpressionNode>>> value;
public:
	MapLiteral(std::vector<std::pair<Scalar, std::shared_ptr<ExpressionNode>>> value) : value(value) {};
};

class RecordLiteral : public Expression
{
	std::vector<std::pair<std::string, std::shared_ptr<ExpressionNode>>> value;
public:
	RecordLiteral(std::vector<std::pair<std::string, std::shared_ptr<ExpressionNode>>> value) : value(value) {};
};

class MemberAccess : public Expression
{
	std::string name;
	std::shared_ptr<ExpressionNode> index_expression;
public:
	MemberAccess(std::string name, std::shared_ptr<ExpressionNode> index_expression) : name(name), index_expression(index_expression) {};
};

class RecordMemberAccess : public Expression
{
	std::string record_name;
	std::string member_name;
public:
	RecordMemberAccess(std::string record_name, std::string member_name) : record_name(record_name), member_name(member_name) {};
};

class Identifier : public Expression
{
	std::string name;
public:
	Identifier(std::string name) : name(name) {};
};

class FunctionCall : public Expression
{
	std::string name;
	std::vector<std::shared_ptr<ExpressionNode>> arguments;
public:
	FunctionCall(std::string name, std::vector<std::shared_ptr<ExpressionNode>> arguments) : name(name), arguments(arguments) {};
};

class Unary : public Expression
{
	std::shared_ptr<Token> op;
	std::shared_ptr<ExpressionNode> operand;
public:
	Unary(std::shared_ptr<Token> op, std::shared_ptr<ExpressionNode> operand) : op(op), operand(operand) {};
};

class Binary : public Expression
{
	std::shared_ptr<ExpressionNode> left;
	std::shared_ptr<Token> op;
	std::shared_ptr<ExpressionNode> right;
public:
	Binary(std::shared_ptr<ExpressionNode> left, std::shared_ptr<Token> op, std::shared_ptr<ExpressionNode> right) : left(left), op(op), right(right) {};
};
