#pragma once
#include "OpCode.h"
#include "CSymbolTable.h"
#include <memory>

struct CScope
{
	CSymbolTable_ptr symbol_table;
	std::vector<std::byte> instructions;

	int break_label;
	int continue_label;

	CScope()
		: break_label(0),
		continue_label(0),
		symbol_table(std::make_shared<CSymbolTable>()) {};

	CScope(CSymbolTable_ptr enclosing_symbol_table)
		: break_label(0),
		continue_label(0),
		symbol_table(std::make_shared<CSymbolTable>(enclosing_symbol_table)) {};
};

using CScope_ptr = std::shared_ptr<CScope>;
