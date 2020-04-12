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

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::pair;
using std::vector;
using std::map;
using std::set;
using std::optional;
using std::get;
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

// Getters

InfoVariant_ptr Environment::get_info(string name)
{
	for (auto scope : scopes)
	{
		if (scope->store.contains(name))
		{
			ASSERT(scope->store[name]->index() != 0, "Info is a monostate");
			return scope->store[name];
		}
	}

	FATAL(name + " does not exist!");
}

VariableInfo& Environment::get_variable_info(string name)
{
	auto info = get_info(name);
	ASSERT(holds_alternative<VariableInfo>(*info), name + " is not a Variable!");

	return get<VariableInfo>(*info);
}

UDTInfo& Environment::get_UDT_info(string name)
{
	auto info = get_info(name);
	ASSERT(holds_alternative<UDTInfo>(*info), name + " is not a UDT!");

	return get<UDTInfo>(*info);
}

EnumInfo& Environment::get_enum_info(string name)
{
	auto info = get_info(name);
	ASSERT(holds_alternative<EnumInfo>(*info), name + " is not an Enum!");

	return get<EnumInfo>(*info);
}

VectorObject& Environment::get_mutable_vector_variable(string name)
{
	auto info = get_variable_info(name);

	ASSERT(info.is_mutable, name + " is  not mutable!");
	ASSERT(holds_alternative<VectorType>(*info.type), name + " does not have a Vector Type!");
	ASSERT(holds_alternative<VectorObject>(*info.value), name + " does not have a Vector Value!");

	return get<VectorObject>(*info.value);
}

UDTObject& Environment::get_mutable_UDT_variable(string name)
{
	auto info = get_variable_info(name);

	ASSERT(info.is_mutable, name + " is  not mutable!");
	ASSERT(holds_alternative<UDTType>(*info.type), name + " is not a UDT Type!");
	ASSERT(holds_alternative<UDTObject>(*info.value), name + " is not a UDT Value!");

	return get<UDTObject>(*info.value);
}

// Setters

void Environment::set_variable(string name, ObjectVariant_ptr value)
{
	auto variable_info = get_variable_info(name);
	ASSERT(variable_info.is_mutable, name + " is not mutable!");

	ASSERT(!value, "Cannot set variable = nullptr");
	ASSERT(value->index() != 0, "Cannot set variable = monostate");

	variable_info.value = move(value);
}

void Environment::set_element(string name, int index, ObjectVariant_ptr value)
{
	ASSERT(!value, "Cannot set element = nullptr");
	ASSERT(value->index() != 0, "Cannot set element = monostate");

	auto vector_object = get_mutable_vector_variable(name);
	vector_object.values[index] = value;
}

// Create

void Environment::create_variable(
	string name,
	bool is_public,
	bool is_mutable,
	TypeVariant_ptr type,
	ObjectVariant_ptr value)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		{
			name,
			make_shared<InfoVariant>(VariableInfo(is_public, is_mutable, type, value))
		}
	);

	ASSERT(result.second, name + " already exists in scope!");
}

void Environment::create_function(
	string name,
	bool is_public,
	vector<pair<string, TypeVariant_ptr>> arguments,
	optional<TypeVariant_ptr> return_type,
	Block_ptr body)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		{
			name,
			make_shared<InfoVariant>(FunctionInfo(is_public, arguments, return_type, body))
		}
	);

	ASSERT(result.second, name + " already exists in scope!");
}

void Environment::create_UDT(
	string name,
	bool is_public,
	map<string, TypeVariant_ptr> member_types)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		{
			name,
			make_shared<InfoVariant>(UDTInfo(is_public, member_types))
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
			make_shared<InfoVariant>(EnumInfo(name, is_public, member_names))
		}
	);

	ASSERT(result.second, name + " already exists in scope!");
}

void Environment::import_builtin(
	std::string name,
	std::function<ObjectVariant_ptr(std::vector<ObjectVariant_ptr>)> func)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		{
			name,
			make_shared<InfoVariant>(InBuiltFunctionInfo(func))
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