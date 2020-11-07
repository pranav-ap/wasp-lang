#pragma once
#include "pch.h"
#include "ObjectStore.h"
#include "Assertion.h"
#include <string>
#include <algorithm>


#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)

using std::byte;
using std::vector;
using std::move;
using std::make_shared;

void ObjectStore::set(int id, Object_ptr value)
{
	ASSERT(!objects.contains(id), "ID already exists in ObjectStore");
	objects.insert({ id, value });
}

Object_ptr ObjectStore::get(int id)
{
	ASSERT(objects.contains(id), "ID does not exist in ObjectStore");
	return objects.at(id);
}

int ObjectStore::allocate()
{
	int id = objects.size();
	objects.insert({ id, move(MAKE_OBJECT_VARIANT(NoneObject(get_none_type()))) });

	return id;
}

int ObjectStore::allocate(int number)
{
	auto result = find_if(
		objects.begin(),
		objects.end(),
		[number](const auto& p) {
			if (holds_alternative<IntObject>(*p.second))
			{
				IntObject* x = get_if<IntObject>(&*p.second);
				return x->value == number;
			}

			return false;
		});

	if (result != objects.end())
	{
		return result->first;
	}

	int id = objects.size();
	auto value = MAKE_OBJECT_VARIANT(IntObject(number, get_int_type()));
	objects.insert({ id, value });

	return id;
}

int ObjectStore::allocate(double number)
{
	auto result = find_if(
		objects.begin(),
		objects.end(),
		[number](const auto& p) {
			if (holds_alternative<FloatObject>(*p.second))
			{
				FloatObject* x = get_if<FloatObject>(&*p.second);
				return x->value == number;
			}

			return false;
		});

	if (result != objects.end())
	{
		return result->first;
	}

	int id = objects.size();
	auto value = MAKE_OBJECT_VARIANT(FloatObject(number, get_float_type()));
	objects.insert({ id, value });

	return id;
}

int ObjectStore::allocate(std::wstring text)
{
	auto result = find_if(
		objects.begin(),
		objects.end(),
		[text](const auto& p) {
			if (holds_alternative<StringObject>(*p.second))
			{
				StringObject* x = get_if<StringObject>(&*p.second);
				return x->value == text;
			}

			return false;
		});

	if (result != objects.end())
	{
		return result->first;
	}

	int id = objects.size();
	auto value = MAKE_OBJECT_VARIANT(StringObject(text, get_string_type()));
	objects.insert({ id, value });

	return id;
}

int ObjectStore::allocate(Object_ptr value)
{
	int id = objects.size();
	objects.insert({ id, move(value) });

	return id;
}

Object_ptr ObjectStore::get_any_type()
{
	return get(0);
}

Object_ptr ObjectStore::get_int_type()
{
	return get(1);
}

Object_ptr ObjectStore::get_float_type()
{
	return get(2);
}

Object_ptr ObjectStore::get_string_type()
{
	return get(3);
}

Object_ptr ObjectStore::get_boolean_type()
{
	return get(4);
}

Object_ptr ObjectStore::get_none_type()
{
	return get(5);
}
