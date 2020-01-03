#pragma once
#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <variant>

#include "Types.h"
#include "ExpressionNodes.h"

class Let;
class Const;

class Assignment;

class Branch;
class Loop;
class Break;
class Continue;
class Return;

class RecordDefinition;
class FunctionDefinition;

class ExpressionStatement;
class Import;

using StatementNode = std::variant<
	std::monostate,
	Let, Const, Assignment,
	Branch, Loop, Break, Continue,
	RecordDefinition, FunctionDefinition,
	Return, ExpressionStatement, Import
>;

using StatementNode_ptr = std::shared_ptr<StatementNode>;

class Statement
{
public:
	virtual void print(int level) = 0;
};

using Block = std::vector<StatementNode_ptr>;

class Let : public Statement
{
	bool is_public;
	std::string name;
	TypeNode_ptr type;
	ExpressionNode_ptr expression;

public:
	Let(bool is_public, std::string name, TypeNode_ptr type, ExpressionNode_ptr expression) : is_public(is_public), name(name), type(type), expression(expression) {};
	void print(int level);
};

class Const : public Statement
{
	bool is_public;
	std::string name;
	TypeNode_ptr type;
	ExpressionNode_ptr expression;

public:
	Const(bool is_public, std::string name, TypeNode_ptr type, ExpressionNode_ptr expression) : is_public(is_public), name(name), type(type), expression(expression) {};
	void print(int level);
};

class Assignment : public Statement
{
	std::string name;
	ExpressionNode_ptr expression;

public:
	Assignment(std::string name, ExpressionNode_ptr expression) : name(name), expression(expression) {};
	void print(int level);
};

class Branch : public Statement
{
	ExpressionNode_ptr condition;
	Block consequence;
	Block alternative;

public:
	Branch(ExpressionNode_ptr condition, Block consequence, Block alternative) : condition(condition), consequence(consequence), alternative(alternative) {};
	void print(int level);
};

class Loop : public Statement
{
	Block block;

public:
	Loop(Block block) : block(block) {};
	void print(int level);
};

class Break : public Statement
{
public:
	void print(int level);
};

class Continue : public Statement
{
public:
	void print(int level);
};

class RecordDefinition : public Statement
{
	bool is_public;
	std::string name;
	std::vector<std::pair<std::string, TypeNode_ptr>> member_types;

public:
	RecordDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, TypeNode_ptr>> member_types) : is_public(is_public), name(name), member_types(member_types) {};
	void print(int level);
};

class FunctionDefinition : public Statement
{
	bool is_public;
	std::string name;
	std::vector<std::pair<std::string, TypeNode_ptr>> arguments;
	std::optional<TypeNode_ptr> return_type;
	Block body;

public:
	FunctionDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, TypeNode_ptr>> arguments, std::optional<TypeNode_ptr> return_type, Block body) : is_public(is_public), name(name), arguments(arguments), return_type(return_type), body(body) {};
	void print(int level);
};

class Return : public Statement
{
	std::optional<ExpressionNode_ptr> expression;

public:
	Return(std::optional<ExpressionNode_ptr> expression) : expression(expression) {};
	void print(int level);
};

class ExpressionStatement : public Statement
{
	ExpressionNode_ptr expression;

public:
	ExpressionStatement(ExpressionNode_ptr expression) : expression(std::move(expression)) {};
	void print(int level);
};

class Import : public Statement
{
	std::vector<std::string> goods;
	std::string path;

public:
	Import(std::vector<std::string> goods, std::string path) : goods(goods), path(path) {};
	void print(int level);
};

void print_statement_node(StatementNode_ptr node, int level);
