#pragma once
#include "pch.h"
#include "ObjectSystem.h"
#include "ObjectVisitor.h"
#include <iostream>
#include <string>

#define INIT_PTR_P(name) std::shared_ptr<name> p{ shared_from_this() }

using std::string;
using std::to_string;
using std::cout;
using std::endl;

void VectorObject::add(Object_ptr value)
{
	values.push_back(value);
}

void UDTObject::add(std::string key, Object_ptr value)
{
	pairs.insert_or_assign(key, value);
}

// Accept

void NumberObject::accept(ObjectVisitor& visitor)
{
	INIT_PTR_P(NumberObject);
	return visitor.visit(p);
}

void StringObject::accept(ObjectVisitor& visitor)
{
	INIT_PTR_P(StringObject);
	return visitor.visit(p);
}

void BooleanObject::accept(ObjectVisitor& visitor)
{
	INIT_PTR_P(BooleanObject);
	return visitor.visit(p);
}

void VectorObject::accept(ObjectVisitor& visitor)
{
	INIT_PTR_P(VectorObject);
	return visitor.visit(p);
}

void UDTObject::accept(ObjectVisitor& visitor)
{
	INIT_PTR_P(UDTObject);
	return visitor.visit(p);
}

void OptionalObject::accept(ObjectVisitor& visitor)
{
	INIT_PTR_P(OptionalObject);
	return visitor.visit(p);
}

void ReturnObject::accept(ObjectVisitor& visitor)
{
	INIT_PTR_P(ReturnObject);
	return visitor.visit(p);
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

std::ostream& operator<<(std::ostream& os, const UDTObject_ptr obj)
{
	string x = "Map Object";
	os << x;
	return os;
}

std::ostream& operator<<(std::ostream& os, const OptionalObject_ptr obj)
{
	os << "Optional Object : ";

	if (obj->value.has_value())
	{
		os << obj->value.value();
	}
	else
	{
		os << "NONE";
	}

	return os;
}

std::ostream& operator<<(std::ostream& os, const ReturnObject_ptr obj)
{
	os << "Return Object : ";

	if (obj->value.has_value())
	{
		os << obj->value.value();
	}
	else
	{
		os << "NONE";
	}

	return os;
}