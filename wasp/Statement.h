#pragma once

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
using string_vector = std::vector<std::string>;

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
	Type_ptr item_type;
	std::string item_name;
	Expression_ptr iterable;

	ForInLoop(Type_ptr item_type, std::string item_name, Expression_ptr iterable, Block block)
		: Loop(block), item_type(std::move(item_type)), item_name(item_name), iterable(std::move(iterable)) {};
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
	std::string name;

	Definition(bool is_public, std::string name)
		: is_public(is_public), name(name) {};
};

struct VariableDefinition : public Definition
{
	bool is_mutable;
	Type_ptr type;
	Expression_ptr expression;

	VariableDefinition(bool is_public, bool is_mutable, std::string name, Type_ptr type, Expression_ptr expression)
		: Definition(is_public, name), is_mutable(is_mutable), type(std::move(type)), expression(std::move(expression)) {};
};

struct UDTDefinition : public Definition
{
	std::map<std::string, Type_ptr> member_types;
	std::map<std::string, bool> is_public_member;

	UDTDefinition(bool is_public, std::string name, std::map<std::string, Type_ptr> member_types, std::map<std::string, bool> is_public_member)
		: Definition(is_public, name), member_types(member_types), is_public_member(is_public_member) {};
};

struct AliasDefinition : public Definition
{
	Type_ptr type;

	AliasDefinition(bool is_public, std::string name, Type_ptr type)
		: Definition(is_public, name), type(std::move(type)) {};
};

struct CallableDefinition : public Definition
{
	std::vector<std::pair<std::string, Type_ptr>> arguments;
	std::optional<Type_ptr> return_type;
	Block body;

	CallableDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block body)
		: Definition(is_public, name), arguments(arguments), return_type(return_type), body(body) {};
};

struct FunctionDefinition : public CallableDefinition
{
	FunctionDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block body)
		: CallableDefinition(is_public, name, arguments, return_type, body) {};
};

struct GeneratorDefinition : public CallableDefinition
{
	GeneratorDefinition(bool is_public, std::string name, std::vector<std::pair<std::string, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block body)
		: CallableDefinition(is_public, name, arguments, return_type, body) {};
};

struct EnumDefinition : public Definition
{
	string_vector members;

	EnumDefinition(bool is_public, std::string name, string_vector members)
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
	std::string path;

	ImportCustom(std::string path, string_vector goods)
		: Import(goods), path(path) {};
};

struct ImportInBuilt : public Import
{
	std::string module_name;

	ImportInBuilt(std::string module_name, string_vector goods)
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
