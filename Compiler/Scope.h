#pragma once
#include "OpCode.h"
#include "Bytecode.h"
#include "CSymbolTable.h"
#include <memory>

struct Scope
{
	CSymbolTable_ptr symbol_table;
	std::vector<std::byte> instructions;

	int break_label;
	int continue_label;

	Scope()
		: break_label(0),
		continue_label(0),
		symbol_table(std::make_shared<CSymbolTable>()) {};

	Scope(CSymbolTable_ptr enclosing_symbol_table)
		: break_label(0),
		continue_label(0),
		symbol_table(std::make_shared<CSymbolTable>(enclosing_symbol_table)) {};
};

using Scope_ptr = std::shared_ptr<Scope>;
