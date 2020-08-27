#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#include "Expression.h"
#include "Type.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <variant>
#include <utility>

struct Module;
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
struct ExpressionStatement;
struct AssertStatement;

using Statement = AST_API std::variant <
	std::monostate,

	Module,

	Assignment,

	Branching,
	WhileLoop, ForInLoop,
	Break, Continue, Pass,
	Return, YieldStatement,

	VariableDefinition,
	UDTDefinition, AliasDefinition,
	FunctionDefinition, GeneratorDefinition,
	EnumDefinition,

	ExpressionStatement,
	AssertStatement
>;

using Statement_ptr = AST_API std::shared_ptr<Statement>;
using Block = AST_API std::vector<Statement_ptr>;
using ConditionBlockPairs = AST_API std::vector<std::pair<Expression_ptr, Block>>;
using StringVector = AST_API std::vector<std::wstring>;

struct AST_API Module
{
	Block statements;
	void add_statement(Statement_ptr node);
};

using Module_ptr = AST_API std::shared_ptr<Module>;

struct AST_API Assignment
{
	std::wstring name;
	Expression_ptr expression;

	Assignment(std::wstring name, Expression_ptr expression)
		: name(name), expression(expression) {};
};

struct AST_API Branching
{
	ConditionBlockPairs branches;
	Block else_block;

	Branching(ConditionBlockPairs branches, Block else_block)
		: branches(branches), else_block(else_block) {};
};
// Looping

struct AST_API WhileLoop
{
	Expression_ptr condition;
	Block block;

	WhileLoop(Expression_ptr condition, Block block)
		: condition(std::move(condition)), block(block) {};
};

struct AST_API ForInLoop
{
	Type_ptr item_type;
	std::wstring item_name;
	Expression_ptr iterable;
	Block block;

	ForInLoop(Type_ptr item_type, std::wstring item_name, Expression_ptr iterable, Block block)
		: block(block), item_type(std::move(item_type)), item_name(item_name), iterable(std::move(iterable)) {};
};

struct AST_API Break
{
};

struct AST_API Pass
{
};

struct AST_API Continue
{
};

// Definitions

struct AST_API Definition
{
	bool is_public;
	std::wstring name;

	Definition(bool is_public, std::wstring name)
		: is_public(is_public), name(name) {};
};

struct AST_API VariableDefinition : public Definition
{
	bool is_mutable;
	Type_ptr type;
	Expression_ptr expression;

	VariableDefinition(bool is_public, bool is_mutable, std::wstring name, Type_ptr type, Expression_ptr expression)
		: Definition(is_public, name), is_mutable(is_mutable), type(std::move(type)), expression(std::move(expression)) {};
};

struct AST_API UDTDefinition : public Definition
{
	std::map<std::wstring, Type_ptr> member_types;
	StringVector public_members;

	UDTDefinition(bool is_public, std::wstring name, std::map<std::wstring, Type_ptr> member_types, StringVector public_members)
		: Definition(is_public, name), member_types(member_types), public_members(public_members) {};
};

struct AST_API AliasDefinition : public Definition
{
	Type_ptr type;

	AliasDefinition(bool is_public, std::wstring name, Type_ptr type)
		: Definition(is_public, name), type(std::move(type)) {};
};

struct AST_API CallableDefinition : public Definition
{
	StringVector arguments;
	Type_ptr type;
	Block block;

	CallableDefinition(bool is_public, std::wstring name, StringVector arguments, Type_ptr type, Block body)
		: Definition(is_public, name), arguments(arguments), type(type), block(body) {};
};

struct AST_API FunctionDefinition : public CallableDefinition
{
	FunctionDefinition(bool is_public, std::wstring name, StringVector arguments, Type_ptr type, Block body)
		: CallableDefinition(is_public, name, arguments, type, body) {};
};

struct AST_API GeneratorDefinition : public CallableDefinition
{
	GeneratorDefinition(bool is_public, std::wstring name, StringVector arguments, Type_ptr type, Block body)
		: CallableDefinition(is_public, name, arguments, type, body) {};
};

struct AST_API EnumDefinition : public Definition
{
	StringVector members;

	EnumDefinition(bool is_public, std::wstring name, StringVector members)
		: Definition(is_public, name), members(members) {};
};

// Other

struct AST_API Return
{
	std::optional<Expression_ptr> expression;

	Return() : expression(std::nullopt) {};
	Return(Expression_ptr expression)
		: expression(std::make_optional(std::move(expression))) {};
};

struct AST_API YieldStatement
{
	std::optional<Expression_ptr> expression;

	YieldStatement() : expression(std::nullopt) {};
	YieldStatement(Expression_ptr expression)
		: expression(std::make_optional(std::move(expression))) {};
};

struct AST_API ExpressionStatement
{
	Expression_ptr expression;

	ExpressionStatement(Expression_ptr expression)
		: expression(std::move(expression)) {};
};

struct AST_API AssertStatement
{
	Expression_ptr expression;

	AssertStatement(Expression_ptr expression)
		: expression(std::move(expression)) {};
};
