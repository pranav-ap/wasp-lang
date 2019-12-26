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
};

class Const : public Statement
{
	bool is_public;
	std::string name;
	Type type;
	Expression expression;
};

class Assignment : public Statement
{
	std::string name;
	Expression expression;
};

class Branch : public Statement
{
	Expression condition;
	Block consequence;
	Block alternative;
};

class Loop : public Statement
{
	Block block;
};

class Break : public Statement {};

class Continue : public Statement {};

class RecordDefinition : public Statement
{
	bool is_public;
	std::string name;
	std::vector<std::pair<std::string, Type>> member_types;
};

class FunctionDefinition : public Statement
{
	bool is_public;
	std::string name;
	std::vector<std::pair<std::string, Type>> arguments;
	std::optional<Type> return_type;
	Block body;
};

class Return : public Statement
{
	std::optional<Expression> expression;
};

class ExpressionStatement : public Statement
{
	Expression expression;
};

class Import : public Statement
{
	std::vector<std::string> goods;
	std::string path;
};

// Expressions

class StringLiteral : public Expression
{
	std::string value;
};

class NumberLiteral : public Expression
{
	double value;
};

class BooleanLiteral : public Expression
{
	bool value;
};

class TupleLiteral : public Expression
{
	std::vector<Expression> expressions;
};

class VectorLiteral : public Expression
{
	std::vector<Expression> expressions;
};

class MapLiteral : public Expression
{
	std::vector<std::pair<Scalar, Expression>> value;
};

class RecordLiteral : public Expression
{
	std::vector<std::pair<std::string, Expression>> value;
};

class MemberAccess : public Expression
{
	std::string name;
	Expression index_expression;
};

class RecordMemberAccess : public Expression
{
	std::string record_name;
	std::string member_name;
};

class Identifier : public Expression
{
	std::string name;
};

class FunctionCall : public Expression
{
	std::string name;
	std::vector<Expression> arguments;
};

class Unary : public Expression
{
	Expression operand;
	Token op;
};

class Binary : public Expression
{
	Expression left;
	Token op;
	Expression right;
};

// Other

typedef std::variant<
	std::monostate,
	Let, Const, Assignment,
	Branch, Loop, Break, Continue,
	RecordDefinition, FunctionDefinition,
	Return, ExpressionStatement, Import
> Node;

class Module
{
	std::vector<Node> nodes;
public:
	void add(Node node);
};
