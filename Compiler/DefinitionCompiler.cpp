#pragma once
#include "pch.h"
#include "Compiler.h"
#include "Assertion.h"
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <utility>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::move;
using std::byte;
using std::wstring;
using std::map;
using std::make_shared;
using std::make_optional;
using std::holds_alternative;
using std::get_if;
using std::vector;
using std::to_wstring;
using std::begin;
using std::end;

void Compiler::visit(SingleVariableDefinition const& statement)
{
	visit(statement.rhs_expression);

	int id = current_scope->lookup(statement.name)->id;
	emit(OpCode::STORE_LOCAL, id); 

	name_map[id] = statement.name;
}

void Compiler::visit(DeconstructedVariableDefinition const& statement)
{
	// TODO
}

void Compiler::visit(EnumDefinition const& statement)
{
	int id = current_scope->lookup(statement.name)->id;
	name_map[id] = statement.name;
}

void Compiler::visit(FunctionDefinition const& statement)
{
	set_current_scope(statement.scope);
	emit(OpCode::START);

	for (auto const& arg_name : statement.arguments)
	{
		int id = current_scope->lookup(arg_name)->id;
		emit(OpCode::STORE_LOCAL, id);

		name_map[id] = arg_name;
	}

	visit(statement.body);
	emit(OpCode::STOP);

	auto instructions = leave_subroutine_scope();
	auto function_id = current_scope->lookup(statement.name)->id;
	auto function_object = MAKE_OBJECT_VARIANT(FunctionObject(statement.name, instructions));

	constant_pool->set(function_id, move(function_object));
	function_ids.push_back(function_id);

	name_map[function_id] = statement.name;
}
