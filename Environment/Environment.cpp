#include "pch.h"
#include "Info.h"
#include "Environment.h"

#include <memory>
#include <string>
#include <list>
#include <utility>

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::pair;
using std::map;

Environment::Environment()
{
	scopes.push_back(make_shared<Scope>());
}

// Getters

shared_ptr<VariableInfo> Environment::get_variable(string name)
{
	for (auto scope : scopes)
	{
		if (scope->variable_store.contains(name))
		{
			return scope->variable_store[name];
		}
	}

	return nullptr;
}

shared_ptr<FunctionInfo> Environment::get_function(string name)
{
	for (auto scope : scopes)
	{
		if (scope->fn_store.contains(name))
		{
			return scope->fn_store[name];
		}
	}

	return nullptr;
}

shared_ptr<UDTInfo> Environment::get_UDT(string name)
{
	for (auto scope : scopes)
	{
		if (scope->type_store.contains(name))
		{
			return scope->type_store[name];
		}
	}

	return nullptr;
}

// Setters

void Environment::set_variable(string name, VariableInfo_ptr info)
{
	auto scope = scopes.front();

	if (!scope->variable_store.contains(name))
	{
		// ERROR
	}

	scope->variable_store[name] = info;
}

void Environment::set_function(string name, FunctionInfo_ptr info)
{
	auto scope = scopes.front();

	if (!scope->fn_store.contains(name))
	{
		// ERROR
	}

	scope->fn_store[name] = info;
}

void Environment::set_UDT(string name, UDTInfo_ptr info)
{
	auto scope = scopes.front();

	if (!scope->type_store.contains(name))
	{
		// ERROR
	}

	scope->type_store[name] = info;
}

// Create and Set

void Environment::create_and_set_variable(
	string name,
	bool is_public,
	bool is_mutable,
	Type_ptr type,
	Object_ptr value)
{
	auto scope = scopes.front();

	auto result = scope->variable_store.insert(
		pair<string, VariableInfo_ptr>(
			name,
			make_shared<VariableInfo>(
				is_public, is_mutable, type, value
				)
			)
	);

	if (result.second == false)
	{
		// ERROR
	}
}

void Environment::create_and_set_function(
	string name,
	bool is_public,
	std::map<std::string, Type_ptr> arguments,
	std::optional<Type_ptr> return_type,
	Block_ptr body)
{
	auto scope = scopes.front();

	auto result = scope->fn_store.insert(
		pair<string, FunctionInfo_ptr>(
			name,
			make_shared<FunctionInfo>(
				is_public,
				arguments,
				return_type,
				body
				)
			)
	);

	if (result.second == false)
	{
		// ERROR
	}
}

void Environment::create_and_set_UDT(
	string name,
	bool is_public,
	std::map<std::string, Type_ptr> member_types)
{
	auto scope = scopes.front();

	auto result = scope->type_store.insert(
		pair<string, UDTInfo_ptr>(
			name,
			make_shared<UDTInfo>(is_public, member_types)
			)
	);

	if (result.second == false)
	{
		// ERROR
	}
}