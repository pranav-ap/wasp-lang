#pragma once
#include "pch.h"
#include "ObjectSystem.h"
#include <iostream>
#include <string>

using std::string;
using std::to_string;

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

// Printers

std::ostream& operator<<(std::ostream& os, const NumberObject_ptr obj)
{
	os << to_string(obj->value);
	return os;
}

std::ostream& operator<<(std::ostream& os, const StringObject_ptr obj)
{
	os << obj->value;
	return os;
}

std::ostream& operator<<(std::ostream& os, const BooleanObject_ptr obj)
{
	os << to_string(obj->value);
	return os;
}

std::ostream& operator<<(std::ostream& os, const VectorObject_ptr obj)
{
	string x = "Vector Object";
	os << x;
	return os;
}

std::ostream& operator<<(std::ostream& os, const MapObject_ptr obj)
{
	string x = "Map Object";
	os << x;
	return os;
}

std::ostream& operator<<(std::ostream& os, const UDTObject_ptr obj)
{
	string x = "Map Object";
	os << x;
	return os;
}