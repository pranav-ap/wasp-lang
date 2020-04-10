#pragma once
#include "pch.h"
#include "ObjectSystem.h"
#include "spdlog.h"

#include <string>

using std::string;

void VectorObject::add(ObjectVariant_ptr value)
{
	if (value->index() == 0)
	{
		spdlog::error("Cannot add monostate to VectorObject");
		exit(1);
	}

	values.push_back(value);
}

void UDTObject::add(string key, ObjectVariant_ptr value)
{
	if (value->index() == 0)
	{
		spdlog::error("Cannot assign monostate to a key in UDTObject");
		exit(1);
	}

	pairs.insert_or_assign(key, value);
}