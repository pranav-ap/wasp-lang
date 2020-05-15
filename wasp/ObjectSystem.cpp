#pragma once
#include "ObjectSystem.h"
#include "Assertion.h"
#include "spdlog.h"

#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <exception>
#include <variant>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define VOID std::make_shared<Object>(ReturnObject())
#define THROW(message) return std::make_shared<Object>(ErrorObject(message));
#define THROW_ASSERT(condition, message) \
	if (!condition) { \
		spdlog::error(message); \
		return std::make_shared<Object>(ErrorObject(message));	\
	}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::string;
using std::to_string;
using std::vector;
using std::get;
using std::move;

// ListObject

Object_ptr ListObject::append(Object_ptr value)
{
	NULL_CHECK(value);
	THROW_ASSERT(value->index() != 0, "Cannot add monostate to VectorObject");
	values.push_back(move(value));
	return VOID;
}

Object_ptr ListObject::prepend(Object_ptr value)
{
	NULL_CHECK(value);
	THROW_ASSERT(value->index() != 0, "Cannot add monostate to VectorObject");
	values.push_front(move(value));
	return VOID;
}

Object_ptr ListObject::pop_back()
{
	if (!values.empty())
	{
		auto value = move(values.back());
		values.pop_back();
		return value;
	}

	THROW("Vector is empty");
}

Object_ptr ListObject::pop_front()
{
	if (!values.empty())
	{
		auto value = move(values.front());
		values.pop_front();
		return value;
	}

	THROW("Vector is empty");
}

Object_ptr ListObject::get(Object_ptr index_object)
{
	try
	{
		auto index = std::get<NumberObject>(*index_object).value;

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
		auto index = std::get<NumberObject>(*index_object).value;

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

Object_ptr ListObject::set(std::deque<Object_ptr> values)
{
	this->values = values;
	return VOID;
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

// MapObject

Object_ptr MapObject::insert(Object_ptr key, Object_ptr value)
{
	NULL_CHECK(key);
	NULL_CHECK(value);
	THROW_ASSERT(value->index() != 0, "Cannot assign monostate to a key in DictionaryObject");

	const auto [_, success] = pairs.insert({ key, value });
	ASSERT(success, "Unable to assign value to key in Map");

	return VOID;
}

Object_ptr MapObject::remove(Object_ptr key)
{
	NULL_CHECK(key);

	if (pairs.contains(key))
	{
		pairs.erase(key);
		return VOID;
	}

	THROW("Dictionary does not contain this key");
}

Object_ptr MapObject::set(Object_ptr key, Object_ptr value)
{
	NULL_CHECK(key);
	NULL_CHECK(value);
	THROW_ASSERT(value->index() != 0, "Cannot assign monostate to a key in DictionaryObject");
	pairs.insert_or_assign(key, value);
	return VOID;
}

int MapObject::get_size()
{
	return pairs.size();
}

Object_ptr MapObject::get_pair(Object_ptr key)
{
	NULL_CHECK(key);

	try
	{
		auto value = pairs.at(key);
		NULL_CHECK(value);

		return MAKE_OBJECT_VARIANT(TupleObject({ key, value }));
	}
	catch (std::out_of_range&)
	{
		THROW("Key is not available");
	}
}

Object_ptr MapObject::get(Object_ptr key)
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

// UDTObject

Object_ptr UDTObject::get(Object_ptr key)
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

Object_ptr UDTObject::set(Object_ptr key, Object_ptr value)
{
	NULL_CHECK(key);
	NULL_CHECK(value);
	THROW_ASSERT(value->index() != 0, "Cannot assign monostate to a key in DictionaryObject");
	pairs.insert_or_assign(key, value);
	return VOID;
}

// TupleObject

Object_ptr TupleObject::get(Object_ptr index_object)
{
	try
	{
		auto index = std::get<NumberObject>(*index_object).value;

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
		auto index = std::get<NumberObject>(*index_object).value;

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

// VariantObject

bool VariantObject::has_value()
{
	return value->index() != 0;
}

Object_ptr VariantObject::safe_get_value()
{
	if (has_value())
	{
		return value;
	}

	return MAKE_OBJECT_VARIANT(NoneObject());
}

// Stringify

std::string ListObject::stringify()
{
	return "ListObject";
}

std::string TupleObject::stringify()
{
	return "TupleObject";
}

std::string MapObject::stringify()
{
	return "MapObject";
}

std::string UDTObject::stringify()
{
	return "UDTObject";
}

std::string VariantObject::stringify()
{
	return "VariantObject";
}

std::string NumberObject::stringify()
{
	return to_string(value);
}

std::string StringObject::stringify()
{
	return value;
}

std::string BooleanObject::stringify()
{
	return to_string(value);
}

std::string EnumMemberObject::stringify()
{
	return enum_name + "::" + member_name;
}

std::string ErrorObject::stringify()
{
	return message;
}

std::string PassObject::stringify()
{
	return "pass";
}

std::string BreakObject::stringify()
{
	return "break";
}

std::string ContinueObject::stringify()
{
	return "continue";
}

std::string ReturnObject::stringify()
{
	return "return";
}

std::string NoneObject::stringify()
{
	return "none";
}