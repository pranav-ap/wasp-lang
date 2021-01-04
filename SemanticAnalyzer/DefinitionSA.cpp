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

void SemanticAnalyzer::visit(SingleVariableDefinition& statement)
{
	statement.scope = current_scope;

	Object_ptr type;

	if (statement.type.has_value())
	{
		type = visit(statement.type.value());
		Object_ptr rhs_type = visit(statement.rhs_expression);
		ASSERT(type_system->assignable(current_scope, type, rhs_type), "Type mismatch in assignment");
	}
	else
	{
		type = visit(statement.rhs_expression);
	}

	auto symbol = MAKE_SYMBOL(next_id++, statement.name, type, statement.is_public, statement.is_mutable);
	current_scope->define(statement.name, symbol);	
}

void SemanticAnalyzer::visit(DeconstructedVariableDefinition& statement)
{
	statement.scope = current_scope;

	// TODO
	FATAL("TODO - SemanticAnalyzer - DeconstructedVariableDefinition");
}

// Enum

void SemanticAnalyzer::visit(EnumDefinition& statement)
{
	auto type = MAKE_OBJECT_VARIANT(EnumType(statement.name, statement.members));
	auto symbol = MAKE_SYMBOL(next_id++, statement.name, type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);
}
