#pragma once

#include "Symbol.h"
#include <map>
#include <memory>
#include <string>
#include <optional>

class SymbolTable;
using SymbolTable_ptr = std::shared_ptr<SymbolTable>;

class SymbolTable
{
	std::map<std::wstring, Symbol_ptr> store;

public:
	std::optional<SymbolTable_ptr> enclosing_scope;

	SymbolTable()
		: enclosing_scope(std::nullopt) {};

	void define(std::wstring name, Symbol_ptr symbol);
	std::optional<Symbol_ptr> lookup(std::wstring name);
};
