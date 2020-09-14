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

struct File;
struct Module;
struct Branching;
struct WhileLoop;
struct ForInLoop;
struct Break;
struct Continue;
struct Return;
struct YieldStatement;
struct VariableDefinition;
struct ClassDefinition;
struct AliasDefinition;
struct FunctionDefinition;
struct GeneratorDefinition;
struct InterfaceDefinition;
struct EnumDefinition;
struct FunctionMethodDefinition;
struct GeneratorMethodDefinition;
struct ExpressionStatement;
struct Assert;
struct Implore;
struct Swear;

using Statement = AST_API std::variant<
	std::monostate,

	File,
	Module,
	Branching,
	WhileLoop, ForInLoop,
	Break, Continue,
	Return, YieldStatement,

	VariableDefinition,
	ClassDefinition, AliasDefinition,
	FunctionDefinition, GeneratorDefinition,
	InterfaceDefinition, EnumDefinition,
	FunctionMethodDefinition,
	GeneratorMethodDefinition,

	ExpressionStatement,
	Assert,
	Implore, Swear
>;

using Statement_ptr = AST_API std::shared_ptr<Statement>;
using Block = AST_API std::vector<Statement_ptr>;
using ConditionBlockPairs = AST_API std::vector<std::pair<Expression_ptr, Block>>;
using StringVector = AST_API std::vector<std::wstring>;

// Branching

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
	Expression_ptr pattern;
	Block block;

	WhileLoop(Expression_ptr pattern, Block block)
		: pattern(std::move(pattern)), block(block) {};
};

struct AST_API ForInLoop
{
	Expression_ptr pattern;
	Block block;

	ForInLoop(Expression_ptr pattern, Block block)
		: block(block), pattern(std::move(pattern)) {};
};

struct AST_API Break
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

struct AST_API VariableDefinition
{
	bool is_public;
	bool is_mutable;
	Expression_ptr expression;

	VariableDefinition(bool is_public, bool is_mutable, Expression_ptr expression)
		: is_public(is_public), is_mutable(is_mutable), expression(std::move(expression)) {};
};

struct AST_API UDTDefinition : public Definition
{
	StringVector interfaces;
	StringVector base_types;

	std::map<std::wstring, Type_ptr> member_types;
	StringVector public_members;

	UDTDefinition(bool is_public, std::wstring name, std::map<std::wstring, Type_ptr> member_types, StringVector public_members, std::vector<std::wstring> interfaces, std::vector<std::wstring> base_types)
		: Definition(is_public, name), member_types(member_types), public_members(public_members), interfaces(interfaces), base_types(base_types) {};
};

struct AST_API ClassDefinition : public UDTDefinition
{
	ClassDefinition(bool is_public, std::wstring name, std::map<std::wstring, Type_ptr> member_types, StringVector public_members, std::vector<std::wstring> interfaces, std::vector<std::wstring> base_types)
		: UDTDefinition(is_public, name, member_types, public_members, interfaces, base_types) {};
};

struct AST_API InterfaceDefinition : public UDTDefinition
{
	InterfaceDefinition(bool is_public, std::wstring name, std::map<std::wstring, Type_ptr> member_types, StringVector public_members, std::vector<std::wstring> interfaces, std::vector<std::wstring> base_types)
		: UDTDefinition(is_public, name, member_types, public_members, interfaces, base_types) {};
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

struct AST_API MethodDefinition
{
	std::wstring type_name;
	std::wstring name;
	StringVector arguments;
	Type_ptr type;
	Block body;

	MethodDefinition(std::wstring type_name, std::wstring name, StringVector arguments, Type_ptr type, Block body)
		: type_name(type_name), name(name), arguments(arguments), type(type), body(body) {};
};

struct AST_API FunctionMethodDefinition : public MethodDefinition
{
	FunctionMethodDefinition(std::wstring type_name, std::wstring name, StringVector arguments, Type_ptr type, Block body)
		: MethodDefinition(type_name, name, arguments, type, body) {};
};

struct AST_API GeneratorMethodDefinition : public MethodDefinition
{
	GeneratorMethodDefinition(std::wstring type_name, std::wstring name, StringVector arguments, Type_ptr type, Block body)
		: MethodDefinition(type_name, name, arguments, type, body) {};
};

struct AST_API EnumDefinition : public Definition
{
	StringVector members;

	EnumDefinition(bool is_public, std::wstring name, StringVector members)
		: Definition(is_public, name), members(members) {};
};

// Control

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

// Single Expression Statement

struct AST_API SingleExpressionStatement
{
	Expression_ptr expression;

	SingleExpressionStatement(Expression_ptr expression)
		: expression(std::move(expression)) {};
};

struct AST_API ExpressionStatement : public SingleExpressionStatement
{
	ExpressionStatement(Expression_ptr expression)
		: SingleExpressionStatement(std::move(expression)) {};
};

struct AST_API Assert : public SingleExpressionStatement
{
	Assert(Expression_ptr expression)
		: SingleExpressionStatement(std::move(expression)) {};
};

struct AST_API Implore : public SingleExpressionStatement
{
	Implore(Expression_ptr expression)
		: SingleExpressionStatement(std::move(expression)) {};
};

struct AST_API Swear : public SingleExpressionStatement
{
	Swear(Expression_ptr expression)
		: SingleExpressionStatement(std::move(expression)) {};
};

// Other

struct AST_API Module
{
	std::wstring name;
	bool is_public;
	Block statements;

	Module(std::wstring name, Block statements, bool is_public)
		: name(name), statements(statements), is_public(is_public) { };
};

struct AST_API File
{
	Block statements;
	void add_statement(Statement_ptr node);
};

using File_ptr = AST_API std::shared_ptr<File>;