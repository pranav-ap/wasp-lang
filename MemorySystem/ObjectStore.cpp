#pragma once
#include "pch.h"
#include "ObjectStore.h"
#include "Assertion.h"

void ObjectStore::set(int id, Object_ptr value)
{
	ASSERT(!objects.contains(id), "ID already exists in ObjectStore");
	objects.insert({ id, value });
}

Object_ptr ObjectStore::get(int id)
{
	ASSERT(objects.contains(id), "ID does not exist in ObjectStore");
	return objects.at(id);
}