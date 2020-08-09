#pragma once
#include "pch.h"
#include "SymbolTable.h"
#include "Assertion.h"

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")

Symbol_ptr SymbolTable::define(std::wstring name)
{
	auto symbol = std::make_shared<Symbol>(name, store.size());

	auto result = store.insert({ name, symbol });
	ASSERT(result.second, "Name already exists in scope!");

	return symbol;
}

Symbol_ptr SymbolTable::lookup(std::wstring name)
{
	if (store.contains(name))
	{
		auto symbol = store.at(name);
		NULL_CHECK(symbol);
		return symbol;
	}

	if (enclosing_scope.has_value())
	{
		return enclosing_scope.value()->lookup(name);
	}

	FATAL("Name does not exist!");
}