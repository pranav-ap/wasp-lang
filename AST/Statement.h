#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#include "Expression.h"
#include "Type_System.h"
#include "uuid.h"

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

using Statement = AST_API std::variant<
	std::monostate,

	Assignment,

	Branching,
	WhileLoop, ForInLoop,
	Break, Continue, Pass,
	Return, YieldStatement,

	VariableDefinition,
	UDTDefinition, AliasDefinition,
	FunctionDefinition, GeneratorDefinition,
	EnumDefinition,

	ImportCustom, ImportInBuilt,

	ExpressionStatement,
	AssertStatement
>;

using Statement_ptr = AST_API std::shared_ptr<Statement>;
using Block = AST_API std::vector<Statement_ptr>;
using StringVector = AST_API std::vector<std::wstring>;

struct StatementBase
{
	std::wstring id;
	StatementBase()
	{
		id = uuids::to_string<wchar_t>(uuids::uuid_system_generator{}());
	};
};

struct AST_API BlockStat : public StatementBase
{
	Block block;
	BlockStat(Block block) : block(block) {};
};

struct AST_API Assignment : public StatementBase
{
	ExpressionVector lhs_expressions;
	ExpressionVector rhs_expressions;

	Assignment(ExpressionVector lhs_expressions, ExpressionVector rhs_expressions)
		: lhs_expressions(lhs_expressions), rhs_expressions(rhs_expressions) {};
};

struct AST_API Branching : public StatementBase
{
	std::vector<std::pair<Expression_ptr, Block>> branches;
	Block else_block;

	Branching(std::vector<std::pair<Expression_ptr, Block>> branches, Block else_block)
		: branches(branches), else_block(else_block) {};
};

// Looping

struct AST_API Loop : public StatementBase
{
	Block block;
	Loop(Block block) : block(block) {};
};

struct AST_API WhileLoop : public Loop
{
	Expression_ptr condition;
	WhileLoop(Expression_ptr condition, Block block)
		: Loop(block), condition(std::move(condition)) {};
};

struct AST_API ForInLoop : public Loop
{
	Type_ptr item_type;
	std::wstring item_name;
	Expression_ptr iterable;

	ForInLoop(Type_ptr item_type, std::wstring item_name, Expression_ptr iterable, Block block)
		: Loop(block), item_type(std::move(item_type)), item_name(item_name), iterable(std::move(iterable)) {};
};

struct AST_API Break : public StatementBase
{
};

struct AST_API Pass : public StatementBase
{
};

struct AST_API Continue : public StatementBase
{
};

// Definitions

struct AST_API Definition : public StatementBase
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
	std::map<std::wstring, bool> is_public_member;

	UDTDefinition(bool is_public, std::wstring name, std::map<std::wstring, Type_ptr> member_types, std::map<std::wstring, bool> is_public_member)
		: Definition(is_public, name), member_types(member_types), is_public_member(is_public_member) {};
};

struct AST_API AliasDefinition : public Definition
{
	Type_ptr type;

	AliasDefinition(bool is_public, std::wstring name, Type_ptr type)
		: Definition(is_public, name), type(std::move(type)) {};
};

struct AST_API CallableDefinition : public Definition
{
	std::vector<std::pair<std::wstring, Type_ptr>> arguments;
	std::optional<Type_ptr> return_type;
	Block body;

	CallableDefinition(bool is_public, std::wstring name, std::vector<std::pair<std::wstring, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block body)
		: Definition(is_public, name), arguments(arguments), return_type(return_type), body(body) {};
};

struct AST_API FunctionDefinition : public CallableDefinition
{
	FunctionDefinition(bool is_public, std::wstring name, std::vector<std::pair<std::wstring, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block body)
		: CallableDefinition(is_public, name, arguments, return_type, body) {};
};

struct AST_API GeneratorDefinition : public CallableDefinition
{
	GeneratorDefinition(bool is_public, std::wstring name, std::vector<std::pair<std::wstring, Type_ptr>> arguments, std::optional<Type_ptr> return_type, Block body)
		: CallableDefinition(is_public, name, arguments, return_type, body) {};
};

struct AST_API EnumDefinition : public Definition
{
	StringVector members;

	EnumDefinition(bool is_public, std::wstring name, StringVector members)
		: Definition(is_public, name), members(members) {};
};

// Import

struct AST_API Import : public StatementBase
{
	StringVector goods;

	Import(StringVector goods) : goods(goods) {};
};

struct AST_API ImportCustom : public Import
{
	std::wstring path;

	ImportCustom(std::wstring path, StringVector goods)
		: Import(goods), path(path) {};
};

struct AST_API ImportInBuilt : public Import
{
	std::wstring module_name;

	ImportInBuilt(std::wstring module_name, StringVector goods)
		: Import(goods), module_name(module_name) {};
};

// Other

struct AST_API Return : public StatementBase
{
	std::optional<Expression_ptr> expression;

	Return() : expression(std::nullopt) {};
	Return(Expression_ptr expression)
		: expression(std::make_optional(std::move(expression))) {};
};

struct AST_API YieldStatement : public StatementBase
{
	std::optional<Expression_ptr> expression;

	YieldStatement() : expression(std::nullopt) {};
	YieldStatement(Expression_ptr expression)
		: expression(std::make_optional(std::move(expression))) {};
};

struct AST_API ExpressionStatement : public StatementBase
{
	Expression_ptr expression;
	ExpressionStatement(Expression_ptr expression)
		: expression(std::move(expression)) {};
};

struct AST_API AssertStatement : public StatementBase
{
	Expression_ptr expression;
	AssertStatement(Expression_ptr expression)
		: expression(std::move(expression)) {};
};
