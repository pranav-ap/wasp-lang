#pragma once
#include "pch.h"
#include "ObjectSystem.h"
#include "spdlog.h"
#include "CommonAssertion.h"

#include <cmath>
#include <string>
#include <vector>
#include <exception>
#include <variant>

#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define VOID std::make_shared<Object>(ReturnObject())
#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")

#define THROW(message)													\
		spdlog::error(message);											\
		return std::make_shared<Object>(ErrorObject(message));

#define THROW_ASSERT(condition, message)								\
	if (!condition) {													\
		spdlog::error(message);											\
		return std::make_shared<Object>(ErrorObject(message));	\
	}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::string;
using std::vector;
using std::get;

Object_ptr ListObject::append(Object_ptr value)
{
	NULL_CHECK(value);
	THROW_ASSERT(value->index() != 0, "Cannot add monostate to VectorObject");
	values.push_back(value);
	return VOID;
}

Object_ptr ListObject::prepend(Object_ptr value)
{
	NULL_CHECK(value);
	THROW_ASSERT(value->index() != 0, "Cannot add monostate to VectorObject");
	values.push_front(value);
	return VOID;
}

Object_ptr ListObject::pop_back()
{
	if (!values.empty())
	{
		auto value = values.back();
		values.pop_back();
		return value;
	}

	THROW("Vector is empty");
}

Object_ptr ListObject::pop_front()
{
	if (!values.empty())
	{
		auto value = values.front();
		values.pop_front();
		return value;
	}

	THROW("Vector is empty");
}

Object_ptr ListObject::get(Object_ptr index_object)
{
	try
	{
		auto index = std::get<double>(*index_object);

		double intpart;
		ASSERT(modf(index, &intpart) == 0.0, "Index must be an integer");

		auto value = values.at(intpart);
		NULL_CHECK(value);
		return value;
	}
	catch (std::out_of_range&)
	{
		THROW("Index is out of range");
	}
	catch (std::bad_variant_access&)
	{
		THROW("Index must be an integer");
	}
}

Object_ptr ListObject::set(Object_ptr index_object, Object_ptr value)
{
	NULL_CHECK(value);
	THROW_ASSERT(value->index() != 0, "Cannot add monostate to VectorObject");

	try
	{
		auto index = std::get<double>(*index_object);

		double intpart;
		ASSERT(modf(index, &intpart) == 0.0, "Index must be an integer");

		values.at(intpart) = move(value);
		return VOID;
	}
	catch (std::out_of_range&)
	{
		THROW("Index is out of range");
	}
	catch (std::bad_variant_access&)
	{
		THROW("Index must be an integer");
	}
}

void ListObject::clear()
{
	values.clear();
}

bool ListObject::is_empty()
{
	return values.empty();
}

int ListObject::get_length()
{
	return values.size();
}

Object_ptr DictionaryObject::insert(Object_ptr key, Object_ptr value)
{
	NULL_CHECK(key);
	NULL_CHECK(value);
	THROW_ASSERT(value->index() != 0, "Cannot assign monostate to a key in DictionaryObject");

	const auto [_, success] = pairs.insert({ key, value });
	ASSERT(success, "Unable to assign value to key in Map");

	return VOID;
}

Object_ptr DictionaryObject::set(Object_ptr key, Object_ptr value)
{
	NULL_CHECK(key);
	NULL_CHECK(value);
	THROW_ASSERT(value->index() != 0, "Cannot assign monostate to a key in DictionaryObject");
	pairs.insert_or_assign(key, value);
	return VOID;
}

int DictionaryObject::get_size()
{
	return pairs.size();
}

Object_ptr DictionaryObject::get_pair(Object_ptr key)
{
	NULL_CHECK(key);

	try
	{
		auto value = pairs.at(key);
		NULL_CHECK(value);

		Object_ptr key_object = std::visit(overloaded{
			[](double num) { return MAKE_OBJECT_VARIANT(num); },
			[](std::string& text) { return MAKE_OBJECT_VARIANT(text); },
			[](bool boolean) { return MAKE_OBJECT_VARIANT(boolean); },

			[&](auto) { THROW("Cannot iterate over this datatype"); }
			}, *key);

		return MAKE_OBJECT_VARIANT(TupleObject({ key_object, value }));
	}
	catch (std::out_of_range&)
	{
		THROW("Key is not available");
	}
}

Object_ptr DictionaryObject::get(Object_ptr key)
{
	NULL_CHECK(key);

	try
	{
		auto value = pairs.at(key);
		NULL_CHECK(value);
		return value;
	}
	catch (std::out_of_range&)
	{
		THROW("Key is not available");
	}
}

Object_ptr TupleObject::get(Object_ptr index_object)
{
	try
	{
		auto index = std::get<double>(*index_object);

		double intpart;
		ASSERT(modf(index, &intpart) == 0.0, "Index must be an integer");

		auto value = values.at(intpart);
		NULL_CHECK(value);
		return value;
	}
	catch (std::out_of_range&)
	{
		THROW("Index is out of range");
	}
	catch (std::bad_variant_access&)
	{
		THROW("Index must be an integer");
	}
}

Object_ptr TupleObject::set(Object_ptr index_object, Object_ptr value)
{
	NULL_CHECK(value);

	try
	{
		auto index = std::get<double>(*index_object);

		double intpart;
		ASSERT(modf(index, &intpart) == 0.0, "Index must be an integer");

		values.at(intpart) = move(value);
	}
	catch (std::out_of_range&)
	{
		THROW("Index is out of range");
	}
	catch (std::bad_variant_access&)
	{
		THROW("Index must be an integer");
	}
}

Object_ptr TupleObject::set(std::vector<Object_ptr> values)
{
	this->values = values;
	return VOID;
}

int TupleObject::get_length()
{
	return values.size();
}

bool VariantObject::has_value()
{
	return value->index() != 0;
}