#pragma once
#include "pch.h"
#include "CSymbolTable.h"
#include "Assertion.h"

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")

void CSymbolTable::define(std::wstring name, int id)
{
	auto result = store.insert({ name, id });
	ASSERT(result.second, "Name already exists in scope!");
}

int CSymbolTable::lookup(std::wstring name)
{
	if (store.contains(name))
	{
		return store.at(name);
	}

	if (enclosing_scope.has_value())
	{
		return enclosing_scope.value()->lookup(name);
	}

	FATAL("Name does not exist!");
}