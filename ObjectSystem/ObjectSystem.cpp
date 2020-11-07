#pragma once
#include "pch.h"
#include "ObjectSystem.h"
#include "Assertion.h"
#include <memory>
#include <algorithm>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::holds_alternative;
using std::wstring;
using std::get_if;
using std::vector;
using std::make_shared;
using std::move;
using std::all_of;
using std::begin;
using std::end;
 
ObjectSystem::ObjectSystem()
{
	next_id = 0;

	// add common type objects to pool

	object_store->set(next_id++, MAKE_OBJECT_VARIANT(AnyType()));	  // 0
	object_store->set(next_id++, MAKE_OBJECT_VARIANT(IntType()));	  // 1
	object_store->set(next_id++, MAKE_OBJECT_VARIANT(FloatType()));   // 2
	object_store->set(next_id++, MAKE_OBJECT_VARIANT(StringType()));  // 3
	object_store->set(next_id++, MAKE_OBJECT_VARIANT(BooleanType())); // 4
	object_store->set(next_id++, MAKE_OBJECT_VARIANT(NoneType()));    // 5
}
