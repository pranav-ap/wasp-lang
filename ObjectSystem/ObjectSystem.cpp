#pragma once
#include "pch.h"
#include "ObjectSystem.h"
#include <string>

using std::string;

void VectorObject::add(Object_ptr value)
{
	values.push_back(value);
}

void VectorObject::accept()
{
}

void MapObject::add(Object_ptr key, Object_ptr value)
{
	pairs.insert_or_assign(key, value);
}

void MapObject::accept()
{
}

void UDTObject::add(std::string key, Object_ptr value)
{
	pairs.insert_or_assign(key, value);
}

void UDTObject::accept()
{
}

void OptionalObject::accept()
{
}

void NumberObject::accept()
{
}

void StringObject::accept()
{
}

void BooleanObject::accept()
{
}