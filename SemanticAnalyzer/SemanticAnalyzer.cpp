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

void SemanticAnalyzer::run(Module_ptr ast)
{
	current_scope = make_shared<SymbolScope>(std::nullopt, ScopeType::MODULE);
	ast->scope = current_scope;

	for (auto statement : ast->statements)
	{
		visit(statement);
	}
}

// Statement

void SemanticAnalyzer::visit(Statement_ptr statement)
{
	std::visit(overloaded{
		[&](VariableDefinition& stat) { visit(stat); },
		[&](ExpressionStatement& stat) { visit(stat); },
		[&](IfBranch& stat) { visit(stat); },
		[&](ElseBranch& stat) { visit(stat); },
		[&](WhileLoop& stat) { visit(stat); },
		[&](UntilLoop& stat) { visit(stat); },
		[&](ForInLoop& stat) { visit(stat); },
		[&](Break& stat) { visit(stat); },
		[&](Continue& stat) { visit(stat); },
		[&](Redo& stat) { visit(stat); },
		[&](Return& stat) { visit(stat); },
		[&](Assert& stat) { visit(stat); },
		[&](EnumDefinition& stat) { visit(stat); },
		[&](FunctionDefinition& stat) { visit(stat); },
		[&](AliasDefinition& stat) { visit(stat); },
		[&](ClassDefinition& stat) { visit(stat); },
		[&](Import& stat) { visit(stat); },
		[&](Native& stat) { visit(stat); },

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