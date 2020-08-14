#pragma once
#include "pch.h"
#include "CSymbolTable.h"
#include "Assertion.h"

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")

CSymbol_ptr CSymbolTable::define(std::wstring name)
{
	auto symbol_table = std::make_shared<CSymbol>(name, store.size());

	auto result = store.insert({ name, symbol_table });
	ASSERT(result.second, "Name already exists in scope!");

	return symbol_table;
}

CSymbol_ptr CSymbolTable::lookup(std::wstring name)
{
	if (store.contains(name))
	{
		auto CSymbol = store.at(name);
		NULL_CHECK(CSymbol);
		return CSymbol;
	}

	if (enclosing_scope.has_value())
	{
		return enclosing_scope.value()->lookup(name);
	}

	FATAL("Name does not exist!");
}