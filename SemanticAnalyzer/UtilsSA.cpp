#pragma once
#include "pch.h"
#include "SemanticAnalyzer.h"
#include "Symbol.h"
#include "Assertion.h"
#include <variant>
#include <vector>
#include <memory>
#include <algorithm>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_SYMBOL(a, b, c, d, e) std::make_shared<Symbol>(a, b, c, d, e)
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::holds_alternative;
using std::wstring;
using std::get_if;
using std::begin;
using std::end;
using std::vector;
using std::make_shared;
using std::move;

void SemanticAnalyzer::enter_scope(ScopeType scope_type)
{
	NULL_CHECK(current_scope);
	
	auto child_scope = std::make_shared<SymbolScope>(current_scope, scope_type);
	child_scope->name_space = current_scope->name_space;

	current_scope = child_scope;
}

void SemanticAnalyzer::leave_scope()
{
	NULL_CHECK(current_scope);
	OPT_CHECK(current_scope->enclosing_scope);
	current_scope = current_scope->enclosing_scope.value();
}

wstring SemanticAnalyzer::concat(StringVector items, wstring middle)
{
	wstring final_string = L"";

	for (const auto member : items)
	{
		final_string.append(middle);
		final_string.append(member);
	}

	final_string = final_string.substr(2, final_string.size());
	return final_string;
}

std::tuple<std::wstring, Object_ptr> SemanticAnalyzer::deconstruct_type_pattern(Expression_ptr expression)
{
	ASSERT(holds_alternative<TypePattern>(*expression), "Expected a TypePattern");
	auto type_pattern = get_if<TypePattern>(&*expression);

	Object_ptr type = visit(type_pattern->type_node);

	ASSERT(holds_alternative<Identifier>(*type_pattern->expression), "Expected an Identifier");
	auto identifier = get_if<Identifier>(&*type_pattern->expression);

	return std::make_tuple(identifier->name, type);
}

bool SemanticAnalyzer::any_eq(ObjectVector vec, Object_ptr x)
{
	return std::any_of(begin(vec), end(vec), [&](auto k) { return type_system->equal(current_scope, x, k); });
}

ObjectVector SemanticAnalyzer::remove_duplicates(ObjectVector vec)
{
	ObjectVector uniq_vec;

	for (const auto x : vec)
	{
		if (!any_eq(uniq_vec, x))
		{
			uniq_vec.push_back(x);
		}
	}

	return uniq_vec;
}

