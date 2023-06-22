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
struct LetDefinition;
struct ConstDefinition;
struct ExpressionStatement;
struct IfBranch;
struct ElseBranch;
struct WhileLoop;
struct UntilLoop;
struct ForInLoop;
struct Break;
struct Continue;
struct Redo;
struct Return;
struct Assert;
struct EnumDefinition;
struct FunctionDefinition;
struct AliasDefinition;
struct ClassDefinition;
struct Import;
struct Native;
struct Matching;

using Statement = AST_API std::variant<
	std::monostate,

	Module,
	LetDefinition, ConstDefinition,
	ExpressionStatement, 
	IfBranch, ElseBranch,
	WhileLoop, ForInLoop, UntilLoop, 
	Matching,
	Break, Continue, Redo,
	Return, Assert, 
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

struct AST_API LetDefinition : public Definition
{
	Expression_ptr expression;

	LetDefinition(bool is_public, Expression_ptr expression)
		: Definition(is_public), expression(expression) {};
};

struct AST_API ConstDefinition : public Definition
{
	Expression_ptr expression;

	ConstDefinition(bool is_public, Expression_ptr expression)
		: Definition(is_public), expression(expression) {};
};

// Branching

struct AST_API IfBranch : public AnnotatedNode
{
	Block body;
	Expression_ptr test;
	std::optional<Statement_ptr> alternative;

	IfBranch(Expression_ptr test, Block body)
		: body(body), test(test), alternative(std::nullopt) {};

	IfBranch(Expression_ptr test, Block body, Statement_ptr alternative)
		: body(body), test(test), alternative(std::make_optional(alternative)) {};
};

struct AST_API ElseBranch : public AnnotatedNode
{
	Block body;
	ElseBranch(Block body) : body(body) {};
};

// Looping

struct AST_API WhileLoop : public AnnotatedNode
{
	Block body;
	Expression_ptr test;

	WhileLoop(Block body, Expression_ptr test)
		: body(body), test(std::move(test)) {};
};

struct AST_API UntilLoop : public AnnotatedNode
{
	Block body;
	Expression_ptr test;

	UntilLoop(Block body, Expression_ptr test)
		: body(body), test(std::move(test)) {};
};

struct AST_API ForInLoop : public AnnotatedNode
{
	Expression_ptr lhs;
	Expression_ptr iterable_expression;
	Block body;

	ForInLoop(Block body, Expression_ptr lhs, Expression_ptr iterable_expression)
		: body(body),
		lhs(std::move(lhs)),
		iterable_expression(std::move(iterable_expression)) {};
};

// Matching

struct AST_API Matching : public AnnotatedNode
{
	Expression_ptr match_expression;
	std::vector<Expression_ptr> case_expressions;
	std::vector<Block> case_blocks;

	Matching(Expression_ptr match_expression,
			std::vector<Expression_ptr> case_expressions,
			std::vector<Block> case_blocks)
		: match_expression(std::move(match_expression)),
		case_expressions(case_expressions),
		case_blocks(case_blocks) {};
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

	EnumDefinition(
		bool is_public, 
		std::wstring name, 
		StringVector member_list) 
		: Definition(is_public)
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

// Match

struct AST_API Match : public AnnotatedNode
{
	Expression_ptr test;
	std::vector<CaseTest> case_tests;
	std::vector<Block> case_blocks;

	Match(Expression_ptr test,
		std::vector<CaseTest> case_tests,
		std::vector<Block> case_blocks)
		: test(std::move(test)),
		case_tests(case_tests),
		case_blocks(case_blocks) {};
};

