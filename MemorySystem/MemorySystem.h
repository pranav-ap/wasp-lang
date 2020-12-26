#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "ObjectStore.h"
#include "CodeObject.h"
#include <memory>

class MEMORYSYSTEM_API MemorySystem
{
public:
	CodeObject_ptr code_object;
	ObjectStore_ptr object_store;

	MemorySystem();
	void print();
};

using MemorySystem_ptr = MEMORYSYSTEM_API std::shared_ptr<MemorySystem>;
