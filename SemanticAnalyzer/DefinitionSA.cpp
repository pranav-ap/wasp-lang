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

void SemanticAnalyzer::visit(VariableDefinition& statement)
{
	statement.scope = current_scope;

	ASSERT(holds_alternative<Identifier>(*statement.lhs_expression), "Must be an Identifier");
	auto identifier = get_if<Identifier>(&*statement.lhs_expression);

	Object_ptr type;

	if (statement.type.has_value())
	{
		type = visit(statement.type.value());
	}
	else
	{
		type = visit(statement.lhs_expression);
	}

	Object_ptr rhs_type = visit(statement.rhs_expression);

	ASSERT(type_system->assignable(current_scope, type, rhs_type), "Type mismatch in assignment");

	auto symbol = MAKE_SYMBOL(next_id++, identifier->name, type, statement.is_public, statement.is_mutable);
	current_scope->define(identifier->name, symbol);
}

void SemanticAnalyzer::visit(EnumDefinition& statement)
{
	statement.scope = current_scope;

	auto type = MAKE_OBJECT_VARIANT(EnumType(statement.name, statement.members));
	auto symbol = MAKE_SYMBOL(next_id++, statement.name, type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(AliasDefinition& statement)
{
	statement.scope = current_scope;

	Object_ptr type = visit(statement.type);
	Object_ptr alias_type = MAKE_OBJECT_VARIANT(AliasType(statement.name, type));

	auto symbol = MAKE_SYMBOL(next_id++, statement.name, type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(ClassDefinition& statement)
{
	statement.scope = current_scope;

	std::map<std::wstring, Object_ptr> members;

	for (const auto x : statement.member_types)
	{
		auto type = visit(x.second);
		members.insert({ x.first, type });
	}

	Object_ptr type = MAKE_OBJECT_VARIANT(ClassType(
		statement.name,
		statement.interfaces,
		statement.base_types,
		members,
		statement.is_public_member
	));

	auto symbol = MAKE_SYMBOL(next_id++, statement.name, type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(InterfaceDefinition& statement)
{
	statement.scope = current_scope;

	std::map<std::wstring, Object_ptr> members;

	for (const auto x : statement.member_types)
	{
		auto type = visit(x.second);
		members.insert({ x.first, type });
	}

	Object_ptr type = MAKE_OBJECT_VARIANT(InterfaceType(
		statement.name,
		statement.interfaces,
		statement.base_types,
		members,
		statement.is_public_member
	));

	auto symbol = MAKE_SYMBOL(next_id++, statement.name, type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(FunctionDefinition& statement)
{

	auto type = visit(statement.type);
	auto function_type = type_system->extract_function_type(type);

	auto symbol = MAKE_SYMBOL(next_id++, statement.name, type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);

	enter_scope(ScopeType::FUNCTION);
	statement.scope = current_scope;
	
	int arg_index = 0;

	for (auto const arg_name : statement.arguments)
	{
		Object_ptr type = function_type->input_types.at(arg_index);
		auto symbol = MAKE_SYMBOL(next_id++, arg_name, type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
		current_scope->define(arg_name, symbol);

		arg_index++;
	}

	visit(statement.body);
	leave_scope();
}

void SemanticAnalyzer::visit(GeneratorDefinition& statement)
{
	auto type = visit(statement.type);
	auto generator_type = type_system->extract_generator_type(type);

	auto symbol = MAKE_SYMBOL(next_id++, statement.name, type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);

	enter_scope(ScopeType::GENERATOR);
	statement.scope = current_scope;

	int arg_index = 0;

	for (auto const arg_name : statement.arguments)
	{
		Object_ptr type = generator_type->input_types.at(arg_index);
		auto symbol = MAKE_SYMBOL(next_id++, arg_name, type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
		current_scope->define(arg_name, symbol);

		arg_index++;
	}

	visit(statement.body);
	leave_scope();
}

void SemanticAnalyzer::visit(FunctionMemberDefinition& statement)
{
	auto type = visit(statement.type);
	auto function_method_type = type_system->extract_function_member_type(type);

	auto symbol = MAKE_SYMBOL(next_id++, statement.name, type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);

	enter_scope(ScopeType::CLASS_FUNCTION);
	statement.scope = current_scope;

	int arg_index = 0;

	for (auto const arg_name : statement.arguments)
	{
		Object_ptr type = function_method_type->input_types.at(arg_index);
		auto symbol = MAKE_SYMBOL(next_id++, arg_name, type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
		current_scope->define(arg_name, symbol);

		arg_index++;
	}

	visit(statement.body);
	leave_scope();

	Symbol_ptr class_symbol = current_scope->lookup(statement.type_name);
	auto class_type = type_system->extract_class_type(class_symbol->type);

	ASSERT(class_type->members.contains(statement.name), "Member does not exist");
	class_type->members.at(statement.name) = type;
}

void SemanticAnalyzer::visit(GeneratorMemberDefinition& statement)
{
	auto type = visit(statement.type);
	auto generator_method_type = type_system->extract_generator_member_type(type);

	auto symbol = MAKE_SYMBOL(next_id++, statement.name, type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);

	enter_scope(ScopeType::CLASS_GENERATOR);
	statement.scope = current_scope;

	int arg_index = 0;

	for (auto const arg_name : statement.arguments)
	{
		Object_ptr type = generator_method_type->input_types.at(arg_index);
		auto symbol = MAKE_SYMBOL(next_id++, arg_name, type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
		current_scope->define(arg_name, symbol);

		arg_index++;
	}

	visit(statement.body);
	leave_scope();

	Symbol_ptr class_symbol = current_scope->lookup(statement.type_name);
	auto class_type = type_system->extract_class_type(class_symbol->type);

	ASSERT(class_type->members.contains(statement.name), "Member does not exist");
	class_type->members.at(statement.name) = type;
}
