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
#include "Expression.h"

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

	bool is_public_declaration();
	bool is_mutable_declaration();
	std::string get_variable_name();
	Type_ptr get_type();
	Expression_ptr get_expression();
};

class PARSER_API Assignment : public Statement
{
	std::string name;
	Expression_ptr expression;

public:
	Assignment(std::string name, Expression_ptr expression) : name(name), expression(std::move(expression)) {};
	void print(int level);
	std::string get_variable_name();
	Expression_ptr get_expression();
};

class PARSER_API Branch : public Statement
{
	Expression_ptr condition;
	Block_ptr consequence;
	Block_ptr alternative;

public:
	Branch(Expression_ptr condition, Block_ptr consequence, Block_ptr alternative) : condition(std::move(condition)), consequence(std::move(consequence)), alternative(std::move(alternative)) {};
	void print(int level);

	Expression_ptr get_condition();
	Block_ptr get_consequence();
	Block_ptr get_alternative();
};

class PARSER_API Loop : public Statement
{
	Block_ptr block;

public:
	Loop(Block_ptr block) : block(block) {};
	void print(int level);
	Block_ptr get_block();
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

	std::string get_alias();
	Type_ptr get_type();
};

class PARSER_API RecordDefinition : public Statement
{
	bool is_public;
	std::string name;
	std::map<std::string, Type_ptr> member_types;

public:
	RecordDefinition(bool is_public, std::string name, std::map<std::string, Type_ptr> member_types) : is_public(is_public), name(name), member_types(member_types) {};
	void print(int level);

	bool is_public_declaration();
	std::string get_name();
	std::map<std::string, Type_ptr> get_member_types();
};

class PARSER_API FunctionDefinition : public Statement
{
	bool is_public;
	std::string name;
	std::map<std::string, Type_ptr> arguments;
	std::optional<Type_ptr> return_type;
	Block_ptr body;

public:
	FunctionDefinition(bool is_public, std::string name, std::map<std::string, Type_ptr> arguments, std::optional<Type_ptr> return_type, Block_ptr body) : is_public(is_public), name(name), arguments(arguments), return_type(return_type), body(body) {};
	void print(int level);

	bool is_public_declaration();
	std::string get_name();
	std::map<std::string, Type_ptr> get_arguments();
	std::optional<Type_ptr> get_return_type();
	Block_ptr get_function_body();
};

class PARSER_API Return : public Statement
{
	std::optional<Expression_ptr> expression;

public:
	Return(std::optional<Expression_ptr> expression) : expression(std::move(expression)) {};
	void print(int level);

	std::optional<Expression_ptr> get_optional_expression();
};

class PARSER_API ExpressionStatement : public Statement
{
	Expression_ptr expression;

public:
	ExpressionStatement(Expression_ptr expression) : expression(std::move(expression)) {};
	void print(int level);
	Expression_ptr get_expression();
};

class PARSER_API Import : public Statement
{
	std::vector<std::string> goods;
	std::string path;

public:
	Import(std::vector<std::string> goods, std::string path) : goods(goods), path(path) {};
	void print(int level);

	std::vector<std::string> get_goods();
	std::string get_path();
};
