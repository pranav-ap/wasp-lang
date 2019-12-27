#pragma once
#include <optional>
#include <string>
#include <vector>
#include <utility>
#include <variant>
#include "Token.h"
#include "Types.h"

// Roots

class Statement {};

class Expression {};

typedef std::vector<Statement> Block;

// Statements

class Let : public Statement
{
	bool is_public;
	std::string name;
	Type type;
	Expression expression;
public:
	Let(bool is_public, std::string name, Type type, Expression expression) :
		is_public(is_public), name(name), type(type), expression(expression) {};
};

class Const : public Statement
{
	bool is_public;
	std::string name;
	Type type;
	Expression expression;
public:
	Const(bool is_public, std::string name, Type type, Expression expression) :
		is_public(is_public), name(name), type(type), expression(expression) {};
};

class Assignment : public Statement
{
	std::string name;
	Expression expression;
public:
	Assignment(std::string name, Expression expression) : name(name), expression(expression) {};
};

class Branch : public Statement
{
	Expression condition;
	Block consequence;
	Block alternative;
public:
	Branch(Expression condition, Block consequence, Block alternative) :
		condition(condition), consequence(consequence), alternative(alternative) {};
};

class Loop : public Statement
{
	Block block;
public:
	Loop(Block block) : block(block) {};
};

class Break : public Statement {};

class Continue : public Statement {};

class RecordDefinition : public Statement
{
	bool is_public;
	std::string name;
	std::vector<std::pair<std::string, Type>> member_types;
public:
	RecordDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type>> member_types) :
		is_public(is_public), name(name), member_types(member_types) {};
};

class FunctionDefinition : public Statement
{
	bool is_public;
	std::string name;
	std::vector<std::pair<std::string, Type>> arguments;
	std::optional<Type> return_type;
	Block body;
public:
	FunctionDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type>> arguments, std::optional<Type> return_type, Block body) :
		is_public(is_public), name(name), arguments(arguments), return_type(return_type), body(body) {};
};

class Return : public Statement
{
	std::optional<Expression> expression;
public:
	Return(std::optional<Expression> expression) : expression(expression) {};
};

class ExpressionStatement : public Statement
{
	Expression expression;
public:
	ExpressionStatement(Expression expression) : expression(expression) {};
};

class Import : public Statement
{
	std::vector<std::string> goods;
	std::string path;
public:
	Import(std::vector<std::string> goods, std::string path) : goods(goods), path(path) {};
};

// Expressions

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
	std::vector<Expression> expressions;
public:
	TupleLiteral(std::vector<Expression> expressions) : expressions(expressions) {};
};

class VectorLiteral : public Expression
{
	std::vector<Expression> expressions;
public:
	VectorLiteral(std::vector<Expression> expressions) : expressions(expressions) {};
};

class MapLiteral : public Expression
{
	std::vector<std::pair<Scalar, Expression>> value;
public:
	MapLiteral(std::vector<std::pair<Scalar, Expression>> value) : value(value) {};
};

class RecordLiteral : public Expression
{
	std::vector<std::pair<std::string, Expression>> value;
public:
	RecordLiteral(std::vector<std::pair<std::string, Expression>> value) : value(value) {};
};

class MemberAccess : public Expression
{
	std::string name;
	Expression index_expression;
public:
	MemberAccess(std::string name, Expression index_expression) : name(name), index_expression(index_expression) {};
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
	std::vector<Expression> arguments;
public:
	FunctionCall(std::string name, std::vector<Expression> arguments) : name(name), arguments(arguments) {};
};

class Unary : public Expression
{
	Expression operand;
	Token op;
public:
	Unary(Expression operand, Token op) : operand(operand), op(op) {};
};

class Binary : public Expression
{
	Expression left;
	Token op;
	Expression right;
public:
	Binary(Expression left, Token op, Expression right) : left(left), op(op), right(right) {};
};

// Other

typedef std::variant<
	std::monostate,
	Let, Const, Assignment,
	Branch, Loop, Break, Continue,
	RecordDefinition, FunctionDefinition,
	Return, ExpressionStatement, Import
> Node;

typedef std::variant <
	std::monostate,
	StringLiteral, NumberLiteral, BooleanLiteral,
	TupleLiteral, VectorLiteral, MapLiteral, RecordLiteral,
	MemberAccess, RecordMemberAccess, Identifier, FunctionCall,
	Unary, Binary
> ExpressionNode;

class Module
{
	std::vector<Node> nodes;
public:
	void add(Node node);
};
