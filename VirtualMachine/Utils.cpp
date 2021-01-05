#pragma once
#include "pch.h"
#include "VirtualMachine.h"
#include "Assertion.h"
#include <string>

#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

void VirtualMachine::push_to_stack(Object_ptr o)
{
	value_stack.push(o);
}

Object_ptr VirtualMachine::pop_from_stack()
{
	if (value_stack.size() == 0)
	{
		FATAL("Stack is empty. Cannot Pop!");
	}

	auto TOS = value_stack.top();
	value_stack.pop();

	return TOS;
}

ObjectVector VirtualMachine::pop_n_from_stack(int n)
{
	ObjectVector elements;
	int count = 0;

	while (count < n)
	{
		auto element = pop_from_stack();
		elements.push_back(element);
		count++;
	}

	ASSERT(elements.size() == n, "Number of elements popped out is not equal to N");
	
	return elements;
}

Object_ptr VirtualMachine::top_of_stack()
{
	if (value_stack.size() == 0)
	{
		FATAL("Stack is empty. Cannot Pop!");
	}

	auto TOS = value_stack.top();
	return TOS;
}

bool VirtualMachine::is_truthy(Object_ptr obj)
{
	return std::visit(overloaded{
		[&](BooleanObject& obj)
		{
			return obj.value;
		},
		[&](IntObject& obj)
		{
			return obj.value > 0;
		},
		[&](FloatObject& obj)
		{
			return obj.value > 0;
		},
		[&](StringObject& obj)
		{
			return obj.value.length() > 0;
		},
		[&](ListObject& obj)
		{
			return obj.values.size() > 0;
		},
		[&](SetObject& obj)
		{
			return false;
		},
		[&](NoneObject& obj)
		{
			return false;
		},
		[&](ErrorObject& obj)
		{
			return false;
		},
		[&](auto)
		{
			return false;
		}
		}, *obj);
}

Object_ptr VirtualMachine::make_iterable(Object_ptr obj)
{
	return std::visit(overloaded{
		[&](StringObject& obj)
		{
			return obj.get_iter();
		},
		[&](ListObject& obj)
		{
			return obj.get_iter();
		},
		[&](SetObject& obj)
		{
			return obj.get_iter();
		},
		[&](MapObject& obj)
		{
			return obj.get_iter();
		},
		[&](auto)
		{
			return constant_pool->make_error_object(L"_");
		}
		}, *obj);
}
