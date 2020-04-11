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

#define MAKE_OBJECT_VARIANT(x) std::make_shared<ObjectVariant>(x)
#define VOID std::make_shared<ObjectVariant>(ReturnObject())
#define THROW(message) return std::make_shared<ObjectVariant>(ErrorObject(message))

#define THROW_ASSERT(condition, message)								\
	if (!condition) {													\
		spdlog::error(message);											\
		return std::make_shared<ObjectVariant>(ErrorObject(message));	\
	}

// ECHO

ObjectVariant_ptr io::echo_visit(std::vector<ObjectVariant_ptr> arguments)
{
	THROW_ASSERT(arguments.size() == 1, "echo(..) takes a string or number as argument");

	return std::visit(overloaded{
		[](std::string text) { cout << text; return VOID; },
		[](double number) { cout << number; return VOID; },

		[](auto) { THROW("echo(..) takes a string or number as argument"); }
		}, *arguments[0]);
}

// ASK

ObjectVariant_ptr io::ask_visit(std::vector<ObjectVariant_ptr> arguments)
{
	THROW_ASSERT(arguments.size() == 1, "ask(..) takes one string or number as argument");

	return std::visit(overloaded{
		[](std::string& text) { cin >> text; return VOID; },
		[](double& number) { cin >> number; return VOID; },

		[](auto) { THROW("ask(..) takes one string or number as argument."); }
		}, *arguments[0]);
}

ObjectVariant_ptr core::size_visit(std::vector<ObjectVariant_ptr> arguments)
{
	THROW_ASSERT(arguments.size() == 1, "size(..) takes a string or vector as argument");

	return std::visit(overloaded{
		[](std::string text) { return MAKE_OBJECT_VARIANT((double)text.length()); },
		[](VectorObject& vec) { return MAKE_OBJECT_VARIANT((double)vec.values.size()); },

		[](auto) { THROW("size(..) takes a string or vector as argument"); }
		}, *arguments[0]);
}