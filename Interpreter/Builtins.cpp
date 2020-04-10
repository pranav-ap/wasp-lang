#pragma once

#include "pch.h"
#include "Builtins.h"
#include "ObjectSystem.h"
#include <iostream>
#include <string>
#include <variant>

using std::cout;
using std::cin;
using std::endl;
using std::string;

// ECHO

ObjectVariant_ptr io::echo_visit(std::vector<ObjectVariant_ptr> arguments)
{
	THROW_IF_TRUTHY(arguments.size() != 1, "echo(..) takes one string as argument.");

	return std::visit(overloaded{
		[](std::string text) { return io::echo(text); },

		[](auto) {
			THROW("echo(..) takes a string as argument.");
			}
		}, *arguments[0]);
}

ObjectVariant_ptr io::echo(std::string text)
{
	cout << text;
	return VOID;
}

// ASK

ObjectVariant_ptr io::ask_visit(std::vector<ObjectVariant_ptr> arguments)
{
	THROW_IF_TRUTHY(arguments.size() != 1, "ask(..) takes one string or number as argument.");

	return std::visit(overloaded{
		[](std::string text) { return io::ask(text); },
		[](double number) { return io::ask(number); },

		[](auto) {
			THROW("ask(..) takes one string or number as argument.");
			}
		}, *arguments[0]);
}

ObjectVariant_ptr io::ask(string& content)
{
	cin >> content;
	return VOID;
}

ObjectVariant_ptr io::ask(double& number)
{
	cin >> number;
	return VOID;
}