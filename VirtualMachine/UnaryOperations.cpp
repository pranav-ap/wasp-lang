#pragma once
#include "pch.h"
#include "VirtualMachine.h"
#include "Assertion.h"
#include <string>
#include <cmath>

#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::to_wstring;
using std::wstring;

Object_ptr VirtualMachine::perform_unary_negative(Object_ptr obj)
{
	return std::visit(overloaded{
		[&](IntObject& obj)
		{
			obj.value = -obj.value;
			return MAKE_OBJECT_VARIANT(obj);
		},
		[&](FloatObject& obj)
		{
			obj.value = -obj.value;
			return MAKE_OBJECT_VARIANT(obj);
		},

		[&](auto)
		{
			return object_store->make_error_object(L"_");
		}
		}, *obj);
}

Object_ptr VirtualMachine::perform_unary_not(Object_ptr obj)
{
	return std::visit(overloaded{
		[&](BooleanObject& obj)
		{
			obj.value = !obj.value;
			return MAKE_OBJECT_VARIANT(obj);
		},

		[&](auto)
		{
			return object_store->make_error_object(L"_");
		}
		}, *obj);
}
