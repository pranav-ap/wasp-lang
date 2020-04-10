#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>

#include "Types.h"
#include "Expression.h"

class StatementVisitor;

struct MODULE_API Statement
{
	virtual ObjectVariant_ptr interpret(StatementVisitor& visitor) = 0;
};

using Statement_ptr = MODULE_API std::shared_ptr<Statement>;
using Block = MODULE_API std::vector<Statement_ptr>;
using Block_ptr = MODULE_API std::shared_ptr<Block>;

struct MODULE_API VariableDeclaration : public Statement, public std::enable_shared_from_this<VariableDeclaration>
{
	bool is_public;
	bool is_mutable;
	std::string name;
	Type_ptr type;
	Expression_ptr expression;

	VariableDeclaration(bool is_public, bool is_mutable, std::string name, Type_ptr type, Expression_ptr expression) : is_public(is_public), is_mutable(is_mutable), name(name), type(std::move(type)), expression(std::move(expression)) {};
	ObjectVariant_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Assignment : public Statement, public std::enable_shared_from_this<Assignment>
{
	std::string name;
	Expression_ptr expression;

	Assignment(std::string name, Expression_ptr expression) : name(name), expression(std::move(expression)) {};
	ObjectVariant_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Branch : public Statement, public std::enable_shared_from_this<Branch>
{
	Expression_ptr condition;
	Block_ptr consequence;
	Block_ptr alternative;

	Branch(Expression_ptr condition, Block_ptr consequence, Block_ptr alternative) : condition(std::move(condition)), consequence(std::move(consequence)), alternative(std::move(alternative)) {};
	ObjectVariant_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Loop : public Statement, public std::enable_shared_from_this<Loop>
{
	Block_ptr block;
	Loop(Block_ptr block) : block(block) {};
	ObjectVariant_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API ForEachLoop : public Statement, public std::enable_shared_from_this<ForEachLoop>
{
	std::string item_name;
	std::string iterable_name;
	Block_ptr block;

	ForEachLoop(std::string item_name, std::string iterable_name, Block_ptr block)
		: item_name(item_name), iterable_name(iterable_name), block(block) {};
	ObjectVariant_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Break : public Statement, public std::enable_shared_from_this<Break>
{
	ObjectVariant_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Continue : public Statement, public std::enable_shared_from_this<Continue>
{
	ObjectVariant_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API UDTDefinition : public Statement, public std::enable_shared_from_this<UDTDefinition>
{
	bool is_public;
	std::string name;
	std::map<std::string, Type_ptr> member_types;

	UDTDefinition(bool is_public, std::string name, std::map<std::string, Type_ptr> member_types) : is_public(is_public), name(name), member_types(member_types) {};
	ObjectVariant_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API FunctionDefinition : public Statement, public std::enable_shared_from_this<FunctionDefinition>
{
	bool is_public;
	std::string name;
	std::vector<std::pair<std::string, Type_ptr>> arguments;
	std::optional<Type_ptr> return_type;
	Block_ptr body;

	FunctionDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block_ptr body) : is_public(is_public), name(name), arguments(arguments), return_type(return_type), body(body) {};
	ObjectVariant_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API EnumDefinition : public Statement, public std::enable_shared_from_this<EnumDefinition>
{
	bool is_public;
	std::string name;
	std::vector<std::string> members;

	EnumDefinition(bool is_public, std::string name, std::vector<std::string> members)
		: is_public(is_public), name(name), members(members) {};

	ObjectVariant_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Return : public Statement, public std::enable_shared_from_this<Return>
{
	std::optional<Expression_ptr> expression;
	Return() : expression(std::nullopt) {};
	Return(std::optional<Expression_ptr> expression) : expression(std::move(expression)) {};
	ObjectVariant_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API ExpressionStatement : public Statement, public std::enable_shared_from_this<ExpressionStatement>
{
	Expression_ptr expression;
	ExpressionStatement(Expression_ptr expression) : expression(std::move(expression)) {};
	ObjectVariant_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Import : public Statement, public std::enable_shared_from_this<Import>
{
	std::vector<std::string> goods;
	std::string path;
	bool is_inbuilt;

	Import(std::string path, std::vector<std::string> goods, bool is_inbuilt)
		: goods(goods), path(path), is_inbuilt(is_inbuilt) {};
	ObjectVariant_ptr interpret(StatementVisitor& visitor);
};

using VariableDeclaration_ptr = MODULE_API std::shared_ptr<VariableDeclaration>;
using Assignment_ptr = MODULE_API std::shared_ptr<Assignment>;
using Branch_ptr = MODULE_API std::shared_ptr<Branch>;
using Loop_ptr = MODULE_API std::shared_ptr<Loop>;
using ForEachLoop_ptr = MODULE_API std::shared_ptr<ForEachLoop>;
using Break_ptr = MODULE_API std::shared_ptr<Break>;
using Continue_ptr = MODULE_API std::shared_ptr<Continue>;
using UDTDefinition_ptr = MODULE_API std::shared_ptr<UDTDefinition>;
using FunctionDefinition_ptr = MODULE_API std::shared_ptr<FunctionDefinition>;
using Return_ptr = MODULE_API std::shared_ptr<Return>;
using ExpressionStatement_ptr = MODULE_API std::shared_ptr<ExpressionStatement>;
using Import_ptr = MODULE_API std::shared_ptr<Import>;
using EnumDefinition_ptr = MODULE_API std::shared_ptr<EnumDefinition>;
