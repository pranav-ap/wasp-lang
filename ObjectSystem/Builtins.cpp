#pragma once
#include "pch.h"
#include "Builtins.h"
#include "Assertion.h"
#include <string>
#include <iostream>

#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define THROW(message) return std::make_shared<Object>(ErrorObject(message))
#define VOID std::make_shared<Object>(ReturnObject())

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::wcout;
using std::cin;
using std::endl;
using std::wstring;


Object_ptr io::print(std::vector<Object_ptr> arguments)
{
	if (!arguments.size() == 1)
	{
		THROW(L"echo(..) takes in one argument");
	}

	auto out = stringify_object(arguments[0]);
	wcout << out << endl;

	return VOID;
}

// BuiltinsManager

BuiltinsManager::BuiltinsManager()
{
	Builtins_ObjectTable io_object_table = {
	   { L"print", MAKE_OBJECT_VARIANT(BuiltInFunctionObject(L"io", L"print", io::print)) }
	};

	Builtins_ObjectTable math_object_table = {
	   { L"pi", MAKE_OBJECT_VARIANT(FloatObject(3.14)) }
	};

	module_table = {
		{ L"io", io_object_table },
		{ L"math", math_object_table }
	};

	type_table = {
		{ L"io", {} },
		{ L"math", {} }
	};
}

void BuiltinsManager::set_native_type(std::wstring module_name, std::wstring name, Object_ptr type)
{
	ASSERT(exists(module_name, name), "Native object does not exist!");
	type_table.at(module_name)[name] = type;
}

Object_ptr BuiltinsManager::get_native_object(std::wstring module_name, std::wstring name)
{
	ASSERT(exists(module_name, name), "Native object does not exist!");
	return module_table.at(module_name).at(name);
}

Object_ptr BuiltinsManager::get_native_type(std::wstring module_name, std::wstring name)
{
	ASSERT(exists(module_name, name), "Native object does not exist!");
	return type_table.at(module_name).at(name);
}

bool BuiltinsManager::exists(std::wstring module_name, std::wstring name)
{
	if (module_table.contains(module_name))
	{
		return module_table.at(module_name).contains(name);
	}

	return false;
}