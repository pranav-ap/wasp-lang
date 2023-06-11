#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#include "Expression.h"
#include "TypeNode.h"
#include "SymbolScope.h"
#include "AnnotatedNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <variant>
#include <utility>

struct Module;
struct VariableDefinition;
struct ExpressionStatement;
struct SimpleIfBranch;
struct AssignedIfBranch;
struct ElseBranch;
struct SimpleWhileLoop;
struct AssignedWhileLoop;
struct Break;
struct Continue;
struct Redo;
struct Return;
struct Assert;
struct Implore;
struct Swear;
struct SimpleForInLoop;
struct DeconstructedForInLoop;
struct EnumDefinition;
struct FunctionDefinition;
struct AliasDefinition;
struct ClassDefinition;
struct Import;
struct Native;

using Statement = AST_API std::variant<
	std::monostate,

	Module,
	VariableDefinition,
	ExpressionStatement, SimpleIfBranch, AssignedIfBranch, ElseBranch,
	SimpleWhileLoop, AssignedWhileLoop, Break, Continue, Redo,
	Return, Assert, Implore, Swear,
	SimpleForInLoop, DeconstructedForInLoop, 
	EnumDefinition, FunctionDefinition,  
	Import, Native,
	AliasDefinition, ClassDefinition
>;

using Statement_ptr = AST_API std::shared_ptr<Statement>;
using Block = AST_API std::vector<Statement_ptr>;

struct AST_API Definition : public AnnotatedNode
{
	bool is_public;

	Definition(bool is_public)
		: is_public(is_public) {};
};

// Variable Definitions

struct AST_API VariableDefinition : public Definition
{
	Expression_ptr expression;

	VariableDefinition(bool is_public, Expression_ptr expression)
		: Definition(is_public), expression(expression) {};
};

// Branching

struct AST_API IfBranch : public AnnotatedNode
{
	Block body;
	std::optional<Statement_ptr> alternative;

	IfBranch(Block body)
		: body(body), alternative(std::nullopt) {};

	IfBranch(Block body, Statement_ptr alternative)
		: body(body), alternative(std::make_optional(alternative)) {};
};

struct AST_API SimpleIfBranch : public IfBranch
{
	Expression_ptr test;

	SimpleIfBranch(Expression_ptr test, Block body)
		: IfBranch(body), test(test) {};

	SimpleIfBranch(Expression_ptr test, Block body, Statement_ptr alternative)
		: IfBranch(body, alternative), test(test) {};
};

struct AST_API AssignedIfBranch : public IfBranch
{
	std::wstring name;
	Expression_ptr rhs_expression;
	std::optional<TypeNode_ptr> type_node;

	AssignedIfBranch(std::wstring name, Expression_ptr rhs_expression, Block body)
		: IfBranch(body), name(name), rhs_expression(rhs_expression), type_node(std::nullopt) {};

	AssignedIfBranch(std::wstring name, Expression_ptr rhs_expression, Block body, TypeNode_ptr type_node)
		: IfBranch(body), name(name), rhs_expression(rhs_expression), type_node(type_node) {};

	AssignedIfBranch(std::wstring name, Expression_ptr rhs_expression, Block body, Statement_ptr alternative)
		: IfBranch(body, alternative), name(name), rhs_expression(rhs_expression), type_node(std::nullopt) {};

	AssignedIfBranch(std::wstring name, Expression_ptr rhs_expression, Block body, TypeNode_ptr type_node, Statement_ptr alternative)
		: IfBranch(body, alternative), name(name), rhs_expression(rhs_expression), type_node(type_node) {};
};

struct AST_API ElseBranch : public AnnotatedNode
{
	Block body;
	ElseBranch(Block body) : body(body) {};
};

// Looping - While

struct AST_API WhileLoop : public AnnotatedNode
{
	Block body;

	WhileLoop(Block body)
		: body(body) {};
};

struct AST_API SimpleWhileLoop : public WhileLoop
{
	Expression_ptr test;

	SimpleWhileLoop(Block body, Expression_ptr test)
		: WhileLoop(body), test(std::move(test)) {};
};

struct AST_API AssignedWhileLoop : public WhileLoop
{
	Expression_ptr lhs_expression;
	Expression_ptr rhs_expression;
	std::optional<TypeNode_ptr> type_node;

	AssignedWhileLoop(Block body, Expression_ptr lhs_expression, Expression_ptr rhs_expression)
		: WhileLoop(body),
		lhs_expression(std::move(lhs_expression)),
		rhs_expression(std::move(rhs_expression)),
		type_node(std::nullopt) {};

	AssignedWhileLoop(Block body, Expression_ptr lhs_expression, Expression_ptr rhs_expression, TypeNode_ptr type_node)
		: WhileLoop(body),
		lhs_expression(std::move(lhs_expression)),
		rhs_expression(std::move(rhs_expression)),
		type_node(std::make_optional(type_node)) {};
};

// Looping - Until

struct AST_API UntilLoop : public AnnotatedNode
{
	Block body;

	UntilLoop(Block body)
		: body(body) {};
};

struct AST_API SimpleUntilLoop : public UntilLoop
{
	Expression_ptr test;

	SimpleUntilLoop(Block body, Expression_ptr test)
		: UntilLoop(body), test(std::move(test)) {};
};

struct AST_API AssignedUntilLoop : public UntilLoop
{
	Expression_ptr lhs_expression;
	Expression_ptr rhs_expression;
	std::optional<TypeNode_ptr> type_node;

