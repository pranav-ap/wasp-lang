#pragma once

#include "pch.h"
#include "Builtins.h"
#include "ObjectSystem.h"
#include "CommonAssertion.h"

#include <iostream>
#include <string>
#include <variant>

using std::cout;
using std::cin;
using std::endl;
using std::string;

#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define VOID std::make_shared<Object>(ReturnObject())
#define THROW(message) return std::make_shared<Object>(ErrorObject(message))

#define THROW_ASSERT(condition, message)								\
	if (!condition) {													\
		spdlog::error(message);											\
		return std::make_shared<Object>(ErrorObject(message));			\
	}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

Object_ptr io::echo_visit(std::vector<Object_ptr> arguments)
{
	THROW_ASSERT(arguments.size() == 1, "echo(..) takes a string or number as argument");

	return std::visit(overloaded{
		[](std::string text) { cout << text << endl; return VOID; },
		[](double number) { cout << number << endl; return VOID; },

		[](auto) { THROW("echo(..) takes a string or number as argument"); }
		}, *arguments[0]);
}

Object_ptr io::ask(std::vector<Object_ptr> arguments)
{
	THROW_ASSERT(arguments.size() == 0, "ask(..) takes no arguments");

	string text;
	cin >> text;

	return MAKE_OBJECT_VARIANT(text);
}