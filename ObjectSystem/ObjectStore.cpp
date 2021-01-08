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

ObjectStore::ObjectStore()
{
	objects.insert({ 0, MAKE_OBJECT_VARIANT(AnyType()) });
	objects.insert({ 1, MAKE_OBJECT_VARIANT(IntType()) });
	objects.insert({ 2, MAKE_OBJECT_VARIANT(FloatType()) });
	objects.insert({ 3, MAKE_OBJECT_VARIANT(StringType()) });
	objects.insert({ 4, MAKE_OBJECT_VARIANT(BooleanType()) });
	objects.insert({ 5, MAKE_OBJECT_VARIANT(NoneType()) });

	objects.insert({ 6, MAKE_OBJECT_VARIANT(BooleanLiteralType(true)) });
	objects.insert({ 7, MAKE_OBJECT_VARIANT(BooleanLiteralType(false)) });

	objects.insert({ 8, MAKE_OBJECT_VARIANT(BooleanObject(true)) });
	objects.insert({ 9, MAKE_OBJECT_VARIANT(BooleanObject(false)) });
}

Object_ptr ObjectStore::get(int id)
{
	ASSERT(objects.contains(id), "ID does not exist in ObjectStore");
	return objects.at(id);
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

Object_ptr ObjectStore::get_true_literal_type()
{
	return get(6);
}

Object_ptr ObjectStore::get_false_literal_type()
{
	return get(7);
}

Object_ptr ObjectStore::get_true_object()
{
	return get(8);
}

Object_ptr ObjectStore::get_false_object()
{
	return get(9);
}

Object_ptr ObjectStore::make_object(bool value)
{
	if (value)
	{
		return get_true_object();
	}

	return get_false_object();
}

Object_ptr ObjectStore::make_object(int value)
{
	return MAKE_OBJECT_VARIANT(IntObject(value));
}

Object_ptr ObjectStore::make_object(double value)
{
	return MAKE_OBJECT_VARIANT(FloatObject(value));
}

Object_ptr ObjectStore::make_object(std::wstring value)
{
	return MAKE_OBJECT_VARIANT(StringObject(value));
}

Object_ptr ObjectStore::make_error_object(std::wstring text)
{
	return MAKE_OBJECT_VARIANT(StringObject(text));
}

// Variable Store

void DefinitionStore::create(int id, Object_ptr value)
{
	ASSERT(!objects.contains(id), "ID already exists in ObjectStore");
	objects.insert({ id, value });
}

void DefinitionStore::set(int id, Object_ptr value)
{
	objects.insert({ id, value });
}

void DefinitionStore::discard(int id)
{
	ASSERT(objects.contains(id), "ID does not exist in ObjectStore");
	objects.erase(id);
}

// Constant Pool

int ConstantPool::allocate()
{
	int id = next_id++;
	// don't allocate anything
	return id;
}

int ConstantPool::allocate(Object_ptr value)
{
	int id = next_id++;
	objects.insert({ id, move(value) });

	return id;
}

int ConstantPool::allocate(int number)
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

	int id = next_id++;
	auto value = MAKE_OBJECT_VARIANT(IntObject(number));
	objects.insert({ id, value });

	return id;
}

int ConstantPool::allocate(double number)
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

	int id = next_id++;
	auto value = MAKE_OBJECT_VARIANT(FloatObject(number));
	objects.insert({ id, value });

	return id;
}

int ConstantPool::allocate(std::wstring text)
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

	int id = next_id++;
	auto value = MAKE_OBJECT_VARIANT(StringObject(text));
	objects.insert({ id, value });

	return id;
}

int ConstantPool::allocate_enum_member(int enum_id, int member_id)
{
	auto result = find_if(
		objects.begin(),
		objects.end(),
		[enum_id, member_id](const auto& p) {
			if (holds_alternative<EnumMemberObject>(*p.second))
			{
				EnumMemberObject* x = get_if<EnumMemberObject>(&*p.second);
				return x->enum_id == enum_id && x->member_id == member_id;
			}

			return false;
		});

	if (result != objects.end())
	{
		return result->first;
	}

	int id = next_id++;
	auto value = MAKE_OBJECT_VARIANT(EnumMemberObject(enum_id, member_id));
	objects.insert({ id, value });

	return id;
}