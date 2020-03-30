#pragma once
#include "pch.h"
#include "Environment.h"
#include "Interpreter.h"

#include <memory>
#include <string>

using std::shared_ptr;
using std::string;

// Getters

shared_ptr<VariableInfo> Interpreter::get_variable(string name)
{
	for (auto env : env_list)
	{
		if (env->variable_store.contains(name))
		{
			return env->variable_store[name];
		}
	}

	return nullptr;
}

shared_ptr<FunctionInfo> Interpreter::get_function(string name)
{
	for (auto env : env_list)
	{
		if (env->fn_store.contains(name))
		{
			return env->fn_store[name];
		}
	}

	return nullptr;
}

shared_ptr<UDTInfo> Interpreter::get_UDT(string name)
{
	for (auto env : env_list)
	{
		if (env->type_store.contains(name))
		{
			return env->type_store[name];
		}
	}

	return nullptr;
}

// Setters

void Interpreter::create_and_set_variable(string name, VariableInfo_ptr info)
{
	auto env = env_list.front();

	if (!env->variable_store.contains(name))
	{
		env->variable_store.insert_or_assign(name, info);
	}
	else
	{
		// ERROR
	}
}

void Interpreter::set_variable(string name, VariableInfo_ptr info)
{
	auto env = env_list.front();

	if (env->variable_store.contains(name))
	{
		env->variable_store.insert_or_assign(name, info);
	}
	else
	{
		// ERROR
	}
}

void Interpreter::set_function(string name, FunctionInfo_ptr info)
{
	auto env = env_list.front();

	if (env->fn_store.contains(name))
	{
		env->fn_store.insert_or_assign(name, info);
	}
	else
	{
		// ERROR
	}
}

void Interpreter::set_UDT(string name, UDTInfo_ptr info)
{
	auto env = env_list.front();

	if (env->type_store.contains(name))
	{
		env->type_store.insert_or_assign(name, info);
	}
	else
	{
		// ERROR
	}
}