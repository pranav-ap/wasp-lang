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


Object_ptr VirtualMachine::perform_not_equal(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](IntObject& left, IntObject& right)
		{
			return object_store->make_object(left.value != right.value);
		},
		[&](FloatObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value != right.value);
		},
		[&](StringObject& left, StringObject& right)
		{
			return object_store->make_object(left.value != right.value);
		},
		[&](BooleanObject& left, BooleanObject& right)
		{
			return object_store->make_object(left.value != right.value);
		},
		[&](TupleObject& left, TupleObject& right)
		{
			if (left.values.size() != right.values.size())
			{
				return object_store->get_false_object();
			}

			int size = left.values.size();

			for (int i = 0; i < size; i++)
			{
				auto left_value = left.values[i];
				auto right_value = right.values[i];

				if (perform_equal(left_value, right_value))
				{
					return object_store->get_false_object();
				}
			}

			return object_store->get_true_object();
		},
		[&](ListObject& left, ListObject& right)
		{
			if (left.values.size() != right.values.size())
			{
				return object_store->get_false_object();
			}

			int size = left.values.size();

			for (int i = 0; i < size; i++)
			{
				auto left_value = left.values[i];
				auto right_value = right.values[i];

				if (perform_equal(left_value, right_value))
				{
					return object_store->get_false_object();
				}
			}

			return object_store->get_true_object();
		},
		[&](VariantObject& left, VariantObject& right)
		{
			return perform_not_equal(left.value, right.value);
		},

		[&](auto, auto)
		{
			return object_store->make_error_object(L"_");
		}
		}, *left, *right);
}

Object_ptr VirtualMachine::perform_equal(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](IntObject& left, IntObject& right)
		{
			return object_store->make_object(left.value == right.value);
		},
		[&](FloatObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value == right.value);
		},
		[&](StringObject& left, StringObject& right)
		{
			return object_store->make_object(left.value == right.value);
		},
		[&](BooleanObject& left, BooleanObject& right)
		{
			return object_store->make_object(left.value == right.value);
		},
		[&](TupleObject& left, TupleObject& right)
		{
			if (left.values.size() != right.values.size())
			{
				return object_store->get_false_object();
			}

			int size = left.values.size();

			for (int i = 0; i < size; i++)
			{
				auto left_value = left.values[i];
				auto right_value = right.values[i];

				if (perform_not_equal(left_value, right_value))
				{
					return object_store->get_false_object();
				}
			}

			return object_store->get_true_object();
		},
		[&](ListObject& left, ListObject& right)
		{
			if (left.values.size() != right.values.size())
			{
				return object_store->get_false_object();
			}

			int size = left.values.size();

			for (int i = 0; i < size; i++)
			{
				auto left_value = left.values[i];
				auto right_value = right.values[i];

				if (perform_not_equal(left_value, right_value))
				{
					return object_store->get_false_object();
				}
			}

			return object_store->get_true_object();
		},
		[&](VariantObject& left, VariantObject& right)
		{
			return perform_equal(left.value, right.value);
		},

		[&](auto, auto)
		{
			return object_store->make_error_object(L"_");
		}
		}, *left, *right);
}

Object_ptr VirtualMachine::perform_lesser_than(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](IntObject& left, IntObject& right)
		{
			return object_store->make_object(left.value < right.value);
		},
		[&](FloatObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value < right.value);
		},
		[&](IntObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value < right.value);
		},
		[&](FloatObject& left, IntObject& right)
		{
			return object_store->make_object(left.value < right.value);
		},

		[&](StringObject& left, StringObject& right)
		{
			return object_store->make_object(left.value < right.value);
		},

		[&](VariantObject& left, VariantObject& right)
		{
			return perform_lesser_than(left.value, right.value);
		},

		[&](auto, auto)
		{
			return object_store->make_error_object(L"_");
		}
		}, *left, *right);
}

Object_ptr VirtualMachine::perform_lesser_than_equal(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](IntObject& left, IntObject& right)
		{
			return object_store->make_object(left.value <= right.value);
		},
		[&](FloatObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value <= right.value);
		},
		[&](IntObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value <= right.value);
		},
		[&](FloatObject& left, IntObject& right)
		{
			return object_store->make_object(left.value <= right.value);
		},

		[&](StringObject& left, StringObject& right)
		{
			return object_store->make_object(left.value <= right.value);
		},

		[&](VariantObject& left, VariantObject& right)
		{
			return perform_lesser_than_equal(left.value, right.value);
		},

		[&](auto, auto)
		{
			return object_store->make_error_object(L"_");
		}
		}, *left, *right);
}

Object_ptr VirtualMachine::perform_greater_than(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](IntObject& left, IntObject& right)
		{
			return object_store->make_object(left.value > right.value);
		},
		[&](FloatObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value > right.value);
		},
		[&](IntObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value > right.value);
		},
		[&](FloatObject& left, IntObject& right)
		{
			return object_store->make_object(left.value > right.value);
		},

		[&](StringObject& left, StringObject& right)
		{
			return object_store->make_object(left.value > right.value);
		},

		[&](VariantObject& left, VariantObject& right)
		{
			return perform_greater_than(left.value, right.value);
		},

		[&](auto, auto)
		{
			return object_store->make_error_object(L"_");
		}
		}, *left, *right);
}

Object_ptr VirtualMachine::perform_greater_than_equal(Object_ptr left, Object_ptr right)
{
	return std::visit(overloaded{
		[&](IntObject& left, IntObject& right)
		{
			return object_store->make_object(left.value >= right.value);
		},
		[&](FloatObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value >= right.value);
		},
		[&](IntObject& left, FloatObject& right)
		{
			return object_store->make_object(left.value >= right.value);
		},
		[&](FloatObject& left, IntObject& right)
		{
			return object_store->make_object(left.value >= right.value);
		},

		[&](StringObject& left, StringObject& right)
		{
			return object_store->make_object(left.value >= right.value);
		},

		[&](VariantObject& left, VariantObject& right)
		{
			return perform_greater_than_equal(left.value, right.value);
		},

		[&](auto, auto)
		{
			return object_store->make_error_object(L"_");
		}
		}, *left, *right);
}

