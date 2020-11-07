#include "pch.h"
#include "ConstantPool.h"
#include "ObjectSystem.h"
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

int ConstantPool::allocate()
{
	int id = pool.size();
	pool.insert({ id, move(MAKE_OBJECT_VARIANT(NoneObject(get_none_type()))) });

	return id;
}

int ConstantPool::allocate(int number)
{
	auto result = find_if(
		pool.begin(),
		pool.end(),
		[number](const auto& p) {
			if (holds_alternative<IntObject>(*p.second))
			{
				IntObject* x = get_if<IntObject>(&*p.second);
				return x->value == number;
			}

			return false;
		});

	if (result != pool.end())
	{
		return result->first;
	}

	int id = pool.size();
	auto value = MAKE_OBJECT_VARIANT(IntObject(number, get_int_type()));
	pool.insert({ id, value });

	return id;
}

int ConstantPool::allocate(double number)
{
	auto result = find_if(
		pool.begin(),
		pool.end(),
		[number](const auto& p) {
			if (holds_alternative<FloatObject>(*p.second))
			{
				FloatObject* x = get_if<FloatObject>(&*p.second);
				return x->value == number;
			}

			return false;
		});

	if (result != pool.end())
	{
		return result->first;
	}

	int id = pool.size();
	auto value = MAKE_OBJECT_VARIANT(FloatObject(number, get_float_type()));
	pool.insert({ id, value });

	return id;
}

int ConstantPool::allocate(std::wstring text)
{
	auto result = find_if(
		pool.begin(),
		pool.end(),
		[text](const auto& p) {
			if (holds_alternative<StringObject>(*p.second))
			{
				StringObject* x = get_if<StringObject>(&*p.second);
				return x->value == text;
			}

			return false;
		});

	if (result != pool.end())
	{
		return result->first;
	}

	int id = pool.size();
	auto value = MAKE_OBJECT_VARIANT(StringObject(text, get_string_type()));
	pool.insert({ id, value });

	return id;
}

int ConstantPool::allocate(Object_ptr value)
{
	int id = pool.size();
	pool.insert({ id, move(value) });

	return id;
}

Object_ptr ConstantPool::get(int id)
{
	if (pool.contains(id))
	{
		auto value = pool.at(id);
		return value;
	}

	return MAKE_OBJECT_VARIANT(ErrorObject(L"Constant does not exist in pool"));
}

void ConstantPool::set(int id, Object_ptr value)
{
	pool.at(id) = move(value);
}
