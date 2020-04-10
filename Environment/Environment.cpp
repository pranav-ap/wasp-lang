#pragma once

#include "pch.h"
#include "logger.h"
#include "Environment.h"
#include "Info.h"
#include "ObjectSystem.h"

#include <memory>
#include <string>
#include <list>
#include <utility>
#include <variant>

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::pair;
using std::vector;
using std::map;
using std::optional;
using std::get;
using std::holds_alternative;

Environment::Environment()
{
	enter_branch_scope();
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
	scopes.pop_back();
}

// Getters

InfoVariant_ptr Environment::get_info(string name)
{
	for (auto scope : scopes)
	{
		if (scope->store.contains(name))
		{
			FATAL_IF_NULLPTR(scope->store[name], "Info is nullptr");
			FATAL_IF_TRUE(scope->store[name]->index() == 0, "Info is monostate");
			return scope->store[name];
		}
	}

	FATAL(name + " does not exist!");
}

VariableInfo Environment::get_variable(string name)
{
	auto info = get_info(name);
	FATAL_IF_FALSE(holds_alternative<VariableInfo>(*info), name + " is not a Variable!");

	return get<VariableInfo>(*info);
}

UDTInfo Environment::get_UDT(string name)
{
	auto info = get_info(name);
	FATAL_IF_FALSE(holds_alternative<UDTInfo>(*info), name + " is not a UDT!");

	return get<UDTInfo>(*info);
}

EnumInfo Environment::get_enum(string name)
{
	auto info = get_info(name);
	FATAL_IF_FALSE(holds_alternative<EnumInfo>(*info), name + " is not an Enum!");

	return get<EnumInfo>(*info);
}

// Setters

void Environment::set_variable(string name, ObjectVariant_ptr value)
{
	FATAL_IF_NULLPTR(value, "Cannot set nullptr to variable");
	FATAL_IF_TRUE(value->index() == 0, "Cannot set variable to monostate");

	auto info = get_info(name);
	FATAL_IF_FALSE(holds_alternative<VariableInfo>(*info), name + " is not a Variable!");
	auto variable_info = get<VariableInfo>(*info);

	FATAL_IF_FALSE(variable_info.is_mutable, "Variable is not mutable");
	variable_info.value = value;
}

void Environment::set_element(string name, int index, ObjectVariant_ptr value)
{
	FATAL_IF_NULLPTR(value, "Cannot set nullptr to variable");
	FATAL_IF_TRUE(value->index() == 0, "Cannot set variable to monostate");

	auto info = get_info(name);
	FATAL_IF_FALSE(holds_alternative<VariableInfo>(*info), name + " is not a Variable!");
	auto variable_info = get<VariableInfo>(*info);

	FATAL_IF_FALSE(variable_info.is_mutable, "Vector is not mutable");
	FATAL_IF_FALSE(holds_alternative<VectorObject>(*variable_info.value), "Variable is not a Vector");
	auto vector_object = get<VectorObject>(*variable_info.value);
	vector_object.values[index] = value;
}

// Create

void Environment::create_variable(
	string name,
	bool is_public,
	bool is_mutable,
	Type_ptr type,
	ObjectVariant_ptr value)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		pair<string, InfoVariant_ptr>(
			name,
			make_shared<InfoVariant>(
				VariableInfo(is_public, is_mutable, type, value)
				)
			)
	);

	FATAL_IF_FALSE(result.second, name + " already exists in scope!");
}

void Environment::create_function(
	string name,
	bool is_public,
	vector<pair<string, Type_ptr>> arguments,
	optional<Type_ptr> return_type,
	Block_ptr body)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		pair<string, InfoVariant_ptr>(
			name,
			make_shared<InfoVariant>(
				FunctionInfo(is_public, arguments, return_type, body)
				)
			)
	);

	FATAL_IF_FALSE(result.second, name + " already exists in scope!");
}

void Environment::create_UDT(
	string name,
	bool is_public,
	map<string, Type_ptr> member_types)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		pair<string, InfoVariant_ptr>(
			name,
			make_shared<InfoVariant>(
				UDTInfo(is_public, member_types)
				)
			)
	);

	FATAL_IF_FALSE(result.second, name + " already exists in scope!");
}

void Environment::create_enum(
	string name,
	bool is_public,
	vector<string> member_names)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		pair<string, InfoVariant_ptr>(
			name,
			make_shared<InfoVariant>(
				EnumInfo(is_public, member_names)
				)
			)
	);

	FATAL_IF_FALSE(result.second, name + " already exists in scope!");
}

void Environment::import_builtin(
	std::string name,
	std::function<ObjectVariant_ptr(std::vector<ObjectVariant_ptr>)> func)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		pair<string, InfoVariant_ptr>(
			name,
			make_shared<InfoVariant>(InBuiltFunctionInfo(func))
			)
	);

	FATAL_IF_FALSE(result.second, name + " already exists in scope!");
}

// Utils

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