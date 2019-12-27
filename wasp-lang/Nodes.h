#pragma once
#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <variant>
#include "Token.h"
#include "Types.h"
#include "ExpressionNodes.h"

class Statement {};

using Block = std::vector<Statement>;

class Let : public Statement
{
	bool is_public;
	std::string name;
	Type_ptr type;
	std::shared_ptr<ExpressionNode> expression;
public:
	Let(bool is_public, std::string name, Type_ptr type, std::shared_ptr<ExpressionNode> expression) :
		is_public(is_public), name(name), type(type), expression(expression) {};
};

class Const : public Statement
{
	bool is_public;
	std::string name;
	Type_ptr type;
	std::shared_ptr<ExpressionNode> expression;
public:
	Const(bool is_public, std::string name, Type_ptr type, std::shared_ptr<ExpressionNode> expression) :
		is_public(is_public), name(name), type(type), expression(expression) {};
};

class Assignment : public Statement
{
	std::string name;
	std::shared_ptr<ExpressionNode> expression;
public:
	Assignment(std::string name, std::shared_ptr<ExpressionNode> expression) : name(name), expression(expression) {};
};

class Branch : public Statement
{
	std::shared_ptr<ExpressionNode> condition;
	Block consequence;
	Block alternative;
public:
	Branch(std::shared_ptr<ExpressionNode> condition, Block consequence, Block alternative) :
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
	std::vector<std::pair<std::string, Type_ptr>> member_types;
public:
	RecordDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type_ptr>> member_types) :
		is_public(is_public), name(name), member_types(member_types) {};
};

class FunctionDefinition : public Statement
{
	bool is_public;
	std::string name;
	std::vector<std::pair<std::string, Type_ptr>> arguments;
	std::optional<Type_ptr> return_type;
	Block body;
public:
	FunctionDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block body) :
		is_public(is_public), name(name), arguments(arguments), return_type(return_type), body(body) {};
};

class Return : public Statement
{
	std::optional<std::shared_ptr<ExpressionNode>> expression;
public:
	Return(std::optional<std::shared_ptr<ExpressionNode>> expression) : expression(expression) {};
};

class ExpressionStatement : public Statement
{
	std::shared_ptr<ExpressionNode> expression;
public:
	ExpressionStatement(std::shared_ptr<ExpressionNode> expression) : expression(expression) {};
};

class Import : public Statement
{
	std::vector<std::string> goods;
	std::string path;
public:
	Import(std::vector<std::string> goods, std::string path) : goods(goods), path(path) {};
};

using StatementNode = std::variant<
	std::monostate,
	Let, Const, Assignment,
	Branch, Loop, Break, Continue,
	RecordDefinition, FunctionDefinition,
	Return, ExpressionStatement, Import
>;

class Module
{
	std::vector<std::shared_ptr<StatementNode>> nodes;
public:
	void add(std::shared_ptr<StatementNode> node);
};
