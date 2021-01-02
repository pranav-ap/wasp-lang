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

Object_ptr VirtualMachine::perform_add(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](IntObject& left, IntObject& right)
		{
			return object_store->make_object(left.value + right.value);
		},
		[&](FloatObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value + right.value);
		},
		[&](IntObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value + right.value);
		},
		[&](FloatObject& left, IntObject& right)
		{
			return object_store->make_object(left.value + right.value);
		},

		// String 

		[&](StringObject& left, IntObject& right)
		{
			return object_store->make_object(left.value + to_wstring(right.value));
		},
		[&](StringObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value + to_wstring(right.value));
		},
		[&](IntObject& left, StringObject& right)
		{
			return object_store->make_object(to_wstring(left.value) + right.value);
		},
		[&](FloatObject& left, StringObject& right)
		{
			return object_store->make_object(to_wstring(left.value) + right.value);
		},

		[&](auto, auto)
		{
			return object_store->make_error_object(L"_");
		}
	}, *left, *right);
}

Object_ptr VirtualMachine::perform_subtract(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](IntObject& left, IntObject& right)
		{
			return object_store->make_object(left.value - right.value);
		},
		[&](FloatObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value - right.value);
		},
		[&](IntObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value - right.value);
		},
		[&](FloatObject& left, IntObject& right)
		{
			return object_store->make_object(left.value - right.value);
		},
		[&](auto, auto)
		{
			return object_store->make_error_object(L"_");
		}
		}, *left, *right);
}

Object_ptr VirtualMachine::perform_multiply(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](IntObject& left, IntObject& right)
		{
			return object_store->make_object(left.value * right.value);
		},
		[&](FloatObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value * right.value);
		},
		[&](IntObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value * right.value);
		},
		[&](FloatObject& left, IntObject& right)
		{
			return object_store->make_object(left.value * right.value);
		},

		// String 

		[&](StringObject& left, IntObject& right)
		{
			wstring result_string(L"");
			int count = 0;

			while (count < right.value)
			{
				result_string += left.value;
				count++;
			}

			return object_store->make_object(result_string);
		},

		[&](auto, auto)
		{
			return object_store->make_error_object(L"_");
		}
	}, *left, *right);
}

Object_ptr VirtualMachine::perform_division(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](IntObject& left, IntObject& right)
		{
			if (right.value == 0)
			{
				return object_store->make_error_object(L"_");
			}

			return object_store->make_object(left.value / right.value);
		},
		[&](FloatObject& left, FloatObject& right)
		{
			if (right.value == 0 || right.value == 0.0)
			{
				return object_store->make_error_object(L"_");
			}

			return object_store->make_object(left.value / right.value);
		},
		[&](IntObject& left, FloatObject& right)
		{
			if (right.value == 0 || right.value == 0.0)
			{
				return object_store->make_error_object(L"_");
			}

			return object_store->make_object(left.value / right.value);
		},
		[&](FloatObject& left, IntObject& right)
		{
			if (right.value == 0)
			{
				return object_store->make_error_object(L"_");
			}

			return object_store->make_object(left.value / right.value);
		},

		[&](auto, auto)
		{
			return object_store->make_error_object(L"_");
		}
		}, *left, *right);
}

Object_ptr VirtualMachine::perform_reminder(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](IntObject& left, IntObject& right)
		{
			if (right.value == 0)
			{
				return object_store->make_error_object(L"_");
			}

			return object_store->make_object(left.value % right.value);
		},

		[&](auto, auto)
		{
			return object_store->make_error_object(L"_");
		}
		}, *left, *right);
}

Object_ptr VirtualMachine::perform_power(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](IntObject& left, IntObject& right)
		{
			return object_store->make_object(std::pow(left.value, right.value));
		},
		[&](FloatObject& left, IntObject& right)
		{
			return object_store->make_object(std::pow(left.value, right.value));
		},

		[&](auto, auto)
		{
			return object_store->make_error_object(L"_");
		}
		}, *left, *right);
}
