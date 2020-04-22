#pragma once

#include "pch.h"
#include "Environment.h"
#include "Info.h"
#include "ObjectSystem.h"
#include "Types.h"
#include "CommonAssertion.h"

#include <memory>
#include <string>
#include <set>
#include <list>
#include <utility>
#include <variant>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::pair;
using std::vector;
using std::map;
using std::set;
using std::optional;
using std::get;
using std::get_if;
using std::holds_alternative;

Environment::Environment()
{
	enter_global_scope();
}

void Environment::enter_global_scope()
{
	scopes.push_back(make_shared<GlobalScope>());
}

void Environment::enter_module_scope()
{
	scopes.push_back(make_shared<ModuleScope>());
}

void Environment::enter_branch_scope()
{
	scopes.push_back(make_shared<BranchScope>());
}

void Environment::enter_loop_scope()
{
	scopes.push_back(make_shared<LoopScope>());
}

void Environment::enter_function_scope()
{
	scopes.push_back(make_shared<FunctionScope>());
}

void Environment::leave_scope()
{
	ASSERT(scopes.size() == 1, "You cannot leave the Global Scope");
	scopes.pop_back();
}

// Info Getters

InfoVariant_ptr Environment::get_info(string name)
{
	for (auto scope : scopes)
	{
		if (scope->store.contains(name))
		{
			auto info = scope->store[name];
			NULL_CHECK(info);
			ASSERT(info->index() != 0, "Info is a monostate");
			return info;
		}
	}

	FATAL(name + " does not exist!");
}

VariableInfo* Environment::get_variable_info(string name)
{
	auto info = get_info(name);
	ASSERT(holds_alternative<VariableInfo>(*info), name + " is not a Variable!");

	return get_if<VariableInfo>(&*info);
}

UDTInfo* Environment::get_UDT_info(string name)
{
	auto info = get_info(name);
	ASSERT(holds_alternative<UDTInfo>(*info), name + " is not a UDT!");

	return get_if<UDTInfo>(&*info);
}

EnumInfo* Environment::get_enum_info(string name)
{
	auto info = get_info(name);
	ASSERT(holds_alternative<EnumInfo>(*info), name + " is not an Enum!");

	return get_if<EnumInfo>(&*info);
}

// Variable Getters

ListObject* Environment::get_mutable_list_variable(string name)
{
	auto info = get_variable_info(name);

	ASSERT(info->is_mutable, name + " is  not mutable!");
	ASSERT(holds_alternative<ListType>(*info->type), name + " does not have a Vector Type!");
	ASSERT(holds_alternative<ListObject>(*info->value), name + " does not have a Vector Value!");

	return get_if<ListObject>(&*info->value);
}

TupleObject* Environment::get_mutable_tuple_variable(string name)
{
	auto info = get_variable_info(name);

	ASSERT(info->is_mutable, name + " is  not mutable!");
	ASSERT(holds_alternative<TupleType>(*info->type), name + " does not have a Vector Type!");
	ASSERT(holds_alternative<TupleObject>(*info->value), name + " does not have a Vector Value!");

	return get_if<TupleObject>(&*info->value);
}

DictionaryObject* Environment::get_mutable_UDT_variable(string name)
{
	auto info = get_variable_info(name);

	ASSERT(info->is_mutable, name + " is  not mutable!");
	ASSERT(holds_alternative<UDTType>(*info->type), name + " is not a UDT Type!");
	ASSERT(holds_alternative<DictionaryObject>(*info->value), name + " is not a UDT Value!");

	return get_if<DictionaryObject>(&*info->value);
}

DictionaryObject* Environment::get_mutable_map_variable(string name)
{
	auto info = get_variable_info(name);

	ASSERT(info->is_mutable, name + " is  not mutable!");
	ASSERT(holds_alternative<MapType>(*info->type), name + " is not a Map Type!");
	ASSERT(holds_alternative<DictionaryObject>(*info->value), name + " is not a UDT Value!");

	return get_if<DictionaryObject>(&*info->value);
}

// Setters

void Environment::set_variable(string name, Object_ptr value)
{
	NULL_CHECK(value);
	ASSERT(value->index() != 0, "Cannot set variable = monostate");

	auto variable_info = get_variable_info(name);

	ASSERT(variable_info->is_mutable, "Variable is not mutable");
	variable_info->value = move(value);
}

void Environment::set_element(string name, int index, Object_ptr value)
{
	NULL_CHECK(value);
	ASSERT(value->index() != 0, "Cannot set element = monostate");

	auto vector_object = get_mutable_list_variable(name);
	vector_object->values[index] = value;
}

// Create

void Environment::create_variable(
	string name,
	bool is_public,
	bool is_mutable,
	Type_ptr type,
	Object_ptr value)
{
	NULL_CHECK(type);
	NULL_CHECK(value);

	auto scope = scopes.front();

	auto result = scope->store.insert(
		{
			name,
			make_shared<Info>(VariableInfo(is_public, is_mutable, type, value))
		}
	);

	ASSERT(result.second, name + " already exists in scope!");
}

void Environment::create_function(
	string name,
	bool is_public,
	vector<pair<string, Type_ptr>> arguments,
	optional<Type_ptr> return_type,
	Block body)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		{
			name,
			make_shared<Info>(FunctionInfo(is_public, arguments, return_type, body))
		}
	);

	ASSERT(result.second, name + " already exists in scope!");
}

void Environment::create_UDT(
	string name,
	bool is_public,
	map<string, Type_ptr> member_types)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		{
			name,
			make_shared<Info>(UDTInfo(is_public, member_types))
		}
	);

	ASSERT(result.second, name + " already exists in scope!");
}

void Environment::create_enum(
	string name,
	bool is_public,
	set<string> member_names)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		{
			name,
			make_shared<Info>(EnumInfo(name, is_public, member_names))
		}
	);

	ASSERT(result.second, name + " already exists in scope!");
}

void Environment::import_builtin(
	std::string name,
	std::function<Object_ptr(std::vector<Object_ptr>)> func)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		{
			name,
			make_shared<Info>(InBuiltFunctionInfo(func))
		}
	);

	ASSERT(result.second, name + " already exists in scope!");
}

// Utils

bool Environment::is_inside_module_scope()
{
	for (auto scope : scopes)
		if (typeid(*scope) == typeid(ModuleScope))
			return true;

	return false;
}

bool Environment::is_inside_function_scope()
{
	for (auto scope : scopes)
		if (typeid(*scope) == typeid(FunctionScope))
			return true;

	return false;
}

bool Environment::is_inside_branch_scope()
{
	for (auto scope : scopes)
		if (typeid(*scope) == typeid(BranchScope))
			return true;

	return false;
}

bool Environment::is_inside_loop_scope()
{
	for (auto scope : scopes)
		if (typeid(*scope) == typeid(LoopScope))
			return true;

	return false;
}