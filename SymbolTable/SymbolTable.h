#pragma once

#ifdef SYMBOLTABLE_EXPORTS
#define SYMBOLTABLE_API __declspec(dllexport)
#else
#define SYMBOLTABLE_API __declspec(dllimport)
#endif

#include "Symbol.h"
#include <map>
#include <memory>
#include <string>
#include <optional>

class ScopedSymbolTable;
using ScopedSymbolTable_ptr = SYMBOLTABLE_API std::shared_ptr<ScopedSymbolTable>;

class SYMBOLTABLE_API ScopedSymbolTable
{
	std::map<std::wstring, Symbol_ptr> store;

public:
	std::optional<ScopedSymbolTable_ptr> enclosing_scope;

	ScopedSymbolTable()
		: enclosing_scope(std::nullopt) {};

	ScopedSymbolTable(ScopedSymbolTable_ptr enclosing_scope)
		: enclosing_scope(std::make_optional(enclosing_scope)) {};

	void define(std::wstring name, Symbol_ptr symbol);
	std::optional<Symbol_ptr> lookup(std::wstring name);
};
