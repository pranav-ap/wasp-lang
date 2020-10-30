#pragma once
#include "OpCode.h"
#include "CSymbolTable.h"
#include "MemorySystem.h"
#include <memory>

struct CScope
{
	CSymbolTable_ptr symbol_table;
	CodeSection_ptr code_section;

	int break_label;
	int continue_label;

	CScope()
		: break_label(0),
		continue_label(0),
		symbol_table(std::make_shared<CSymbolTable>()),
		code_section(std::make_shared<CodeSection>()) {};

	CScope(CodeSection_ptr code_section)
		: break_label(0),
		continue_label(0),
		symbol_table(std::make_shared<CSymbolTable>()),
		code_section(code_section) {};

	CScope(CSymbolTable_ptr enclosing_symbol_table)
		: break_label(0),
		continue_label(0),
		symbol_table(std::make_shared<CSymbolTable>(enclosing_symbol_table)),
		code_section(std::make_shared<CodeSection>()) {};
};

using CScope_ptr = std::shared_ptr<CScope>;
