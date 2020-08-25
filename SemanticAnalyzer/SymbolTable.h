#pragma once

#include "Symbol.h"
#include <map>
#include <memory>
#include <string>
#include <optional>

class SemanticAnalyzerScope;
using Scope_ptr = std::shared_ptr<SemanticAnalyzerScope>;

class SemanticAnalyzerScope
{
	std::map<std::wstring, Symbol_ptr> store;

public:
	std::optional<Scope_ptr> enclosing_scope;

	SemanticAnalyzerScope() : enclosing_scope(std::nullopt) {};
	SemanticAnalyzerScope(Scope_ptr enclosing_scope) : enclosing_scope(enclosing_scope) {};

	void define(std::wstring name, Symbol_ptr symbol);
	std::optional<Symbol_ptr> lookup(std::wstring name);
};
