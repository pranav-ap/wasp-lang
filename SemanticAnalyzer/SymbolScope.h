#pragma once
#include "Symbol.h"
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
	OPERATOR_FUNCTION
};

class SymbolScope;
using SymbolScope_ptr = std::shared_ptr<SymbolScope>;

class SymbolScope
{
	ScopeType scope_type;
	std::map<std::wstring, Symbol_ptr> store;

public:
	std::optional<SymbolScope_ptr> enclosing_scope;

	SymbolScope()
		: scope_type(ScopeType::FILE), enclosing_scope(std::nullopt) {};

	SymbolScope(SymbolScope_ptr enclosing_scope, ScopeType scope_type)
		: scope_type(scope_type), enclosing_scope(enclosing_scope) {};

	void define(std::wstring name, Symbol_ptr symbol);
	std::optional<Symbol_ptr> lookup(std::wstring name);

	bool enclosed_in(ScopeType type);
	bool enclosed_in(std::vector<ScopeType> types);
};
