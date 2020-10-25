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
struct IfBranch;
struct ElseBranch;
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
struct InfixOperatorDefinition;
struct PrefixOperatorDefinition;
struct PostfixOperatorDefinition;

using Statement = AST_API std::variant <
	std::monostate,

	File,
	Module,
	IfBranch, ElseBranch,
	WhileLoop, ForInLoop,
	Break, Continue,
	Return, YieldStatement,

	VariableDefinition,
	ClassDefinition, AliasDefinition,
	FunctionDefinition, GeneratorDefinition,
	InterfaceDefinition, EnumDefinition,
	FunctionMethodDefinition,
	GeneratorMethodDefinition,

	InfixOperatorDefinition,
	PrefixOperatorDefinition,
	PostfixOperatorDefinition,

	ExpressionStatement,
	Assert,
	Implore, Swear
> ;

using Statement_ptr = AST_API std::shared_ptr<Statement>;
using Block = AST_API std::vector<Statement_ptr>;
using StringVector = AST_API std::vector<std::wstring>;

// Branching

struct AST_API IfBranch
{
	Expression_ptr test;
	Block body;
	std::optional<Statement_ptr> alternative;

	IfBranch(Expression_ptr test, Block body)
		: test(test), body(body) {};

	IfBranch(Expression_ptr test, Block body, Statement_ptr alternative)
		: test(test), body(body), alternative(std::make_optional(alternative)) {};
};

struct AST_API ElseBranch
{
	Block body;

	ElseBranch(Block body) : body(body) {};
};

// Looping

struct AST_API WhileLoop
{
	Expression_ptr expression;
	Block block;

	WhileLoop(Expression_ptr expression, Block block)
		: expression(std::move(expression)), block(block) {};
};

struct AST_API ForInLoop
{
	Expression_ptr lhs_expression;
	Expression_ptr rhs_expression;
	Block block;

	ForInLoop(Expression_ptr lhs_expression, Expression_ptr rhs_expression, Block block)
		: block(block), lhs_expression(std::move(lhs_expression)), rhs_expression(std::move(rhs_expression)) {};
};

struct AST_API Break
{
};

struct AST_API Continue
{
};

// Definitions

struct AST_API VariableDefinition
{
	bool is_public;
	bool is_mutable;
	Expression_ptr expression;

	VariableDefinition(bool is_public, bool is_mutable, Expression_ptr expression)
		: is_public(is_public), is_mutable(is_mutable), expression(std::move(expression)) {};
};

struct AST_API Definition
{
	bool is_public;
	std::wstring name;

	Definition(bool is_public, std::wstring name)
		: is_public(is_public), name(name) {};
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

struct AST_API OperatorDefinition : public Definition
{
	Type_ptr type;
	Block body;

	OperatorDefinition(bool is_public, std::wstring operator_symbol, Type_ptr type, Block body)
		: Definition(is_public, operator_symbol), type(type), body(body) {};
};

struct AST_API InfixOperatorDefinition : public OperatorDefinition
{
	std::wstring lhs_argument;
	std::wstring rhs_argument;

	InfixOperatorDefinition(bool is_public, std::wstring operator_symbol, Type_ptr type, Block body, std::wstring lhs_argument, std::wstring rhs_argument)
		: OperatorDefinition(is_public, operator_symbol, type, body), lhs_argument(lhs_argument), rhs_argument(rhs_argument) {};
};

struct AST_API PrefixOperatorDefinition : public OperatorDefinition
{
	std::wstring argument;

	PrefixOperatorDefinition(bool is_public, std::wstring operator_symbol, Type_ptr type, Block body, std::wstring argument)
		: OperatorDefinition(is_public, operator_symbol, type, body), argument(argument) {};
};

struct AST_API PostfixOperatorDefinition : public OperatorDefinition
{
	std::wstring argument;

	PostfixOperatorDefinition(bool is_public, std::wstring operator_symbol, Type_ptr type, Block body, std::wstring argument)
		: OperatorDefinition(is_public, operator_symbol, type, body), argument(argument) {};
};

struct AST_API MethodDefinition
{
	std::wstring type_name;
	std::wstring name;
	StringVector arguments;
	Type_ptr type;
	Block body;
	bool is_public;

	MethodDefinition(std::wstring type_name, std::wstring name, bool is_public, StringVector arguments, Type_ptr type, Block body)
		: type_name(type_name), name(name), is_public(is_public), arguments(arguments), type(type), body(body) {};
};

struct AST_API FunctionMethodDefinition : public MethodDefinition
{
	FunctionMethodDefinition(std::wstring type_name, std::wstring name, bool is_public, StringVector arguments, Type_ptr type, Block body)
		: MethodDefinition(type_name, name, is_public, arguments, type, body) {};
};

struct AST_API GeneratorMethodDefinition : public MethodDefinition
{
	GeneratorMethodDefinition(std::wstring type_name, std::wstring name, bool is_public, StringVector arguments, Type_ptr type, Block body)
		: MethodDefinition(type_name, name, is_public, arguments, type, body) {};
};

struct AST_API EnumDefinition : public Definition
{
	StringVector members;

	EnumDefinition(bool is_public, std::wstring name, StringVector members)
		: Definition(is_public, name), members(members) {};
};

// Return and Yield

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