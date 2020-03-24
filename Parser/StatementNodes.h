#pragma once

#ifdef PARSER_EXPORTS
#define PARSER_API __declspec(dllexport)
#else
#define PARSER_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <memory>
#include <optional>

#include "Types.h"
#include "ExpressionNodes.h"

class PARSER_API Statement
{
public:
	virtual void print(int level) = 0;
};

using Statement_ptr = PARSER_API std::shared_ptr<Statement>;
using Block = PARSER_API std::vector<Statement_ptr>;
using Block_ptr = PARSER_API std::shared_ptr<Block>;

class PARSER_API VariableDeclaration : public Statement
{
	bool is_public;
	bool is_mutable;
	std::string name;
	Type_ptr type;
	Expression_ptr expression;

public:
	VariableDeclaration(bool is_public, bool is_mutable, std::string name, Type_ptr type, Expression_ptr expression) : is_public(is_public), is_mutable(is_mutable), name(name), type(std::move(type)), expression(std::move(expression)) {};
	void print(int level);
};

class PARSER_API Assignment : public Statement
{
	std::string name;
	Expression_ptr expression;

public:
	Assignment(std::string name, Expression_ptr expression) : name(name), expression(std::move(expression)) {};
	void print(int level);
};

class PARSER_API Branch : public Statement
{
	Expression_ptr condition;
	Block consequence;
	Block alternative;

public:
	Branch(Expression_ptr condition, Block consequence, Block alternative) : condition(std::move(condition)), consequence(std::move(consequence)), alternative(std::move(alternative)) {};
	void print(int level);
};

class PARSER_API Loop : public Statement
{
	Block block;

public:
	Loop(Block block) : block(block) {};
	void print(int level);
};

class PARSER_API Break : public Statement
{
public:
	void print(int level);
};

class PARSER_API Continue : public Statement
{
public:
	void print(int level);
};

class PARSER_API Alias : public Statement
{
	std::string name;
	Type_ptr type;

public:
	Alias(std::string name, Type_ptr type) : name(name), type(std::move(type)) {};
	void print(int level);
};

class PARSER_API RecordDefinition : public Statement
{
	bool is_public;
	std::string name;
	std::vector<std::pair<std::string, Type_ptr>> member_types;

public:
	RecordDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type_ptr>> member_types) : is_public(is_public), name(name), member_types(member_types) {};
	void print(int level);
};

class PARSER_API FunctionDefinition : public Statement
{
	bool is_public;
	std::string name;
	std::vector<std::pair<std::string, Type_ptr>> arguments;
	std::optional<Type_ptr> return_type;
	Block body;

public:
	FunctionDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block body) : is_public(is_public), name(name), arguments(arguments), return_type(return_type), body(body) {};
	void print(int level);
};

class PARSER_API Return : public Statement
{
	std::optional<Expression_ptr> expression;

public:
	Return(std::optional<Expression_ptr> expression) : expression(std::move(expression)) {};
	void print(int level);
};

class PARSER_API ExpressionStatement : public Statement
{
	Expression_ptr expression;

public:
	ExpressionStatement(Expression_ptr expression) : expression(std::move(expression)) {};
	void print(int level);
};

class PARSER_API Import : public Statement
{
	std::vector<std::string> goods;
	std::string path;

public:
	Import(std::vector<std::string> goods, std::string path) : goods(goods), path(path) {};
	void print(int level);
};
