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

SemanticAnalyzer::SemanticAnalyzer()
{
	next_id = 0;
	type_system = make_shared<TypeSystem>();
}

void SemanticAnalyzer::execute(const File_ptr ast)
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

void SemanticAnalyzer::visit(const Statement_ptr statement)	
{
	std::visit(overloaded{
		[&](IfBranch const& stat) { visit(stat); },
		[&](ElseBranch const& stat) { visit(stat); },
		[&](WhileLoop const& stat) { visit(stat); },
		[&](ForInLoop const& stat) { visit(stat); },
		[&](Break const& stat) { visit(stat); },
		[&](Continue const& stat) { visit(stat); },
		[&](Redo const& stat) { visit(stat); },
		[&](Return const& stat) { visit(stat); },
		[&](YieldStatement const& stat) { visit(stat); },
		[&](VariableDefinition const& stat) { visit(stat); },
		[&](ClassDefinition const& stat) { visit(stat); },
		[&](InterfaceDefinition const& stat) { visit(stat); },
		[&](AliasDefinition const& stat) { visit(stat); },
		[&](FunctionDefinition const& stat) { visit(stat); },
		[&](GeneratorDefinition const& stat) { visit(stat); },
		[&](FunctionMemberDefinition const& stat) { visit(stat); },
		[&](GeneratorMemberDefinition const& stat) { visit(stat); },
		[&](EnumDefinition const& stat) { visit(stat); },
		[&](ExpressionStatement const& stat) { visit(stat); },
		[&](Assert const& stat) { visit(stat); },
		[&](Implore const& stat) { visit(stat); },
		[&](Swear const& stat) { visit(stat); },
		[&](Namespace const& stat) { visit(stat); },

		[&](auto) { FATAL("Never Seen this Statement before!"); }
		}, *statement);
}

void SemanticAnalyzer::visit(std::vector<Statement_ptr> const& body)
{
	for (const auto stat : body)
	{
		visit(stat);
	}
}

// Branching 
 
void SemanticAnalyzer::visit(IfBranch const& statement)
{
	enter_scope(ScopeType::CONDITIONAL);

	std::visit(overloaded{
		[&](Assignment const& expr)
		{
			if (holds_alternative<Identifier>(*expr.lhs_expression))
			{
				auto identifier = get_if<Identifier>(&*expr.lhs_expression);
				auto left_type = visit(expr.rhs_expression);
				type_system->expect_condition_type(current_scope, left_type);

				auto symbol = MAKE_SYMBOL(next_id++, identifier->name, left_type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
				current_scope->define(identifier->name, symbol);
			}
			else
			{
				Object_ptr right_type = visit(expr.rhs_expression);

				const auto [identifier, left_type] = deconstruct_type_pattern(expr.lhs_expression);
				ASSERT(type_system->assignable(current_scope, left_type, right_type), "TypeNode mismatch in assignment");
				type_system->expect_condition_type(current_scope, left_type);

				auto symbol = MAKE_SYMBOL(next_id++, identifier, left_type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
				current_scope->define(identifier, symbol);
			}
		},

		[&](auto)
		{
			Object_ptr condition_type = visit(statement.test);
			type_system->expect_condition_type(current_scope, condition_type);
		}
		}, *statement.test);

	visit(statement.body);
	leave_scope();

	if (statement.alternative.has_value())
	{
		visit(statement.alternative.value());
	}
}

void SemanticAnalyzer::visit(ElseBranch const& statement)
{
	enter_scope(ScopeType::CONDITIONAL);
	visit(statement.body);
	leave_scope();
}

// Looping

void SemanticAnalyzer::visit(WhileLoop const& statement)
{
	Object_ptr condition_type = visit(statement.expression);
	type_system->expect_condition_type(current_scope, condition_type);

	enter_scope(ScopeType::LOOP);
	visit(statement.body);
	leave_scope();
}

void SemanticAnalyzer::visit(ForInLoop const& statement)
{
	Object_ptr right_type = visit(statement.rhs_expression);
	type_system->expect_iterable_type(current_scope, right_type);

	enter_scope(ScopeType::LOOP);

	const auto [identifier, left_type] = deconstruct_type_pattern(statement.lhs_expression);

	auto symbol = MAKE_SYMBOL(next_id++, identifier, left_type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
	current_scope->define(identifier, symbol);

	visit(statement.body);
	leave_scope();
}

// Other

void SemanticAnalyzer::visit(Namespace const& statement)
{
	/*enter_scope(ScopeType::NAMESPACE);
	visit(statement.statements);
	leave_scope();*/
}

void SemanticAnalyzer::visit(Break const& statement)
{
	ASSERT(current_scope->enclosed_in(ScopeType::LOOP), "Break is not expected in this body");
}

void SemanticAnalyzer::visit(Continue const& statement)
{
	ASSERT(current_scope->enclosed_in(ScopeType::LOOP), "Continue is not expected in this body");
}

void SemanticAnalyzer::visit(Redo const& statement)
{
	ASSERT(current_scope->enclosed_in(ScopeType::LOOP), "Redo is not expected in this body");
}

void SemanticAnalyzer::visit(Return const& statement)
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

void SemanticAnalyzer::visit(YieldStatement const& statement)
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

void SemanticAnalyzer::visit(Assert const& statement)
{
	Object_ptr type = visit(statement.expression);
	type_system->expect_condition_type(current_scope, type);
}

void SemanticAnalyzer::visit(Implore const& statement)
{
	Object_ptr type = visit(statement.expression);
	type_system->expect_condition_type(current_scope, type);
}

void SemanticAnalyzer::visit(Swear const& statement)
{
	Object_ptr type = visit(statement.expression);
	type_system->expect_condition_type(current_scope, type);
}

void SemanticAnalyzer::visit(ExpressionStatement const& statement)
{
	Object_ptr type = visit(statement.expression);
}
