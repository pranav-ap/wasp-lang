#pragma once
#include "OpCode.h"
#include "ObjectStore.h"
#include "CodeObject.h"
#include <memory>

class MemorySystem
{
public:
	CodeObject_ptr code_object;
	ObjectStore_ptr object_store;

	MemorySystem();
	void print();
};

using MemorySystem_ptr = std::shared_ptr<MemorySystem>;
