#pragma once

#include "CSymbol.h"
#include <map>
#include <memory>
#include <string>
#include <optional>

class CSymbolTable;
using CSymbolTable_ptr = std::shared_ptr<CSymbolTable>;

class CSymbolTable
{
	std::map<std::wstring, CSymbol_ptr> store;

public:
	std::optional<CSymbolTable_ptr> enclosing_scope;

	CSymbolTable()
		: enclosing_scope(std::nullopt) {};

	CSymbolTable(CSymbolTable_ptr enclosing_scope)
		: enclosing_scope(enclosing_scope) {};

	int define(std::wstring name);
	int lookup(std::wstring name);
};
