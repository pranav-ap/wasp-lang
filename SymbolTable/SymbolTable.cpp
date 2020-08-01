#include "pch.h"
#include "SymbolTable.h"
#include "Assertion.h"

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")

void SymbolTable::define(std::wstring name, Symbol_ptr symbol)
{
	NULL_CHECK(symbol);

	auto result = store.insert({ name, symbol });
	ASSERT(result.second, "Name already exists in scope!");
}

std::optional<Symbol_ptr> SymbolTable::lookup(std::wstring name)
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