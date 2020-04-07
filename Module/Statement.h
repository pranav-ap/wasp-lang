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
	virtual Object_ptr interpret(StatementVisitor& visitor) = 0;
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
	Object_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Assignment : public Statement, public std::enable_shared_from_this<Assignment>
{
	std::string name;
	Expression_ptr expression;

	Assignment(std::string name, Expression_ptr expression) : name(name), expression(std::move(expression)) {};
	Object_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Branch : public Statement, public std::enable_shared_from_this<Branch>
{
	Expression_ptr condition;
	Block_ptr consequence;
	Block_ptr alternative;

	Branch(Expression_ptr condition, Block_ptr consequence, Block_ptr alternative) : condition(std::move(condition)), consequence(std::move(consequence)), alternative(std::move(alternative)) {};
	Object_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Loop : public Statement, public std::enable_shared_from_this<Loop>
{
	Block_ptr block;
	Loop(Block_ptr block) : block(block) {};
	Object_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API ForEachLoop : public Statement, public std::enable_shared_from_this<ForEachLoop>
{
	std::string item_name;
	std::string iterable_name;
	Block_ptr block;

	ForEachLoop(std::string item_name, std::string iterable_name, Block_ptr block)
		: item_name(item_name), iterable_name(iterable_name), block(block) {};
	Object_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Break : public Statement, public std::enable_shared_from_this<Break>
{
	Object_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Continue : public Statement, public std::enable_shared_from_this<Continue>
{
	Object_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API UDTDefinition : public Statement, public std::enable_shared_from_this<UDTDefinition>
{
	bool is_public;
	std::string name;
	std::map<std::string, Type_ptr> member_types;

	UDTDefinition(bool is_public, std::string name, std::map<std::string, Type_ptr> member_types) : is_public(is_public), name(name), member_types(member_types) {};
	Object_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API FunctionDefinition : public Statement, public std::enable_shared_from_this<FunctionDefinition>
{
	bool is_public;
	std::string name;
	std::vector<std::pair<std::string, Type_ptr>> arguments;
	std::optional<Type_ptr> return_type;
	Block_ptr body;

	FunctionDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block_ptr body) : is_public(is_public), name(name), arguments(arguments), return_type(return_type), body(body) {};
	Object_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Return : public Statement, public std::enable_shared_from_this<Return>
{
	std::optional<Expression_ptr> expression;
	Return(std::optional<Expression_ptr> expression) : expression(std::move(expression)) {};
	Object_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API ExpressionStatement : public Statement, public std::enable_shared_from_this<ExpressionStatement>
{
	Expression_ptr expression;
	ExpressionStatement(Expression_ptr expression) : expression(std::move(expression)) {};
	Object_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Import : public Statement, public std::enable_shared_from_this<Import>
{
	std::vector<std::string> goods;
	std::string path;

	Import(std::vector<std::string> goods, std::string path) : goods(goods), path(path) {};
	Object_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API ImportSTD : public Statement, public std::enable_shared_from_this<ImportSTD>
{
	std::string name;

	ImportSTD(std::string name) : name(name) {};
	Object_ptr interpret(StatementVisitor& visitor);
};

struct MODULE_API Enum : public Statement, public std::enable_shared_from_this<Enum>
{
	bool is_public;
	std::string name;
	std::vector<std::string> members;

	Enum(bool is_public, std::string name, std::vector<std::string> members)
		: is_public(is_public), name(name), members(members) {};

	Object_ptr interpret(StatementVisitor& visitor);
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
using ImportSTD_ptr = MODULE_API std::shared_ptr<ImportSTD>;
using Enum_ptr = MODULE_API std::shared_ptr<Enum>;

// Printers

MODULE_API std::ostream& operator<<(std::ostream& os, const VariableDeclaration_ptr stat);
MODULE_API std::ostream& operator<<(std::ostream& os, const Assignment_ptr stat);
MODULE_API std::ostream& operator<<(std::ostream& os, const Branch_ptr stat);
MODULE_API std::ostream& operator<<(std::ostream& os, const Loop_ptr stat);
MODULE_API std::ostream& operator<<(std::ostream& os, const ForEachLoop_ptr stat);
MODULE_API std::ostream& operator<<(std::ostream& os, const Break_ptr stat);
MODULE_API std::ostream& operator<<(std::ostream& os, const Continue_ptr stat);
MODULE_API std::ostream& operator<<(std::ostream& os, const UDTDefinition_ptr stat);
MODULE_API std::ostream& operator<<(std::ostream& os, const FunctionDefinition_ptr stat);
MODULE_API std::ostream& operator<<(std::ostream& os, const Return_ptr stat);
MODULE_API std::ostream& operator<<(std::ostream& os, const ExpressionStatement_ptr stat);
MODULE_API std::ostream& operator<<(std::ostream& os, const Import_ptr stat);
MODULE_API std::ostream& operator<<(std::ostream& os, const ImportSTD_ptr stat);
MODULE_API std::ostream& operator<<(std::ostream& os, const Enum_ptr stat);
