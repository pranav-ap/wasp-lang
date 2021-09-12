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

	int symbol_id = current_scope->lookup(statement.name)->id;
	int id = create_pool_id(symbol_id);

	emit(OpCode::PUSH_CONSTANT, id);
	emit(OpCode::CREATE_LOCAL, id);

	name_map[id] = statement.name;
	id_map[symbol_id] = id;
}

void Compiler::visit(DeconstructedVariableDefinition const& statement)
{
	// TODO
}

void Compiler::visit(EnumDefinition const& statement)
{
	int symbol_id = current_scope->lookup(statement.name)->id;
	int id = create_pool_id(symbol_id);

	emit(OpCode::PUSH_CONSTANT, id);
	emit(OpCode::CREATE_LOCAL, id);

	name_map[id] = statement.name;
	id_map[symbol_id] = id;
}

void Compiler::visit(FunctionDefinition const& statement)
{
	set_current_scope(statement.scope);
	emit(OpCode::FUNCTION_START);

	for (auto const& arg_name : statement.arguments)
	{
		int symbol_id = current_scope->lookup(arg_name)->id;
		int id = create_pool_id(symbol_id);

		emit(OpCode::PUSH_CONSTANT, id);
		emit(OpCode::CREATE_LOCAL, id);

		name_map[id] = arg_name;
		id_map[symbol_id] = id;
	}

	visit(statement.body);
	emit(OpCode::FUNCTION_STOP);

	auto instructions = leave_subroutine_scope();

	int symbol_id = current_scope->lookup(statement.name)->id;

	auto function_object = MAKE_OBJECT_VARIANT(FunctionDefinitionObject(statement.name, instructions));
	int id = constant_pool->allocate(move(function_object));

	emit(OpCode::PUSH_CONSTANT, id);
	emit(OpCode::CREATE_LOCAL, id);

	function_ids.push_back(id);

	name_map[id] = statement.name;
	id_map[symbol_id] = id;
}

void Compiler::visit(Import const& statement)
{
	set_current_scope(statement.scope);

	for (const auto name : statement.names)
	{
		auto symbol = current_scope->lookup(name);
		ASSERT(symbol->is_builtin, "Expected a Builtin symbol");

		auto builtin_object = current_scope->builtins.at(symbol->name);
		int pool_id = constant_pool->allocate(builtin_object);

		emit(OpCode::PUSH_CONSTANT, pool_id);
		emit(OpCode::CREATE_LOCAL, pool_id);

		name_map[pool_id] = name;
		id_map[symbol->id] = pool_id;
	}
}