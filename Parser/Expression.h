#pragma once

#ifdef PARSER_EXPORTS
#define PARSER_API __declspec(dllexport)
#else
#define PARSER_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "Token.h"
#include "Types.h"

class PARSER_API Expression
{
public:
	virtual void print(int level) = 0;
};

using Expression_ptr = std::shared_ptr<Expression>;

class PARSER_API StringLiteral : public Expression
{
	std::string value;

public:
	StringLiteral(std::string value) : value(value) {};
	void print(int level);
	std::string get_value();
};

class PARSER_API NumberLiteral : public Expression
{
	double value;

public:
	NumberLiteral(double value) : value(value) {};
	void print(int level);
	double get_value();
};

class PARSER_API BooleanLiteral : public Expression
{
	bool value;

public:
	BooleanLiteral(bool value) : value(value) {};
	void print(int level);
	bool get_value();
};

class PARSER_API TupleLiteral : public Expression
{
	std::vector<Expression_ptr> expressions;

public:
	TupleLiteral(std::vector<Expression_ptr> expressions) : expressions(expressions) {};
	void print(int level);
	std::vector<Expression_ptr> get_expressions();
};

class PARSER_API VectorLiteral : public Expression
{
	std::vector<Expression_ptr> expressions;

public:
	VectorLiteral(std::vector<Expression_ptr> expressions) : expressions(expressions) {};
	void print(int level);
	std::vector<Expression_ptr> get_expressions();
};

class PARSER_API MapLiteral : public Expression
{
	std::map<Expression_ptr, Expression_ptr> pairs;

public:
	MapLiteral(std::map<Expression_ptr, Expression_ptr> pairs) : pairs(pairs) {};
	void print(int level);
	std::map<Expression_ptr, Expression_ptr> get_pairs();
};

class PARSER_API RecordLiteral : public Expression
{
	std::map<std::string, Expression_ptr> pairs;

public:
	RecordLiteral(std::map<std::string, Expression_ptr> pairs) : pairs(pairs) {};
	void print(int level);
	std::map<std::string, Expression_ptr> get_pairs();
};

class PARSER_API MemberAccess : public Expression
{
	std::string name;
	Expression_ptr expression;

public:
	MemberAccess(std::string name, Expression_ptr expression) : name(name), expression(std::move(expression)) {};
	void print(int level);
	std::string get_container_name();
	Expression_ptr get_index_expression();
};

class PARSER_API RecordMemberAccess : public Expression
{
	std::string record_name;
	std::string member_name;

public:
	RecordMemberAccess(std::string record_name, std::string member_name) : record_name(record_name), member_name(member_name) {};
	void print(int level);
	std::string get_record_name();
	std::string get_member_name();
};

class PARSER_API Identifier : public Expression
{
	std::string name;

public:
	Identifier(std::string name) : name(name) {};
	void print(int level);
	std::string get_name();
};

class PARSER_API FunctionCall : public Expression
{
	std::string name;
	std::vector<Expression_ptr> arguments;

public:
	FunctionCall(std::string name, std::vector<Expression_ptr> arguments) : name(name), arguments(arguments) {};
	void print(int level);
	std::string get_function_name();
	std::vector<Expression_ptr> get_arguments();
};

class PARSER_API InclusiveRange : public Expression
{
	Expression_ptr left;
	Expression_ptr right;

public:
	InclusiveRange(Expression_ptr left, Expression_ptr right) : left(std::move(left)), right(std::move(right)) {};
	void print(int level);
	Expression_ptr get_left_expression();
	Expression_ptr get_right_expression();
};

class PARSER_API ExclusiveRange : public Expression
{
	Expression_ptr left;
	Expression_ptr right;

public:
	ExclusiveRange(Expression_ptr left, Expression_ptr right) : left(std::move(left)), right(std::move(right)) {};
	void print(int level);
	Expression_ptr get_left_expression();
	Expression_ptr get_right_expression();
};

class PARSER_API Unary : public Expression
{
	Token_ptr op;
	Expression_ptr operand;

public:
	Unary(Token_ptr op, Expression_ptr operand) : op(std::move(op)), operand(std::move(operand)) {};
	void print(int level);
	Expression_ptr get_expression();
};

class PARSER_API Binary : public Expression
{
	Expression_ptr left;
	Token_ptr op;
	Expression_ptr right;

public:
	Binary(Expression_ptr left, Token_ptr op, Expression_ptr right) : left(std::move(left)), op(std::move(op)), right(std::move(right)) {};
	void print(int level);

	Expression_ptr get_left_expression();
	Token_ptr get_operator();
	Expression_ptr get_right_expression();
};
