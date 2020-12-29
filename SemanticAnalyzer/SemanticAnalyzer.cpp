#pragma once
#include "pch.h"
#include "SemanticAnalyzer.h"
#include "Symbol.h"
#include "Assertion.h"
#include <variant>
#include <vector>
#include <memory>
#include <algorithm>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_SYMBOL(a, b, c, d, e) std::make_shared<Symbol>(a, b, c, d, e)
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)

#define PUBLIC_SYMBOL true
#define PRIVATE_SYMBOL false
#define MUTABLE_SYMBOL true
#define CONST_SYMBOL false

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::holds_alternative;
using std::wstring;
using std::get_if;
using std::begin;
using std::end;
using std::vector;
using std::make_shared;
using std::move;


void SemanticAnalyzer::execute(Module_ptr ast)
{
	current_scope = make_shared<SymbolScope>(std::nullopt, ScopeType::FILE);
	ast->scope = current_scope;

	for (auto statement : ast->statements)
	{
		visit(statement);
	}

	//leave_scope();
}

// Statement

void SemanticAnalyzer::visit(Statement_ptr statement)	
{
	std::visit(overloaded{
		[&](IfBranch& stat) { visit(stat); },
		[&](ElseBranch& stat) { visit(stat); },
		[&](WhileLoop& stat) { visit(stat); },
		[&](ForInLoop& stat) { visit(stat); },
		[&](Break& stat) { visit(stat); },
		[&](Continue& stat) { visit(stat); },
		[&](Redo& stat) { visit(stat); },
		[&](Return& stat) { visit(stat); },
		[&](YieldStatement& stat) { visit(stat); },
		[&](VariableDefinition& stat) { visit(stat); },
		[&](ClassDefinition& stat) { visit(stat); },
		[&](InterfaceDefinition& stat) { visit(stat); },
		[&](AliasDefinition& stat) { visit(stat); },
		[&](FunctionDefinition& stat) { visit(stat); },
		[&](GeneratorDefinition& stat) { visit(stat); },
		[&](FunctionMemberDefinition& stat) { visit(stat); },
		[&](GeneratorMemberDefinition& stat) { visit(stat); },
		[&](EnumDefinition& stat) { visit(stat); },
		[&](ExpressionStatement& stat) { visit(stat); },
		[&](Assert& stat) { visit(stat); },
		[&](Implore& stat) { visit(stat); },
		[&](Swear& stat) { visit(stat); },
		[&](Namespace& stat) { visit(stat); },

		[&](AnyTypeNode const& node) { return visit(node); },
		[&](IntLiteralTypeNode const& node) { return visit(node); },
		[&](FloatLiteralTypeNode const& node) { return visit(node); },
		[&](StringLiteralTypeNode const& node) { return visit(node); },
		[&](BooleanLiteralTypeNode const& node) { return visit(node); },
		[&](IntTypeNode const& node) { return visit(node); },
		[&](FloatTypeNode const& node) { return visit(node); },
		[&](StringTypeNode const& node) { return visit(node); },
		[&](BooleanTypeNode const& node) { return visit(node); },
		[&](NoneTypeNode const& node) { return visit(node); },
		[&](ListTypeNode const& node) { return visit(node); },
		[&](TupleTypeNode const& node) { return visit(node); },
		[&](SetTypeNode const& node) { return visit(node); },
		[&](MapTypeNode const& node) { return visit(node); },
		[&](VariantTypeNode const& node) { return visit(node); },
		[&](FunctionTypeNode const& node) { return visit(node); },
		[&](GeneratorTypeNode const& node) { return visit(node); },
		[&](FunctionMemberTypeNode const& node) { return visit(node); },
		[&](GeneratorMemberTypeNode const& node) { return visit(node); },
		[&](TypeIdentifierTypeNode const& node) { return visit(node); },

		[&](auto) { FATAL("Never Seen this Statement before!"); }
		}, *statement);
}

void SemanticAnalyzer::visit(std::vector<Statement_ptr>& body)
{
	for (const auto stat : body)
	{
		visit(stat);
	}
}

// Branching 
 
void SemanticAnalyzer::visit(IfBranch& statement)
{
	enter_scope(ScopeType::CONDITIONAL);
	statement.scope = current_scope;

	Object_ptr condition_type = visit(statement.test);
	type_system->expect_condition_type(current_scope, condition_type);

	visit(statement.body);
	leave_scope();

	if (statement.alternative.has_value())
	{
		visit(statement.alternative.value());
	}
}

void SemanticAnalyzer::visit(ElseBranch& statement)
{
	enter_scope(ScopeType::CONDITIONAL);
	statement.scope = current_scope;
	visit(statement.body);
	leave_scope();
}

// Looping

void SemanticAnalyzer::visit(WhileLoop& statement)
{
	Object_ptr condition_type = visit(statement.expression);
	type_system->expect_condition_type(current_scope, condition_type);

	enter_scope(ScopeType::LOOP);
	statement.scope = current_scope;
	visit(statement.body);
	leave_scope();
}

void SemanticAnalyzer::visit(ForInLoop& statement)
{
	Object_ptr right_type = visit(statement.rhs_expression);
	type_system->expect_iterable_type(current_scope, right_type);

	enter_scope(ScopeType::LOOP);
	statement.scope = current_scope;

	const auto [identifier, left_type] = deconstruct_tag_pattern(statement.lhs_expression);

	auto symbol = MAKE_SYMBOL(next_id++, identifier, left_type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
	current_scope->define(identifier, symbol);

	visit(statement.body);
	leave_scope();
}

// Other

void SemanticAnalyzer::visit(Namespace& statement)
{
	/*enter_scope(ScopeType::NAMESPACE);
	visit(statement.statements);
	leave_scope();*/
}

void SemanticAnalyzer::visit(Break& statement)
{
	ASSERT(current_scope->enclosed_in(ScopeType::LOOP), "Break is not expected in this body");
}

void SemanticAnalyzer::visit(Continue& statement)
{
	ASSERT(current_scope->enclosed_in(ScopeType::LOOP), "Continue is not expected in this body");
}

void SemanticAnalyzer::visit(Redo& statement)
{
	ASSERT(current_scope->enclosed_in(ScopeType::LOOP), "Redo is not expected in this body");
}

void SemanticAnalyzer::visit(Return& statement)
{
	ASSERT(current_scope->enclosed_in({
		ScopeType::FUNCTION,
		ScopeType::GENERATOR,
		ScopeType::CLASS_FUNCTION,
		ScopeType::CLASS_GENERATOR
		}), "Return is not expected in this body");

	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
	}
}

void SemanticAnalyzer::visit(YieldStatement& statement)
{
	ASSERT(current_scope->enclosed_in({
		   ScopeType::GENERATOR,
		   ScopeType::CLASS_GENERATOR
		}), "Yield is not expected in this body");

	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
	}
}

void SemanticAnalyzer::visit(Assert& statement)
{
	Object_ptr type = visit(statement.expression);
	type_system->expect_condition_type(current_scope, type);
}

void SemanticAnalyzer::visit(Implore& statement)
{
	Object_ptr type = visit(statement.expression);
	type_system->expect_condition_type(current_scope, type);
}

void SemanticAnalyzer::visit(Swear& statement)
{
	Object_ptr type = visit(statement.expression);
	type_system->expect_condition_type(current_scope, type);
}

void SemanticAnalyzer::visit(ExpressionStatement& statement)
{
	Object_ptr type = visit(statement.expression);
}
