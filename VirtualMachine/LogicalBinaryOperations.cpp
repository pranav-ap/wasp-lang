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


Object_ptr VirtualMachine::perform_and(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](BooleanObject& left, BooleanObject& right)
		{
			return constant_pool->make_object(left.value & right.value);
		},
		[&](auto, auto)
		{
			return constant_pool->make_error_object(L"_");
		}
		}, *left, *right);
}

Object_ptr VirtualMachine::perform_or(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](BooleanObject& left, BooleanObject& right)
		{
			return constant_pool->make_object(left.value | right.value);
		},
		[&](auto, auto)
		{
			return constant_pool->make_error_object(L"_");
		}
		}, *left, *right);
}

Object_ptr VirtualMachine::perform_nullish_coalese(Object_ptr left, Object_ptr right)
{
	if (is_truthy(left))
	{
		return left;
	}

	return right;
}
