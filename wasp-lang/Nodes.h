#pragma once
#include <optional>
#include <string>
#include <vector>
#include <utility>
#include "Token.h"
#include "Types.h"

using namespace Types;

namespace Nodes {
	typedef std::vector<Statement> Block;

	class Statement {};

	class Let : public Statement
	{
		bool is_public;
		std::string name;
		Type type;
		Expression expression;
	public:
		Let(bool is_public, std::string name, Type type, Expression expression) : is_public(is_public),
			name(name), type(type), expression(expression) {};
	};

	class Const : public Statement
	{
		bool is_public;
		std::string name;
		Type type;
		Expression expression;
	public:
		Const(bool is_public, std::string name, Type type, Expression expression) : is_public(is_public),
			name(name), type(type), expression(expression) {};
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
		Branch(Expression condition, Block consequence, Block alternative) : condition(condition),
			consequence(consequence), alternative(alternative) {};
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
		std::vector<std::pair<std::string, Type>> pairs;
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

	// Expression

	class Expression {};

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
	public:
		FunctionCall(std::string name, std::vector<Expression> arguments) : name(name), arguments(arguments) {};
	};

	class Unary : public Expression
	{
		Expression operand;
		Token op;
	public:
		Unary(Token op, Expression operand) : op(op), operand(operand) {};
	};

	class Binary : public Expression
	{
		Expression left;
		Token op;
		Expression right;
	public:
		Binary(Expression left, Token op, Expression right) : left(left), op(op), right(right) {};
	};
}
