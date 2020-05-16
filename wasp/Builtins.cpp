#pragma once

#include "Builtins.h"
#include "ObjectSystem.h"
#include "Assertion.h"
#include "spdlog.h"

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
#define THROW_ASSERT(condition, message) if (!condition) { return std::make_shared<Object>(ErrorObject(message)); }

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

Object_ptr io::echo_visit(std::vector<Object_ptr> arguments)
{
	THROW_ASSERT(arguments.size() == 1, "echo(..) takes a string or number as argument");

	return std::visit(overloaded{
		[](StringObject text) { cout << text.value << endl; return VOID; },
		[](NumberObject number) { cout << number.value << endl; return VOID; },

		[](auto) { THROW("echo(..) takes a string or number as argument"); }
		}, *arguments[0]);
}

Object_ptr io::ask_visit(std::vector<Object_ptr> arguments)
{
	THROW_ASSERT(arguments.size() == 0, "ask(..) takes no arguments");

	string text;
	cin >> text;

	return MAKE_OBJECT_VARIANT(StringObject(text));
}