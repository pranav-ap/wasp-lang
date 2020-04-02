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

Info_ptr Environment::get_info(std::string name)
{
	for (auto scope : scopes)
	{
		if (scope->store.contains(name))
		{
			auto info = scope->store[name];
			return info;
		}
	}

	return nullptr;
}

VariableInfo_ptr Environment::get_variable(string name)
{
	auto info = get_info(name);

	if (info == nullptr)
		return nullptr;

	if (typeid(*info) != typeid(VariableInfo))
		return nullptr;

	auto variable_info = dynamic_pointer_cast<VariableInfo>(info);
	return variable_info;
}

FunctionInfo_ptr Environment::get_function(string name)
{
	auto info = get_info(name);

	if (info == nullptr)
		return nullptr;

	if (typeid(*info) != typeid(FunctionInfo))
		return nullptr;

	auto function_info = dynamic_pointer_cast<FunctionInfo>(info);
	return function_info;
}

UDTInfo_ptr Environment::get_UDT(string name)
{
	auto info = get_info(name);

	if (info == nullptr)
		return nullptr;

	if (typeid(*info) != typeid(UDTInfo))
		return nullptr;

	auto UDT_info = dynamic_pointer_cast<UDTInfo>(info);
	return UDT_info;
}

// Setters

void Environment::set_variable(string name, Object_ptr value)
{
	if (value == nullptr)
		return;

	auto info = get_info(name);

	if (info == nullptr)
		return;

	if (typeid(*info) != typeid(VariableInfo))
		return;

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

	if (result.second == false)
	{
		// ERROR - already existed
	}
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

	if (result.second == false)
	{
		// ERROR
	}
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

	if (result.second == false)
	{
		// ERROR
	}
}