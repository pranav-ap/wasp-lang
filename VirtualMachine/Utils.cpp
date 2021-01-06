#pragma once
#include "pch.h"
#include "VirtualMachine.h"
#include "Assertion.h"
#include <string>
#include <memory>

#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::make_shared;

void VirtualMachine::push_to_value_stack(Object_ptr o)
{
	value_stack.push(o);
}

Object_ptr VirtualMachine::pop_from_value_stack()
{
	if (value_stack.size() == 0)
	{
		FATAL("Stack is empty. Cannot Pop!");
	}

	auto TOS = value_stack.top();
	value_stack.pop();

	return TOS;
}

ObjectVector VirtualMachine::pop_n_from_value_stack(int n)
{
	ObjectVector elements;
	int count = 0;

	while (count < n)
	{
		auto element = pop_from_value_stack();
		elements.push_back(element);
		count++;
	}

	ASSERT(elements.size() == n, "Number of elements popped out is not equal to N");
	
	return elements;
}

Object_ptr VirtualMachine::top_of_value_stack()
{
	if (value_stack.size() == 0)
	{
		FATAL("Value Stack is empty. Cannot Pop!");
	}

	auto TOS = value_stack.top();
	return TOS;
}

void VirtualMachine::push_empty_scope_to_local_scope_stack()
{
	scope_stack.push(make_shared<LocalScope>());
}

void VirtualMachine::pop_from_local_scope_stack()
{
	if (scope_stack.size() == 0)
	{
		FATAL("Scope Stack is empty. Cannot Pop!");
	}

	scope_stack.pop();
}

void VirtualMachine::push_to_call_stack(CodeObject_ptr code_object, int base_pointer)
{
	call_stack.push(make_shared<CallFrame>(code_object, base_pointer));
}

void VirtualMachine::pop_from_call_stack()
{
	if (scope_stack.size() == 0)
	{
		FATAL("Scope Stack is empty. Cannot Pop!");
	}

	call_stack.pop();
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

int VirtualMachine::get_ip()
{
	int ip = call_stack.top()->ip;
	return ip;
}

void VirtualMachine::set_ip(int ip)
{
	call_stack.top()->ip = ip;
}

CodeObject_ptr VirtualMachine::get_current_code_object()
{
	auto obj = call_stack.top()->code_object;
	return obj;
}
