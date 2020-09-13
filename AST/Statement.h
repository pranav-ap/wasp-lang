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
struct UDTDefinition;
struct AliasDefinition;
struct FunctionDefinition;
struct GeneratorDefinition;
struct InterfaceDefinition;
struct EnumDefinition;
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
	UDTDefinition, AliasDefinition,
	FunctionDefinition, GeneratorDefinition,
	InterfaceDefinition, EnumDefinition,

	ExpressionStatement,
	Assert,
	Implore, Swear
>;

using Statement_ptr = AST_API std::shared_ptr<Statement>;
using Block = AST_API std::vector<Statement_ptr>;
using ConditionBlockPairs = AST_API std::vector<std::pair<Expression_ptr, Block>>;
using StringVector = AST_API std::vector<std::wstring>;

struct AST_API File
{
	Block statements;
	void add_statement(Statement_ptr node);
};

using File_ptr = AST_API std::shared_ptr<File>;

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
	std::map<std::wstring, Type_ptr> member_types;
	StringVector public_members;

	UDTDefinition(bool is_public, std::wstring name, std::map<std::wstring, Type_ptr> member_types, StringVector public_members)
		: Definition(is_public, name), member_types(member_types), public_members(public_members) {};
};

struct AST_API InterfaceDefinition : public Definition
{
	std::map<std::wstring, Type_ptr> member_types;

	InterfaceDefinition(bool is_public, std::wstring name, std::map<std::wstring, Type_ptr> member_types)
		: Definition(is_public, name), member_types(member_types) {};
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

struct AST_API Assert
{
	Expression_ptr expression;

	Assert(Expression_ptr expression)
		: expression(std::move(expression)) {};
};

struct AST_API Implore
{
	Expression_ptr expression;

	Implore(Expression_ptr expression)
		: expression(std::move(expression)) {};
};

struct AST_API Swear
{
	Expression_ptr expression;

	Swear(Expression_ptr expression)
		: expression(std::move(expression)) {};
};

struct AST_API Module
{
	std::wstring name;
	bool is_public;
	Block statements;

	Module(std::wstring name, Block statements, bool is_public)
		: name(name), statements(statements), is_public(is_public) { };
};
