#pragma once
#include "pch.h"
#include "Symbol.h"
#include "SymbolScope.h"
#include "Assertion.h"
#include <algorithm>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")

using std::any_of;
using std::begin;
using std::end;

void SymbolScope::define(std::wstring name, Symbol_ptr symbol)
{
	NULL_CHECK(symbol);

	auto result = symbols.insert({ name, symbol });
	ASSERT(result.second, "Name already exists in scope!");
}

void SymbolScope::define_subroutine(std::wstring name, Symbol_ptr symbol)
{
	//this->function_overloads
}

Symbol_ptr SymbolScope::lookup_all(std::wstring name)
{
	return Symbol_ptr();
}

Symbol_ptr SymbolScope::lookup(std::wstring name)
{
	if (symbols.contains(name))
	{
		auto symbol = symbols.at(name);
		NULL_CHECK(symbol);
		return symbol;
	}

	if (enclosing_scope.has_value())
	{
		return enclosing_scope.value()->lookup(name);
	}

	FATAL("Name does not exist!");
}

bool SymbolScope::enclosed_in(ScopeType type)
{
	if (scope_type == type)
	{
		return true;
	}

	if (enclosing_scope.has_value())
	{
		return enclosing_scope.value()->enclosed_in(type);
	}

	return false;
}

bool SymbolScope::enclosed_in(std::vector<ScopeType> types)
{
	return any_of(begin(types), end(types), [&](ScopeType scope_type) { return enclosed_in(scope_type); });
}