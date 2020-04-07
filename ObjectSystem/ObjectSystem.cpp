#pragma once
#include "pch.h"
#include "logger.h"
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
	FATAL_IF_NULLPTR(value, "Cannot add nullptr to VectorObject");
	values.push_back(value);
}

void UDTObject::add(std::string key, Object_ptr value)
{
	FATAL_IF_NULLPTR(value, "Cannot assign nullptr to a key in UDTObject");
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
}

void VoidObject::accept(ObjectVisitor& visitor)
{
}

void BreakObject::accept(ObjectVisitor& visitor)
{
}

void ContinueObject::accept(ObjectVisitor& visitor)
{
}