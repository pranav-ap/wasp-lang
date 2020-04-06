#include "pch.h"
#include "logger.h"
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
	enter_scope();
}

void Environment::enter_scope()
{
	scopes.push_back(make_shared<Scope>());
}

void Environment::leave_scope()
{
	scopes.pop_back();
}

// Getters

Info_ptr Environment::get_info(std::string name)
{
	for (auto scope : scopes)
	{
		if (scope->store.contains(name))
		{
			return scope->store[name];
		}
	}

	string message = name + " does not exist!";
	FATAL(message);
}

VariableInfo_ptr Environment::get_variable(string name)
{
	auto info = get_info(name);

	if (typeid(*info) != typeid(VariableInfo))
	{
		string message = name + " is not a Variable!";
		FATAL(message);
	}

	return dynamic_pointer_cast<VariableInfo>(info);
}

FunctionInfo_ptr Environment::get_function(string name)
{
	auto info = get_info(name);

	if (typeid(*info) != typeid(FunctionInfo))
	{
		string message = name + " is not a Function!";
		FATAL(message);
	}

	return dynamic_pointer_cast<FunctionInfo>(info);
}

UDTInfo_ptr Environment::get_UDT(string name)
{
	auto info = get_info(name);

	if (typeid(*info) != typeid(UDTInfo))
	{
		string message = name + " is not a UDT!";
		FATAL(message);
	}

	return dynamic_pointer_cast<UDTInfo>(info);
}

// Setters

void Environment::set_variable(string name, Object_ptr value)
{
	FATAL_IF_TRUE(value == nullptr, "Cannot set variable to nullptr");

	auto info = get_info(name);

	if (typeid(*info) != typeid(VariableInfo))
	{
		string message = name + " is not a Variable!";
		FATAL(message);
	}

	auto variable_info = dynamic_pointer_cast<VariableInfo>(info);
	variable_info->value = value;
}

// Create and Set

void Environment::create_variable(
	string name,
	bool is_public,
	bool is_mutable,
	Type_ptr type,
	Object_ptr value)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		pair<string, Info_ptr>(
			name,
			make_shared<VariableInfo>(is_public, is_mutable, type, value)
			)
	);

	string message = name + " already exists in scope!";
	FATAL_IF_FALSE(result.second, message);
}

void Environment::create_function(
	string name,
	bool is_public,
	std::map<std::string, Type_ptr> arguments,
	std::optional<Type_ptr> return_type,
	Block_ptr body)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		pair<string, Info_ptr>(
			name,
			make_shared<FunctionInfo>(is_public, arguments, return_type, body)
			)
	);

	string message = name + " already exists in scope!";
	FATAL_IF_FALSE(result.second, message);
}

void Environment::create_UDT(
	string name,
	bool is_public,
	std::map<std::string, Type_ptr> member_types)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		pair<string, Info_ptr>(
			name,
			make_shared<UDTInfo>(is_public, member_types)
			)
	);

	string message = name + " already exists in scope!";
	FATAL_IF_FALSE(result.second, message);
}

void Environment::create_enum(
	string name,
	bool is_public,
	std::vector<std::string> member_names)
{
	auto scope = scopes.front();

	auto result = scope->store.insert(
		pair<string, Info_ptr>(
			name,
			make_shared<EnumInfo>(is_public, member_names)
			)
	);

	string message = name + " already exists in scope!";
	FATAL_IF_FALSE(result.second, message);
}