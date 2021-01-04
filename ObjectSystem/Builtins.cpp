#pragma once
#include "pch.h"
#include "Builtins.h"
#include <string>
#include <iostream>

#define THROW(message) return std::make_shared<Object>(ErrorObject(message))
#define VOID std::make_shared<Object>(ReturnObject())

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::cout;
using std::cin;
using std::endl;
using std::wstring;

// ECHO

Object_ptr io::echo_visit(std::vector<Object_ptr> arguments)
{
	if (!arguments.size() == 1) 
	{
		THROW(L"echo(..) takes a string or number as argument");
	}

	return std::visit(overloaded{
		[](std::string text) 
		{ 
			cout << text; 
			return VOID; 
		},
		[](int number) 
		{ 
			cout << number; 
			return VOID; 
		},
		[](double number) 
		{
			cout << number; 
			return VOID; 
		},

		[](auto) 
		{ 
			THROW(L"echo(..) takes a string or number as argument"); 
		}
		}, *arguments[0]);
}