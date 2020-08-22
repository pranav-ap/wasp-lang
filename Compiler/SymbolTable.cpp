#pragma once
#include "pch.h"
#include "CSymbolTable.h"
#include "Assertion.h"

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")

int CSymbolTable::define(std::wstring name, int label)
{
	auto symbol_table = std::make_shared<CSymbol>(name, label);

	auto result = store.insert({ name, symbol_table });
	ASSERT(result.second, "Name already exists in scope!");

	return label;
}

int CSymbolTable::lookup(std::wstring name)
{
	if (store.contains(name))
	{
		auto symbol = store.at(name);
		NULL_CHECK(symbol);
		return symbol->label;
	}

	if (enclosing_scope.has_value())
	{
		return enclosing_scope.value()->lookup(name);
	}

	FATAL("Name does not exist!");
}