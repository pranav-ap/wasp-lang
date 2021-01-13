#pragma once
#include "pch.h"
#include "Objects.h"
#include "Assertion.h"
#include <string>
#include <vector>
#include <exception>
#include <variant>
#include <stdexcept>

#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define VOID std::make_shared<Object>(ReturnObject())
#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define THROW(message) return std::make_shared<Object>(ErrorObject(message));
#define THROW_IF(condition, message) if (!condition) { return std::make_shared<Object>(ErrorObject(message)); }
#define STR(x) to_wstring(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::string;
using std::wstring;
using std::vector;
using std::get;
using std::get_if;
using std::to_wstring;

// StringObject

Object_ptr StringObject::get_iter()
{
	ObjectVector vec = to_vector(value);
	return MAKE_OBJECT_VARIANT(IteratorObject(vec));
}

// ListObject

Object_ptr ListObject::append(Object_ptr value)
{
	NULL_CHECK(value);
	THROW_IF(value->index() != 0, L"Cannot add monostate to VectorObject");
	values.push_back(value);
	return VOID;
}

Object_ptr ListObject::prepend(Object_ptr value)
{
	NULL_CHECK(value);
	THROW_IF(value->index() != 0, L"Cannot add monostate to VectorObject");
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

	THROW(L"Vector is empty");
}

Object_ptr ListObject::pop_front()
{
	if (!values.empty())
	{
		auto value = values.front();
		values.pop_front();
		return value;
	}

	THROW(L"Vector is empty");
}

Object_ptr ListObject::get(Object_ptr index_object)
{
	try
	{
		auto index = std::get<IntObject>(*index_object);
		auto value = values.at(index.value);
		NULL_CHECK(value);
		return value;
	}
	catch (std::out_of_range&)
	{
		THROW(L"Index is out of range");
	}
	catch (std::bad_variant_access&)
	{
		THROW(L"Index must be an integer");
	}
}

