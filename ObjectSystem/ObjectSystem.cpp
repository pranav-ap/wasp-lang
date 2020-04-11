#pragma once
#include "pch.h"
#include "ObjectSystem.h"
#include "spdlog.h"

#include <string>
#include <exception>

#define MAKE_OBJECT_VARIANT(x) std::make_shared<ObjectVariant>(x)
#define VOID std::make_shared<ObjectVariant>(ReturnObject())

#define THROW(message)													\
		spdlog::error(message);											\
		return std::make_shared<ObjectVariant>(ErrorObject(message));

#define THROW_ASSERT(condition, message)								\
	if (!condition) {													\
		spdlog::error(message);											\
		return std::make_shared<ObjectVariant>(ErrorObject(message));	\
	}

using std::string;

ObjectVariant_ptr VectorObject::add(ObjectVariant_ptr value)
{
	THROW_ASSERT(value->index() != 0, "Cannot add monostate to VectorObject");
	values.push_back(value);
	return VOID;
}

ObjectVariant_ptr VectorObject::get_element(double index)
{
	try
	{
		return values.at(index);
	}
	catch (std::out_of_range&)
	{
		THROW("Index is out of range");
	}
}

ObjectVariant_ptr UDTObject::add(string key, ObjectVariant_ptr value)
{
	THROW_ASSERT(value->index() == 0, "Cannot assign monostate to a key in UDTObject");
	pairs.insert_or_assign(key, value);
	return VOID;
}

ObjectVariant_ptr UDTObject::get_pair(std::string key)
{
	try
	{
		auto value = pairs.at(key);
		return MAKE_OBJECT_VARIANT(UDTKeyValuePairObject(key, move(value)));
	}
	catch (std::out_of_range&)
	{
		THROW("Key " + key + " is not available");
	}
}

ObjectVariant_ptr UDTObject::get_value(std::string key)
{
	try
	{
		return pairs.at(key);
	}
	catch (std::out_of_range&)
	{
		THROW("Key " + key + " is not available");
	}
}