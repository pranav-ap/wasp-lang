#pragma once

#ifdef SYMBOL_EXPORTS
#define SYMBOL_API __declspec(dllexport)
#else
#define SYMBOL_API __declspec(dllimport)
#endif

#include "Symbol.h"
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <optional>

enum class SYMBOL_API ScopeType
{
	NONE,
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
using SymbolScope_ptr = SYMBOL_API std::shared_ptr<SymbolScope>;

struct SYMBOL_API SymbolScope
{
	ScopeType scope_type;
	std::map<std::wstring, Symbol_ptr> symbols;
	std::optional<SymbolScope_ptr> enclosing_scope;

	int break_label;
	int continue_label;

	SymbolScope(SymbolScope_ptr enclosing_scope, ScopeType scope_type)
		: break_label(0),
		continue_label(0),
		scope_type(scope_type),
		enclosing_scope(std::move(enclosing_scope)) {};

	void define(std::wstring name, Symbol_ptr symbol);
	Symbol_ptr lookup(std::wstring name);

	bool enclosed_in(ScopeType type);
	bool enclosed_in(std::vector<ScopeType> types);
};
