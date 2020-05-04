#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

#include "TypeSystem.h"
#include "Expression.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <variant>

struct Assignment;
struct MultipleAssignment;
struct Branching;
struct WhileLoop;
struct ForInLoop;
struct Break;
struct Pass;
struct Continue;
struct VariableDefinition;
struct UDTDefinition;
struct FunctionDefinition;
struct EnumDefinition;
struct ImportCustom;
struct ImportInBuilt;
struct Return;
struct ExpressionStatement;

using Statement = MODULE_API std::variant<
	std::monostate,
	Assignment, MultipleAssignment,
	Branching,
	WhileLoop, ForInLoop,
	Break, Continue,
	VariableDefinition, UDTDefinition, FunctionDefinition, EnumDefinition,
	ImportCustom, ImportInBuilt,
	Return, Pass,
	ExpressionStatement
>;

using Statement_ptr = MODULE_API std::shared_ptr<Statement>;
using Block = MODULE_API std::vector<Statement_ptr>;
using string_vector = MODULE_API std::vector<std::string>;

struct MODULE_API StatementBase
{
};

struct MODULE_API Assignment : public StatementBase
{
	std::string name;
	Expression_ptr expression;

	Assignment(std::string name, Expression_ptr expression)
		: name(name), expression(expression) {};
};

struct MODULE_API MultipleAssignment : public StatementBase
{
	ExpressionVector names;
	ExpressionVector expressions;

	MultipleAssignment(ExpressionVector names, ExpressionVector expressions)
		: names(names), expressions(expressions) {};
};

struct MODULE_API Branching : public StatementBase
{
	std::vector<std::pair<Expression_ptr, Block>> branches;

	Branching(std::vector<std::pair<Expression_ptr, Block>> branches)
		: branches(branches) {};
};

// Looping

struct MODULE_API Loop : public StatementBase
{
	Block block;
	Loop(Block block) : block(block) {};
};

struct MODULE_API WhileLoop : public Loop
{
	Expression_ptr condition;
	WhileLoop(Expression_ptr condition, Block block)
		: Loop(block), condition(condition) {};
};

struct MODULE_API ForInLoop : public Loop
{
	Type_ptr item_type;
	std::string item_name;
	Expression_ptr iterable;

	ForInLoop(Type_ptr item_type, std::string item_name, Expression_ptr iterable, Block block)
		: Loop(block), item_type(item_type), item_name(item_name), iterable(iterable) {};
};

struct MODULE_API Break : public StatementBase
{
};

struct MODULE_API Pass : public StatementBase
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
	string_vector members;

	EnumDefinition(bool is_public, std::string name, string_vector members)
		: Definition(is_public, name), members(members) {};
};

// Import

struct MODULE_API Import : public StatementBase
{
	string_vector goods;

	Import(string_vector goods) : goods(goods) {};
};

struct MODULE_API ImportCustom : public Import
{
	std::string path;

	ImportCustom(std::string path, string_vector goods)
		: Import(goods), path(path) {};
};

struct MODULE_API ImportInBuilt : public Import
{
	std::string module_name;

	ImportInBuilt(std::string module_name, string_vector goods)
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
