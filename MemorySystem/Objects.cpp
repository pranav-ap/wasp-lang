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
	return std::visit(overloaded{
		// Object

		[&](IntObject const& obj) { return L"Object"; },
		[&](FloatObject const& obj) { return L"Object"; },
		[&](StringObject const& obj) { return L"Object"; },
		[&](BooleanObject const& obj) { return L"Object"; },
		[&](ListObject const& obj) { return L"Object"; },
		[&](TupleObject const& obj) { return L"Object"; },
		[&](SetObject const& obj) { return L"Object"; },
		[&](MapObject const& obj) { return L"Object"; },
		[&](EnumMemberObject const& obj) { return L"Object"; },
		[&](VariantObject const& obj) { return L"Object"; },
		[&](FunctionObject const& obj) { return L"Object"; },
		[&](GeneratorObject const& obj) { return L"Object"; },
		[&](FunctionMethodObject const& obj) { return L"Object"; },
		[&](GeneratorMethodObject const& obj) { return L"Object"; },
		[&](ReturnObject const& obj) { return L"Object"; },
		[&](YieldObject const& obj) { return L"Object"; },
		[&](ErrorObject const& obj) { return L"Object"; },
		[&](BreakObject const& obj) { return L"Object"; },
		[&](ContinueObject const& obj) { return L"Object"; },
		[&](BuiltInsObject const& obj) { return L"Object"; },
		[&](NoneObject const& obj) { return L"Object"; },
		[&](EnumObject const& obj) { return L"Object"; },
		[&](ClassObject const& obj) { return L"Object"; },
		[&](InstanceObject const& obj) { return L"Object"; },

		// Types Object

		[&](AnyType const& obj) { return L"Object"; },
		[&](IntLiteralType const& obj) { return L"Object"; },
		[&](FloatLiteralType const& obj) { return L"Object"; },
		[&](StringLiteralType const& obj) { return L"Object"; },
		[&](BooleanLiteralType const& obj) { return L"Object"; },
		[&](IntType const& obj) { return L"Object"; },
		[&](FloatType const& obj) { return L"Object"; },
		[&](StringType const& obj) { return L"Object"; },
		[&](BooleanType const& obj) { return L"Object"; },
		[&](TupleType const& obj) { return L"Object"; },
		[&](SetType const& obj) { return L"Object"; },
		[&](MapType const& obj) { return L"Object"; },
		[&](ClassType const& obj) { return L"Object"; },
		[&](AliasType const& obj) { return L"Object"; },
		[&](InterfaceType const& obj) { return L"Object"; },
		[&](EnumType const& obj) { return L"Object"; },
		[&](VariantType const& obj) { return L"Object"; },
		[&](NoneType const& obj) { return L"Object"; },
		[&](FunctionType const& obj) { return L"Object"; },
		[&](GeneratorType const& obj) { return L"Object"; },
		[&](FunctionMemberType const& obj) { return L"Object"; },
		[&](GeneratorMemberType const& obj) { return L"Object"; },
		[&](OperatorType const& obj) { return L"Object"; },

		[](auto)
		{
			wstring empty = L"";
			return empty;
		}
		}, *value);
}