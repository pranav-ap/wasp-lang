#pragma once

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
struct Continue;
struct Pass;
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
struct AssertStatement;

using Statement = std::variant<
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

	ExpressionStatement,
	AssertStatement
>;

using Statement_ptr = std::shared_ptr<Statement>;
using Block = std::vector<Statement_ptr>;
using string_vector = std::vector<std::wstring>;

struct StatementBase
{
};

struct Assignment : public StatementBase
{
	ExpressionVector lhs_expressions;
	ExpressionVector rhs_expressions;

	Assignment(ExpressionVector lhs_expressions, ExpressionVector rhs_expressions)
		: lhs_expressions(lhs_expressions), rhs_expressions(rhs_expressions) {};
};

struct Branching : public StatementBase
{
	std::vector<std::pair<Expression_ptr, Block>> branches;
	Block else_block;

	Branching(std::vector<std::pair<Expression_ptr, Block>> branches, Block else_block)
		: branches(branches), else_block(else_block) {};
};

// Looping

struct Loop : public StatementBase
{
	Block block;
	Loop(Block block) : block(block) {};
};

struct WhileLoop : public Loop
{
	Expression_ptr condition;
	WhileLoop(Expression_ptr condition, Block block)
		: Loop(block), condition(std::move(condition)) {};
};

struct ForInLoop : public Loop
{
	std::wstring item_name;
	Expression_ptr iterable;

	ForInLoop(std::wstring item_name, Expression_ptr iterable, Block block)
		: Loop(block), item_name(item_name), iterable(std::move(iterable)) {};
};

struct Break : public StatementBase
{
};

struct Pass : public StatementBase
{
};

struct Continue : public StatementBase
{
};

// Definitions

struct Definition : public StatementBase
{
	bool is_public;
	std::wstring name;

	Definition(bool is_public, std::wstring name)
		: is_public(is_public), name(name) {};
};

struct VariableDefinition : public Definition
{
	bool is_mutable;
	Expression_ptr expression;

	VariableDefinition(bool is_public, bool is_mutable, std::wstring name, Expression_ptr expression)
		: Definition(is_public, name), is_mutable(is_mutable), expression(std::move(expression)) {};
};

struct CallableDefinition : public Definition
{
	std::vector<std::wstring> arguments;
	Block body;

	CallableDefinition(bool is_public, std::wstring name, std::vector<std::wstring> arguments, Block body)
		: Definition(is_public, name), arguments(arguments), body(body) {};
};

struct FunctionDefinition : public CallableDefinition
{
	FunctionDefinition(bool is_public, std::wstring name, std::vector<std::wstring> arguments, Block body)
		: CallableDefinition(is_public, name, arguments, body) {};
};

struct GeneratorDefinition : public CallableDefinition
{
	GeneratorDefinition(bool is_public, std::wstring name, std::vector<std::wstring> arguments, Block body)
		: CallableDefinition(is_public, name, arguments, body) {};
};

struct EnumDefinition : public Definition
{
	string_vector members;

	EnumDefinition(bool is_public, std::wstring name, string_vector members)
		: Definition(is_public, name), members(members) {};
};

// Import

struct Import : public StatementBase
{
	string_vector goods;

	Import(string_vector goods) : goods(goods) {};
};

struct ImportCustom : public Import
{
	std::wstring path;

	ImportCustom(std::wstring path, string_vector goods)
		: Import(goods), path(path) {};
};

struct ImportInBuilt : public Import
{
	std::wstring module_name;

	ImportInBuilt(std::wstring module_name, string_vector goods)
		: Import(goods), module_name(module_name) {};
};

// Other

struct Return : public StatementBase
{
	std::optional<Expression_ptr> expression;

	Return() : expression(std::nullopt) {};
	Return(Expression_ptr expression)
		: expression(std::make_optional(std::move(expression))) {};
};

struct YieldStatement : public StatementBase
{
	std::optional<Expression_ptr> expression;

	YieldStatement() : expression(std::nullopt) {};
	YieldStatement(Expression_ptr expression)
		: expression(std::make_optional(std::move(expression))) {};
};

struct ExpressionStatement : public StatementBase
{
	Expression_ptr expression;
	ExpressionStatement(Expression_ptr expression)
		: expression(std::move(expression)) {};
};

struct AssertStatement : public StatementBase
{
	Expression_ptr expression;
	AssertStatement(Expression_ptr expression)
		: expression(std::move(expression)) {};
};
