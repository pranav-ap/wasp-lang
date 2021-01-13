#pragma once
#include "pch.h"
#include "SemanticAnalyzer.h"
#include "Symbol.h"
#include "Assertion.h"
#include "Builtins.h"
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

void SemanticAnalyzer::visit(ExpressionStatement& statement)
{
	Object_ptr type = visit(statement.expression);
}

// Branching

void SemanticAnalyzer::visit(SimpleIfBranch& statement)
{
	enter_scope(ScopeType::BRANCH);
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

void SemanticAnalyzer::visit(AssignedIfBranch& statement)
{
	enter_scope(ScopeType::BRANCH);
	statement.scope = current_scope;

	Object_ptr right_type = visit(statement.rhs_expression);
	type_system->expect_condition_type(current_scope, right_type);

	Object_ptr left_type;

	if (statement.type_node.has_value())
	{
		left_type = visit(statement.type_node.value());
		type_system->expect_condition_type(current_scope, left_type);
		ASSERT(type_system->assignable(current_scope, left_type, right_type), "Type mismatch in AssignedIfBranch assignment");
	}
	else
	{
		left_type = right_type;
	}

	auto symbol = MAKE_SYMBOL(next_id++, statement.name, left_type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
	current_scope->define(statement.name, symbol);

	visit(statement.body);
	leave_scope();

	if (statement.alternative.has_value())
	{
		visit(statement.alternative.value());
	}
}

void SemanticAnalyzer::visit(ElseBranch& statement)
{
	enter_scope(ScopeType::EXPRESSION);
	statement.scope = current_scope;
	visit(statement.body);
	leave_scope();
}

// Looping

void SemanticAnalyzer::visit(SimpleWhileLoop& statement)
{
	Object_ptr condition_type = visit(statement.test);
	type_system->expect_condition_type(current_scope, condition_type);

	enter_scope(ScopeType::LOOP);
	statement.scope = current_scope;
	visit(statement.body);
	leave_scope();
}

void SemanticAnalyzer::visit(AssignedWhileLoop& statement)
{
}

// Looping - for loop

void SemanticAnalyzer::visit(SimpleForInLoop& statement)
{
	Object_ptr right_type = visit(statement.iterable_expression);
	type_system->expect_iterable_type(current_scope, right_type);

	enter_scope(ScopeType::LOOP);
	statement.scope = current_scope;

	Object_ptr left_type;

	if (statement.type_node.has_value())
	{
		left_type = visit(statement.type_node.value());
	}
	else
	{
		type_system->expect_spreadable_type(current_scope, right_type);
		left_type = type_system->spread_type(right_type);
	}

	auto symbol = MAKE_SYMBOL(next_id++, statement.name, left_type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
	current_scope->define(statement.name, symbol);

	visit(statement.body);
	leave_scope();
}

void SemanticAnalyzer::visit(DeconstructedForInLoop& statement)
{
	// TODO
}

void SemanticAnalyzer::visit(Scenario& statement)
{
	enter_scope(ScopeType::TEST);
	statement.scope = current_scope;

	visit(statement.body);
	leave_scope();
}

void SemanticAnalyzer::visit(Test& statement)
{
	enter_scope(ScopeType::TEST);
	statement.scope = current_scope;

	visit(statement.body);
	leave_scope();
}

void SemanticAnalyzer::visit(Import& statement)
{
	statement.scope = current_scope;

	for (const auto name : statement.names)
	{
		ASSERT(builtins_manager->exists(statement.module_name, name), "Native Module does not contains this member");
		auto type = builtins_manager->get_native_type(statement.module_name, name);

		auto symbol = MAKE_SYMBOL(next_id++, name, type, PRIVATE_SYMBOL, CONST_SYMBOL);
		symbol->is_builtin = true;

		current_scope->define(name, symbol);
		current_scope->builtins[name] = builtins_manager->get_native_object(statement.module_name, name);
	}
}

void SemanticAnalyzer::visit(Native& statement)
{
	statement.scope = current_scope;

	for (auto member : statement.members)
	{
		auto type = visit(member.second);
		builtins_manager->set_native_type(statement.module_name, member.first, type);
	}
}

// simple stuff

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
	ASSERT(current_scope->enclosed_in(ScopeType::FUNCTION), "Return is not expected in this body");

	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
	}
}

void SemanticAnalyzer::visit(YieldStatement& statement)
{
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