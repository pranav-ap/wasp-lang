#pragma once
#include "OpCode.h"
#include "ObjectStore.h"
#include "CodeObject.h"
#include "MemorySystem.h"
#include <memory>

class VM
{
	MemorySystem_ptr memory;

public:
	VM() :
		memory(std::shared_ptr<MemorySystem>()) {};
};

using VM_ptr = std::shared_ptr<VM>;
