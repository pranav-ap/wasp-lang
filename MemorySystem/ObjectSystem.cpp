#pragma once
#include "pch.h"
#include "ObjectSystem.h"
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

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::string;
using std::wstring;
using std::vector;
using std::get;
using std::to_wstring;

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
		values.at(index.value) = move(value);
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

std::wstring ListObject::stringify() const
{
	return L"VariantObject";
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
		values.at(index.value) = move(value);
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

Object_ptr TupleObject::set(std::vector<Object_ptr> values)
{
	this->values = values;
	return VOID;
}

int TupleObject::get_length()
{
	return values.size();
}

// SetObject

std::vector<Object_ptr> SetObject::get()
{
	return this->values;
}

Object_ptr SetObject::set(std::vector<Object_ptr> values)
{
	this->values = values;
	return VOID;
}

int SetObject::get_length()
{
	return 0;
}

// VariantObject

bool VariantObject::has_value()
{
	return value->index() != 0;
}

// Stringify

std::wstring VariantObject::stringify() const const
{
	return L"VariantObject";
}

std::wstring NoneObject::stringify() const
{
	return L"NoneObject";
}

std::wstring TupleObject::stringify() const
{
	return L"TupleObject";
}

std::wstring SetObject::stringify() const
{
	return L"SetObject";
}

std::wstring MapObject::stringify() const
{
	return L"MapObject";
}

std::wstring FunctionObject::stringify() const
{
	return L"FunctionObject : " + this->name;
}

std::wstring GeneratorObject::stringify() const
{
	return L"GeneratorObject : " + this->name;
}

std::wstring IntObject::stringify() const
{
	return to_wstring(this->value);
}

std::wstring FloatObject::stringify() const
{
	return to_wstring(this->value);
}

std::wstring StringObject::stringify() const
{
	return this->value;
}

std::wstring BooleanObject::stringify() const
{
	return to_wstring(this->value);
}

std::wstring EnumMemberObject::stringify() const
{
	return L"EnumMemberObject";
}

std::wstring BreakObject::stringify() const
{
	return L"BreakObject";
}

std::wstring ContinueObject::stringify() const
{
	return L"ContinueObject";
}

std::wstring BuiltInsObject::stringify() const
{
	return L"BuiltInsObject";
}

std::wstring ReturnObject::stringify() const
{
	return L"ReturnObject";
}

std::wstring YieldObject::stringify() const
{
	return L"YieldObject";
}

std::wstring ErrorObject::stringify() const
{
	return L"ErrorObject";
}

std::wstring ClassObject::stringify() const
{
	return L"ClassObject";
}

std::wstring InstanceObject::stringify() const
{
	return L"InstanceObject";
}

std::wstring FunctionMethodObject::stringify() const
{
	return L"FunctionMethodObject";
}

std::wstring GeneratorMethodObject::stringify() const
{
	return L"GeneratorMethodObject";
}

std::wstring EnumObject::stringify() const
{
	return L"EnumObject";
}

// Utils

std::wstring stringify_object(Object_ptr value)
{
	return std::visit(overloaded{
		[&](IntObject const& obj) { return obj.stringify(); },
		[&](FloatObject const& obj) { return obj.stringify(); },
		[&](StringObject const& obj) { return obj.stringify(); },
		[&](BooleanObject const& obj) { return obj.stringify(); },

		[&](ListObject const& obj) { return obj.stringify(); },
		[&](TupleObject const& obj) { return obj.stringify(); },
		[&](MapObject const& obj) { return obj.stringify(); },
		[&](EnumMemberObject const& obj) { return obj.stringify(); },
		[&](VariantObject const& obj) { return obj.stringify(); },

		[&](FunctionObject const& obj) { return obj.stringify(); },
		[&](GeneratorObject const& obj) { return obj.stringify(); },

		[&](ReturnObject const& obj) { return obj.stringify(); },
		[&](ErrorObject const& obj) { return obj.stringify(); },
		[&](BreakObject const& obj) { return obj.stringify(); },
		[&](ContinueObject const& obj) { return obj.stringify(); },
		[&](BuiltInsObject const& obj) { return obj.stringify(); },
		[&](NoneObject const& obj) { return obj.stringify(); },

		[](auto)
		{
			wstring empty = L"";
			return empty;
		}
		}, *value);
}