	AssignedUntilLoop(Block body, Expression_ptr lhs_expression, Expression_ptr rhs_expression)
		: UntilLoop(body),
		lhs_expression(std::move(lhs_expression)),
		rhs_expression(std::move(rhs_expression)),
		type_node(std::nullopt) {};

	AssignedUntilLoop(Block body, Expression_ptr lhs_expression, Expression_ptr rhs_expression, TypeNode_ptr type_node)
		: UntilLoop(body),
		lhs_expression(std::move(lhs_expression)),
		rhs_expression(std::move(rhs_expression)),
		type_node(std::make_optional(type_node)) {};
};

// Looping - For

struct AST_API ForInLoop : public AnnotatedNode
{
	Block body;
	Expression_ptr iterable_expression;
	std::optional<TypeNode_ptr> type_node;

	ForInLoop(Block body, Expression_ptr iterable_expression)
		: body(body),
		iterable_expression(std::move(iterable_expression)),
		type_node(std::nullopt) {};

	ForInLoop(Block body, Expression_ptr iterable_expression, TypeNode_ptr type_node)
		: body(body),
		iterable_expression(std::move(iterable_expression)),
		type_node(std::make_optional(type_node)) {};
};

struct AST_API SimpleForInLoop : public ForInLoop
{
	std::wstring name;

	SimpleForInLoop(Block body, std::wstring name, Expression_ptr iterable_expression)
		: ForInLoop(body, iterable_expression), name(name) {};

	SimpleForInLoop(Block body, std::wstring name, Expression_ptr iterable_expression, TypeNode_ptr type_node)
		: ForInLoop(body, iterable_expression, type_node), name(name) {};
};

struct AST_API DeconstructedForInLoop : public ForInLoop
{
	Expression_ptr deconstruction;

	DeconstructedForInLoop(Block body, Expression_ptr deconstruction, Expression_ptr iterable_expression)
		: ForInLoop(body, iterable_expression),
		deconstruction(std::move(deconstruction)) {};

	DeconstructedForInLoop(Block body, Expression_ptr deconstruction, Expression_ptr rhs_expression, TypeNode_ptr type_node)
		: ForInLoop(body, iterable_expression, type_node),
		deconstruction(std::move(deconstruction)) {};
};

// Single Expression Statement

struct AST_API SingleExpressionStatement : public AnnotatedNode
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

struct AST_API Module : public AnnotatedNode
{
	Block statements;
	void add_statement(Statement_ptr node);
};

using Module_ptr = AST_API std::shared_ptr<Module>;

struct AST_API Return : public AnnotatedNode
{
	std::optional<Expression_ptr> expression;

	Return() : expression(std::nullopt) {};
	Return(Expression_ptr expression)
		: expression(std::make_optional(std::move(expression))) {};
};

struct AST_API Break : public AnnotatedNode
{
};

struct AST_API Continue : public AnnotatedNode
{
};

struct AST_API Redo : public AnnotatedNode
{
};

struct AST_API Import : public AnnotatedNode
{
	std::vector<std::wstring> names;
	std::vector<std::wstring> nicknames;
	std::wstring module_name;

	Import(std::vector<std::wstring> names, std::vector<std::wstring> nicknames, std::wstring module_name)
		: names(names), nicknames(nicknames), module_name(module_name) {};
};

// Definition

struct AST_API FunctionDefinition : public Definition
{
	std::wstring name;
	TypeNode_ptr type;
	StringVector argument_names;
	Block body;

	FunctionDefinition(bool is_public, std::wstring name, StringVector argument_names, TypeNode_ptr type, Block body)
		: Definition(is_public), name(name), argument_names(argument_names), type(type), body(body) {};
};

struct AST_API EnumDefinition : public Definition
{
	std::wstring name;
	std::map<std::wstring, int> members;

	EnumDefinition(bool is_public, std::wstring name, StringVector member_list) : Definition(is_public)
	{
		this->name = name;
		int index = 0;

		for (auto const member : member_list)
		{
			members.insert({ member, index });
			index++;
		}
	};
};

struct AST_API AliasDefinition : public Definition
{
	std::wstring name;
	TypeNode_ptr ref_type;

	AliasDefinition(bool is_public, std::wstring name,
		TypeNode_ptr ref_type)
		: Definition(is_public), name(name), ref_type(ref_type) {};
};

struct AST_API ClassDefinition : public Definition
{
	std::wstring name;
	
	std::map<std::wstring, TypeNode_ptr> member_types;
	std::map<std::wstring, Block> function_body_map;
	std::map<std::wstring, StringVector> function_arguments_names_map;

	std::vector<std::wstring> parent_classes;

	ClassDefinition(bool is_public, std::wstring name, 
		std::map<std::wstring, TypeNode_ptr> member_types, 
		std::map<std::wstring, Block> function_body_map,
		std::map<std::wstring, StringVector> function_arguments_names_map, 
		std::vector<std::wstring> parent_classes)
		: Definition(is_public), 
		name(name), 
		member_types(member_types), 
		function_body_map(function_body_map),
		function_arguments_names_map(function_arguments_names_map),
		parent_classes(parent_classes) {};
};

// Native

struct AST_API Native : public AnnotatedNode
{
	std::wstring module_name;
	std::map<std::wstring, TypeNode_ptr> members;

	Native(std::wstring module_name, std::map<std::wstring, TypeNode_ptr> members)
		: module_name(module_name), members(members) {};
};
