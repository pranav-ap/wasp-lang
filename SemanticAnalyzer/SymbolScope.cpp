#pragma once
#include "pch.h"
#include "Symbol.h"
#include "SymbolScope.h"
#include "Assertion.h"

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")

void SymbolScope::define(std::wstring name, Symbol_ptr symbol)
{
	NULL_CHECK(symbol);

	auto result = store.insert({ name, symbol });
	ASSERT(result.second, "Name already exists in scope!");
}

std::optional<Symbol_ptr> SymbolScope::lookup(std::wstring name)
{
	if (store.contains(name))
	{
		auto symbol = store.at(name);
		NULL_CHECK(symbol);
		ASSERT(symbol->index() != 0, "Info is a monostate");
		return make_optional(symbol);
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
	for (auto const type : types)
	{
		if (enclosed_in(type))
		{
			return true;
		}
	}

	return false;
}