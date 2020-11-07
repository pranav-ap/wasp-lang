#pragma once
#include "Symbol.h"
#include "CodeSection.h"
#include <map>
#include <memory>
#include <string>
#include <optional>

enum class ScopeType
{
	FILE,
	CONDITIONAL,
	LOOP,
	FUNCTION,
	GENERATOR,
	CLASS_FUNCTION,
	CLASS_GENERATOR,
	OPERATOR_FUNCTION,
	NAMESPACE
};

class SymbolScope;
using SymbolScope_ptr = std::shared_ptr<SymbolScope>;

struct SymbolScope
{
	ScopeType scope_type;
	std::map<std::wstring, Symbol_ptr> symbols;
	std::optional<SymbolScope_ptr> enclosing_scope;

	CodeSection_ptr code_section;

	int break_label;
	int continue_label;

	SymbolScope(SymbolScope_ptr enclosing_scope, ScopeType scope_type, CodeSection_ptr code_section)
		: break_label(0),
		continue_label(0),
		scope_type(scope_type),
		enclosing_scope(std::move(enclosing_scope)),
		code_section(std::move(code_section)) {};

	void define(std::wstring name, Symbol_ptr symbol);
	Symbol_ptr lookup(std::wstring name);

	bool enclosed_in(ScopeType type);
	bool enclosed_in(std::vector<ScopeType> types);
};
