#pragma once
#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <variant>

#include "Token.h"
#include "Types.h"

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
	TupleLiteral, VectorLiteral,
	MapLiteral, RecordLiteral,
	MemberAccess, RecordMemberAccess,
	Identifier, FunctionCall,
	Unary, Binary
>;

using ExpressionNode_ptr = std::shared_ptr<ExpressionNode>;

class Expression
{
public:
	virtual void print(int level) = 0;
};

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
	std::vector<ExpressionNode_ptr> expressions;

public:
	TupleLiteral(std::vector<ExpressionNode_ptr> expressions) : expressions(expressions) {};
	void print(int level);
};

class VectorLiteral : public Expression
{
	std::vector<ExpressionNode_ptr> expressions;

public:
	VectorLiteral(std::vector<ExpressionNode_ptr> expressions) : expressions(expressions) {};
	void print(int level);
};

class MapLiteral : public Expression
{
	std::vector<std::pair<KeyTypeNode_ptr, ExpressionNode_ptr>> pairs;

public:
	MapLiteral(std::vector<std::pair<KeyTypeNode_ptr, ExpressionNode_ptr>> pairs) : pairs(pairs) {};
	void print(int level);
};

class RecordLiteral : public Expression
{
	std::vector<std::pair<std::string, ExpressionNode_ptr>> pairs;

public:
	RecordLiteral(std::vector<std::pair<std::string, ExpressionNode_ptr>> pairs) : pairs(pairs) {};
	void print(int level);
};

class MemberAccess : public Expression
{
	std::string name;
	ExpressionNode_ptr index_expression;

public:
	MemberAccess(std::string name, ExpressionNode_ptr index_expression) : name(name), index_expression(index_expression) {};
	void print(int level);
};

class RecordMemberAccess : public Expression
{
	std::string record_name;
	std::string member_name;

public:
	RecordMemberAccess(std::string record_name, std::string member_name) : record_name(record_name), member_name(member_name) {};
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
	std::vector<ExpressionNode_ptr> arguments;

public:
	FunctionCall(std::string name, std::vector<ExpressionNode_ptr> arguments) : name(name), arguments(arguments) {};
	void print(int level);
};

class Unary : public Expression
{
	std::shared_ptr<Token> op;
	ExpressionNode_ptr operand;

public:
	Unary(std::shared_ptr<Token> op, ExpressionNode_ptr operand) : op(op), operand(operand) {};
	void print(int level);
};

class Binary : public Expression
{
	ExpressionNode_ptr left;
	std::shared_ptr<Token> op;
	ExpressionNode_ptr right;

public:
	Binary(ExpressionNode_ptr left, std::shared_ptr<Token> op, ExpressionNode_ptr right) : left(left), op(op), right(right) {};
	void print(int level);
};

void print_expression_node(ExpressionNode_ptr node, int level);
