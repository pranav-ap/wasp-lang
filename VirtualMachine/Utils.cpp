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
	stack.push(o);
}

Object_ptr VirtualMachine::pop_from_stack()
{
	if (stack.size() == 0)
	{
		FATAL("Stack is empty. Cannot Pop!");
	}

	auto TOS = stack.top();
	stack.pop();

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
	if (stack.size() == 0)
	{
		FATAL("Stack is empty. Cannot Pop!");
	}

	auto TOS = stack.top();
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
			ObjectVector vec = to_vector(obj.value);
			return MAKE_OBJECT_VARIANT(IteratorObject(vec));
		},
		[&](ListObject& obj)
		{
			ObjectVector vec = to_vector(obj.values);
			return MAKE_OBJECT_VARIANT(IteratorObject(vec));
		},
		[&](SetObject& obj)
		{
			return MAKE_OBJECT_VARIANT(IteratorObject(obj.values));
		},
		[&](MapObject& obj)
		{
			ObjectVector tuples;

			for (auto p : obj.pairs)
			{
				auto tuple_pair = MAKE_OBJECT_VARIANT(TupleObject({p.first, p.second}));
				tuples.push_back(tuple_pair);
			}

			return MAKE_OBJECT_VARIANT(IteratorObject(tuples));
		},
		[&](auto)
		{
			return object_store->make_error_object(L"_");
		}
		}, *obj);
}

ObjectVector VirtualMachine::to_vector(std::deque<Object_ptr> values)
{
	ObjectVector vec;

	for (auto value : values)
	{
		vec.push_back(value);
	}

	return vec;
}

ObjectVector VirtualMachine::to_vector(std::wstring text)
{
	ObjectVector vec;

	for (auto ch : text)
	{
		vec.push_back(MAKE_OBJECT_VARIANT(StringObject(std::to_wstring(ch))));
	}

	return vec;
}
