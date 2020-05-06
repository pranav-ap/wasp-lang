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
struct Branching;
struct WhileLoop;
struct ForInLoop;
struct Break;
struct Pass;
struct Continue;
struct Return;
struct YieldStatement;
struct VariableDefinition;
struct UDTDefinition;
struct AliasDefinition;
struct FunctionDefinition;
struct GeneratorDefinition;
struct EnumDefinition;
struct ImportCustom;
struct ImportInBuilt;
struct ExpressionStatement;

using Statement = MODULE_API std::variant<
	std::monostate,
	Assignment,

	Branching,
	WhileLoop, ForInLoop,
	Break, Continue,
	Return, YieldStatement, Pass,

	VariableDefinition,
	UDTDefinition, AliasDefinition,
	FunctionDefinition, GeneratorDefinition,
	EnumDefinition,

	ImportCustom, ImportInBuilt,

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
	ExpressionVector lhs_expressions;
	ExpressionVector rhs_expressions;

	Assignment(ExpressionVector lhs_expressions, ExpressionVector rhs_expressions)
		: lhs_expressions(lhs_expressions), rhs_expressions(rhs_expressions) {};
};

struct MODULE_API Branching : public StatementBase
{
	std::vector<std::pair<Expression_ptr, Block>> branches;
	Block else_branch;

	Branching(std::vector<std::pair<Expression_ptr, Block>> branches, Block else_branch)
		: branches(branches), else_branch(else_branch) {};
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
		: Loop(block), condition(std::move(condition)) {};
};

struct MODULE_API ForInLoop : public Loop
{
	Type_ptr item_type;
	std::string item_name;
	Expression_ptr iterable;

	ForInLoop(Type_ptr item_type, std::string item_name, Expression_ptr iterable, Block block)
		: Loop(block), item_type(std::move(item_type)), item_name(item_name), iterable(std::move(iterable)) {};
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
	std::map<std::string, bool> is_public_member;

	UDTDefinition(bool is_public, std::string name, std::map<std::string, Type_ptr> member_types, std::map<std::string, bool> is_public_member)
		: Definition(is_public, name), member_types(member_types), is_public_member(is_public_member) {};
};

struct MODULE_API AliasDefinition : public Definition
{
	Type_ptr type;

	AliasDefinition(bool is_public, std::string name, Type_ptr type)
		: Definition(is_public, name), type(std::move(type)) {};
};

struct MODULE_API CallableDefinition : public Definition
{
	std::vector<std::pair<std::string, Type_ptr>> arguments;
	std::optional<Type_ptr> return_type;
	Block body;

	CallableDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block body)
		: Definition(is_public, name), arguments(arguments), return_type(return_type), body(body) {};
};

struct MODULE_API FunctionDefinition : public CallableDefinition
{
	FunctionDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block body)
		: CallableDefinition(is_public, name, arguments, return_type, body) {};
};

struct MODULE_API GeneratorDefinition : public CallableDefinition
{
	GeneratorDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block body)
		: CallableDefinition(is_public, name, arguments, return_type, body) {};
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

struct MODULE_API YieldStatement : public StatementBase
{
	std::optional<Expression_ptr> expression;

	YieldStatement() : expression(std::nullopt) {};
	YieldStatement(Expression_ptr expression)
		: expression(std::make_optional(std::move(expression))) {};
};

struct MODULE_API ExpressionStatement : public StatementBase
{
	Expression_ptr expression;
	ExpressionStatement(Expression_ptr expression)
		: expression(std::move(expression)) {};
};
