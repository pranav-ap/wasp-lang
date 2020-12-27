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


void Compiler::visit(VariableDefinition const& statement)
{
	visit(statement.rhs_expression);

	ASSERT(holds_alternative<Identifier>(*statement.lhs_expression), "Must be an identifier");
	auto identifier = get_if<Identifier>(&*statement.lhs_expression);

	int id = current_scope->lookup(identifier->name)->id;
	emit(OpCode::STORE_LOCAL, id);

	object_store->name_map[id] = identifier->name;
}

void Compiler::visit(EnumDefinition const& statement)
{
	for (auto [member, index] : statement.members)
	{
		visit(member);
	}

	int total = statement.members.size();
	auto enum_id = current_scope->lookup(statement.name)->id;
	emit(OpCode::MAKE_ENUM, enum_id, total);
}

void Compiler::visit(ClassDefinition const& statement)
{
	map<wstring, int> members;

	for (auto const& [member_name, type] : statement.member_types)
	{
		int id = current_scope->lookup(statement.name + L"::" + member_name)->id;
		members.insert({ member_name , id });
	}

	auto class_symbol = current_scope->lookup(statement.name);
	auto class_id = class_symbol->id;
	auto class_object = MAKE_OBJECT_VARIANT(ClassObject(statement.name, members, class_symbol->type));
	object_store->set(class_id, move(class_object));
	object_store->name_map.insert({ class_id, statement.name });
}

void Compiler::visit(InterfaceDefinition const& statement)
{
	// Do nothing
}

void Compiler::visit(AliasDefinition const& statement)
{
	// Do nothing
}

void Compiler::visit(FunctionDefinition const& statement)
{
	set_current_scope(statement.scope);

	for (auto const& arg_name : statement.arguments)
	{
		int id = current_scope->lookup(arg_name)->id;
		emit(OpCode::STORE_LOCAL, id);
	}

	visit(statement.body);

	auto instructions = leave_subroutine_scope();
	int parameter_count = statement.arguments.size();

	auto function_symbol = current_scope->lookup(statement.name);
	auto function_id = function_symbol->id;
	auto function_object = MAKE_OBJECT_VARIANT(FunctionObject(statement.name, instructions, parameter_count, function_symbol->type));
	object_store->set(function_id, move(function_object));
	object_store->name_map.insert({ function_id, statement.name });
}

void Compiler::visit(GeneratorDefinition const& statement)
{
	set_current_scope(statement.scope);

	for (auto const& arg_name : statement.arguments)
	{
		int id = current_scope->lookup(arg_name)->id;
		emit(OpCode::STORE_LOCAL, id);
	}

	visit(statement.body);

	auto instructions = leave_subroutine_scope();
	int parameter_count = statement.arguments.size();

	auto generator_symbol = current_scope->lookup(statement.name);
	auto generator_id = generator_symbol->id;
	auto generator_object = MAKE_OBJECT_VARIANT(GeneratorObject(statement.name, instructions, parameter_count, generator_symbol->type));
	object_store->set(generator_id, move(generator_object));
	object_store->name_map.insert({ generator_id, statement.name });
}

void Compiler::visit(FunctionMemberDefinition const& statement)
{
	/*int id = current_scope->lookup(statement.type_name)->id;

	Object_ptr object = object_store->get(id);
	ASSERT(holds_alternative<ClassObject>(*object), "Expected class defintion object");
	auto class_object = get_if<ClassObject>(&*object);

	ASSERT(class_object->members.contains(statement.name), "Method name is not found in class definition");
	int method_id = class_object->members.at(statement.name);

	set_current_scope(statement.scope);

	for (auto const& arg_name : statement.arguments)
	{
		int id = current_scope->lookup(arg_name)->id;
		emit(OpCode::STORE_LOCAL, id);
	}

	visit(statement.body);

	auto instructions = leave_subroutine_scope();
	int parameter_count = statement.arguments.size();

	auto type = MAKE_OBJECT_VARIANT(FunctionMemberType(statement.name, statement.members));

	auto function_object = MAKE_OBJECT_VARIANT(FunctionMethodObject(statement.name, instructions, parameter_count, type));
	object_store->set(method_id, move(function_object));
	object_store->name_map.insert({ method_id, statement.name });*/
}

void Compiler::visit(GeneratorMemberDefinition const& statement)
{
	//int id = current_scope->lookup(statement.type_name)->id;

	//Object_ptr object = object_store->get(id);
	//ASSERT(holds_alternative<ClassObject>(*object), "Expected class defintion object");
	//auto class_object = get_if<ClassObject>(&*object);

	//ASSERT(class_object->members.contains(statement.name), "Method name is not found in class definition");
	//int method_id = class_object->members.at(statement.name);

	//set_current_scope(statement.scope);

	//for (auto const& arg_name : statement.arguments)
	//{
	//	int id = current_scope->lookup(arg_name)->id;
	//	emit(OpCode::STORE_LOCAL, id);
	//}

	//visit(statement.body);

	//auto instructions = leave_subroutine_scope();
	//int parameter_count = statement.arguments.size();

	//Symbol_ptr class_symbol = current_scope->lookup(statement.name);
	//auto generator_object = MAKE_OBJECT_VARIANT(GeneratorMethodObject(statement.name, instructions, parameter_count, type));
	//object_store->set(method_id, move(generator_object));
	//object_store->name_map.insert({ method_id, statement.name });
}
