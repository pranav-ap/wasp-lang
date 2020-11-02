#pragma once
#include "OpCode.h"
#include "MemorySystem.h"
#include <memory>

struct CScope;
using CScope_ptr = std::shared_ptr<CScope>;

struct CScope
{
	std::map<std::wstring, int> store;
	std::optional<CScope_ptr> enclosing_scope;

	CodeSection_ptr code_section;

	int break_label;
	int continue_label;

	bool is_rvalue;

	CScope(std::optional<CScope_ptr> enclosing_scope, CodeSection_ptr code_section)
		: break_label(0),
		continue_label(0),
		is_rvalue(true),
		enclosing_scope(enclosing_scope),
		code_section(code_section) {};

	void define(std::wstring name, int label);
	int lookup(std::wstring name);
};
