#pragma once
#include "pch.h"
#include "logger.h"
#include "ObjectSystem.h"
#include <string>

using std::string;

void VectorObject::add(ObjectVariant_ptr value)
{
	FATAL_IF_TRUE(value->index() == 0, "Cannot add null to VectorObject");
	values.push_back(value);
}

void UDTObject::add(string key, ObjectVariant_ptr value)
{
	FATAL_IF_TRUE(value->index() == 0, "Cannot assign null to a key in UDTObject");
	pairs.insert_or_assign(key, value);
}