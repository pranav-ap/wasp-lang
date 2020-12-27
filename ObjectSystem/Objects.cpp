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

		auto map_type = get_if<MapType>(&*type);
		auto tuple_type = MAKE_OBJECT_VARIANT(TupleType({ map_type->key_type, map_type->value_type }));
		return MAKE_OBJECT_VARIANT(TupleObject({ key, value }, tuple_type));
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

int SetObject::get_length()
{
	return 0;
}

// VariantObject

bool VariantObject::has_value()
{
	return value->index() != 0;
}

// Utils

std::wstring stringify_object(Object_ptr value)
{
	auto s = wstring(L"Object");

	return std::visit(overloaded{
		[&](IntObject const& obj) { return STR(obj.value); },
		[&](FloatObject const& obj) { return STR(obj.value); },
		[&](StringObject const& obj) { return obj.value; },
		[&](BooleanObject const& obj) { return STR(obj.value); },

		[&](ListObject const& obj) { return s; },
		[&](TupleObject const& obj) { return s; },
		[&](SetObject const& obj) { return s; },
		
		[&](EnumMemberObject const& obj) { return s; },
		[&](MapObject const& obj) { return s; },

		[&](VariantObject const& obj) { return s; },
		[&](FunctionObject const& obj) { return s; },
		[&](GeneratorObject const& obj) { return s; },
		[&](FunctionMethodObject const& obj) { return s; },
		[&](GeneratorMethodObject const& obj) { return s; },
		[&](ReturnObject const& obj) { return s; },
		[&](YieldObject const& obj) { return s; },
		[&](ErrorObject const& obj) { return s; },
		[&](BreakObject const& obj) { return wstring(L"break"); },
		[&](ContinueObject const& obj) { return wstring(L"continue"); },
		[&](RedoObject const& obj) { return wstring(L"redo"); },
		[&](NoneObject const& obj) { return wstring(L"none"); },
		[&](BuiltInsObject const& obj) { return s; },
		[&](EnumObject const& obj) { return s; },
		[&](ClassObject const& obj) { return s; },
		[&](InstanceObject const& obj) { return s; },

		// Types

		[&](AnyType const& obj) { return s; },
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
		[&](ClassType const& obj) { return s; },
		[&](AliasType const& obj) { return s; },
		[&](InterfaceType const& obj) { return s; },
		[&](EnumType const& obj) { return s; },
		[&](VariantType const& obj) { return s; },
		[&](NoneType const& obj) { return s; },
		[&](FunctionType const& obj) { return s; },
		[&](GeneratorType const& obj) { return s; },
		[&](FunctionMemberType const& obj) { return s; },
		[&](GeneratorMemberType const& obj) { return s; },
		[&](auto) { return wstring(L" "); }
		}, *value);
}