Object_ptr ListObject::set(Object_ptr index_object, Object_ptr value)
{
	NULL_CHECK(value);
	THROW_IF(value->index() != 0, L"Cannot add monostate to VectorObject");

	try
	{
		auto index = std::get<IntObject>(*index_object);
		values.at(index.value) = value;
		return VOID;
	}
	catch (std::out_of_range&)
	{
		THROW(L"Index is out of range");
	}
	catch (std::bad_variant_access&)
	{
		THROW(L"Index must be an integer");
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

Object_ptr ListObject::get_iter()
{
	ObjectVector vec = to_vector(values);
	return MAKE_OBJECT_VARIANT(IteratorObject(vec));
}

ListObject::ListObject(ObjectVector values)
{
	for (auto value : values)
	{
		this->values.push_back(value);
	}
}

// MapObject

Object_ptr MapObject::insert(Object_ptr key, Object_ptr value)
{
	NULL_CHECK(key);
	NULL_CHECK(value);
	THROW_IF(value->index() != 0, L"Cannot assign monostate to a key in DictionaryObject");

	const auto [_, success] = pairs.insert({ key, value });
	ASSERT(success, "Unable to assign value to key in Map");

	return VOID;
}

Object_ptr MapObject::set(Object_ptr key, Object_ptr value)
{
	NULL_CHECK(key);
	NULL_CHECK(value);
	THROW_IF(value->index() != 0, L"Cannot assign monostate to a key in DictionaryObject");
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
		THROW(L"Key is not available");
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
		THROW(L"Key is not available");
	}
}

// TupleObject

Object_ptr TupleObject::get(Object_ptr index_object)
{
	try
	{
		auto index = std::get<IntObject>(*index_object);
		auto value = values.at(index.value);
		NULL_CHECK(value);
		return value;
	}
	catch (std::out_of_range&)
	{
		THROW(L"Index is out of range");
	}
	catch (std::bad_variant_access&)
	{
		THROW(L"Index must be an integer");
	}
}

Object_ptr TupleObject::set(Object_ptr index_object, Object_ptr value)
{
	NULL_CHECK(value);

	try
	{
		auto index = std::get<IntObject>(*index_object);
		values.at(index.value) = value;
	}
	catch (std::out_of_range&)
	{
		THROW(L"Index is out of range");
	}
	catch (std::bad_variant_access&)
	{
		THROW(L"Index must be an integer");
	}
}

Object_ptr TupleObject::set(ObjectVector values)
{
	this->values = values;
	return VOID;
}

int TupleObject::get_length()
{
	return values.size();
}

// SetObject

ObjectVector SetObject::get()
{
	return this->values;
}

Object_ptr SetObject::set(ObjectVector values)
{
	this->values = values;
	return VOID;
}

Object_ptr SetObject::get_iter()
{
	return MAKE_OBJECT_VARIANT(IteratorObject(values));
}

int SetObject::get_length()
{
	return 0;
}

// MapObject

Object_ptr MapObject::get_iter()
{
	ObjectVector tuples;

	for (auto p : pairs)
	{
		auto tuple_pair = MAKE_OBJECT_VARIANT(TupleObject({ p.first, p.second }));
		tuples.push_back(tuple_pair);
	}

	return MAKE_OBJECT_VARIANT(IteratorObject(tuples));
}

// IteratorObject

std::optional<Object_ptr> IteratorObject::get_next()
{
	it++;

	if (it == std::end(vec))
	{
		return std::nullopt;
	}

	Object_ptr obj = *it;
	return std::make_optional(obj);
}

void IteratorObject::reset_iter()
{
	it = std::begin(vec);
}

// VariantObject

bool VariantObject::has_value()
{
	return value->index() != 0;
}

// Utils

ObjectVector to_vector(std::deque<Object_ptr> values)
{
	ObjectVector vec;

	for (auto value : values)
	{
		vec.push_back(value);
	}

	return vec;
}

ObjectVector to_vector(std::wstring text)
{
	ObjectVector vec;

	for (auto ch : text)
	{
		vec.push_back(MAKE_OBJECT_VARIANT(StringObject(std::to_wstring(ch))));
	}

	return vec;
}

std::wstring stringify_object(Object_ptr value)
{
	auto s = wstring(L"Object");

	return std::visit(overloaded{
		[&](IntObject const& obj) { return STR(obj.value); },
		[&](FloatObject const& obj) { return STR(obj.value); },
		[&](StringObject const& obj) { return obj.value; },
		[&](BooleanObject const& obj) { return STR(obj.value); },
		[&](ListObject const& obj) { return wstring(L"List Object"); },
		[&](TupleObject const& obj) { return wstring(L"Tuple Object"); },
		[&](SetObject const& obj) { return wstring(L"Set Object"); },
		[&](MapObject const& obj) { return wstring(L"Map Object"); },
		[&](VariantObject const& obj) { return wstring(L"Variant Object"); },
		[&](ReturnObject const& obj) { return wstring(L"Return Object"); },
		[&](YieldObject const& obj) { return wstring(L"Yield Object"); },
		[&](BuiltInObject const& obj) { return wstring(L"BuiltIns Object"); },
		[&](ErrorObject const& obj) { return obj.message; },
		[&](NoneObject const& obj) { return wstring(L"none"); },
		[&](BreakObject const& obj) { return wstring(L"break"); },
		[&](RedoObject const& obj) { return wstring(L"redo"); },
		[&](ContinueObject const& obj) { return wstring(L"continue"); },
		[&](IteratorObject const& obj) { return wstring(L"break"); },
		[&](EnumObject const& obj) { return wstring(L"enum " + obj.name); },
		[&](EnumMemberObject const& obj) { return wstring(L"EnumMemberObject"); },
		[&](FunctionObject const& obj) { return wstring(L"FunctionObject " + obj.name); },
		[&](BuiltInFunctionObject const& obj) { return wstring(L"builtin " + obj.name); },

		// Types

		[&](AnyType const& obj) { return wstring(L"any"); },
		[&](IntLiteralType const& obj) { return s; },
		[&](FloatLiteralType const& obj) { return s; },
		[&](StringLiteralType const& obj) { return s; },
		[&](BooleanLiteralType const& obj) { return s; },
		[&](IntType const& obj) { return s; },
		[&](FloatType const& obj) { return s; },
		[&](StringType const& obj) { return s; },
		[&](BooleanType const& obj) { return s; },
		[&](ListType const& obj) { return s; },
		[&](TupleType const& obj) { return s; },
		[&](SetType const& obj) { return s; },
		[&](MapType const& obj) { return s; },
		[&](VariantType const& obj) { return s; },
		[&](NoneType const& obj) { return s; },
		[&](FunctionType const& obj) { return s; },

		[&](auto) { return wstring(L" "); }
		}, *value);
}