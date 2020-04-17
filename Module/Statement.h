#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

#include "Types.h"
#include "Expression.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <variant>

struct Assignment;
struct MultipleAssignment;
struct ConditionalBranch;
struct IfLetBranch;
struct InfiniteLoop;
struct ForEachLoop;
struct Break;
struct Continue;
struct VariableDefinition;
struct UDTDefinition;
struct FunctionDefinition;
struct EnumDefinition;
struct ImportCustom;
struct ImportInBuilt;
struct ExpressionStatement;
struct Return;

using Statement = MODULE_API std::variant<
	std::monostate,
	Assignment, MultipleAssignment,
	ConditionalBranch, IfLetBranch,
	InfiniteLoop, ForEachLoop,
	Break, Continue,
	VariableDefinition, UDTDefinition, FunctionDefinition, EnumDefinition,
	ImportCustom, ImportInBuilt,
	ExpressionStatement,
	Return
>;

using Statement_ptr = MODULE_API std::shared_ptr<Statement>;
using Block = MODULE_API std::vector<Statement_ptr>;

struct MODULE_API StatementBase
{
};

// Assignments

struct MODULE_API Assignment : public StatementBase
{
	std::string name;
	Expression_ptr expression;

	Assignment(std::string name, Expression_ptr expression)
		: name(name), expression(std::move(expression)) {};
};

struct MODULE_API MultipleAssignment : public StatementBase
{
	std::vector<std::string> names;
	std::vector<Expression_ptr> expressions;

	MultipleAssignment(std::vector<std::string> names, std::vector<Expression_ptr> expressions)
		: names(names), expressions(expressions) {};
};

// Branching

struct MODULE_API Branch : public StatementBase
{
	Block consequence;
	Block alternative;

	Branch(Block consequence, Block alternative)
		: consequence(consequence), alternative(alternative) {};
};

struct MODULE_API ConditionalBranch : public Branch
{
	Expression_ptr condition;

	ConditionalBranch(Expression_ptr condition, Block consequence, Block alternative)
		: Branch(consequence, alternative), condition(std::move(condition)) {};
};

struct MODULE_API IfLetBranch : public Branch
{
	std::string variable_name;
	Expression_ptr expression;

	IfLetBranch(std::string variable_name, Expression_ptr expression, Block consequence, Block alternative)
		: Branch(consequence, alternative), variable_name(variable_name), expression(std::move(expression)) {};
};

// Looping

struct MODULE_API Loop : public StatementBase
{
	Block block;
	Loop(Block block) : block(block) {};
};

struct MODULE_API InfiniteLoop : public Loop
{
	InfiniteLoop(Block block) : Loop(block) {};
};

struct MODULE_API ForEachLoop : public Loop
{
	std::string item_name;
	Expression_ptr iterable;

	ForEachLoop(std::string item_name, Expression_ptr iterable, Block block)
		: Loop(block), item_name(item_name), iterable(iterable) {};
};

struct MODULE_API Break : public StatementBase
{
};

struct MODULE_API Continue : public StatementBase
{
};

// Definitions

struct MODULE_API Definition : public StatementBase
{
	bool is_public;
	std::string name;

	Definition(bool is_public, std::string name)
		: is_public(is_public), name(name) {};
};

struct MODULE_API VariableDefinition : public Definition
{
	bool is_mutable;
	Type_ptr type;
	Expression_ptr expression;

	VariableDefinition(bool is_public, bool is_mutable, std::string name, Type_ptr type, Expression_ptr expression)
		: Definition(is_public, name), is_mutable(is_mutable), type(std::move(type)), expression(std::move(expression)) {};
};

struct MODULE_API UDTDefinition : public Definition
{
	std::map<std::string, Type_ptr> member_types;

	UDTDefinition(bool is_public, std::string name, std::map<std::string, Type_ptr> member_types)
		: Definition(is_public, name), member_types(member_types) {};
};

struct MODULE_API FunctionDefinition : public Definition
{
	std::vector<std::pair<std::string, Type_ptr>> arguments;
	std::optional<Type_ptr> return_type;
	Block body;

	FunctionDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block body)
		: Definition(is_public, name), arguments(arguments), return_type(return_type), body(body) {};
};

struct MODULE_API EnumDefinition : public Definition
{
	std::vector<std::string> members;

	EnumDefinition(bool is_public, std::string name, std::vector<std::string> members)
		: Definition(is_public, name), members(members) {};
};

// Import

struct MODULE_API Import : public StatementBase
{
	std::vector<std::string> goods;

	Import(std::vector<std::string> goods) : goods(goods) {};
};

struct MODULE_API ImportCustom : public Import
{
	std::string path;

	ImportCustom(std::string path, std::vector<std::string> goods)
		: Import(goods), path(path) {};
};

struct MODULE_API ImportInBuilt : public Import
{
	std::string module_name;

	ImportInBuilt(std::string module_name, std::vector<std::string> goods)
		: Import(goods), module_name(module_name) {};
};

// Other

struct MODULE_API Return : public StatementBase
{
	std::optional<Expression_ptr> expression;

	Return() : expression(std::nullopt) {};
	Return(Expression_ptr expression)
		: expression(std::make_optional(std::move(expression))) {};
};

struct MODULE_API ExpressionStatement : public StatementBase
{
	Expression_ptr expression;
	ExpressionStatement(Expression_ptr expression)
		: expression(std::move(expression)) {};
};