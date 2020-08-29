#pragma once

#ifdef VM_EXPORTS
#define VM_API __declspec(dllexport)
#else
#define VM_API __declspec(dllimport)
#endif

#include "MemorySystem.h"

struct Frame
{
};

class VM_API VM
{
	ConstantPool_ptr constant_pool;
	CodeSection_ptr code_section;

public:
	VM()
	{
		constant_pool = std::make_shared<ConstantPool>();
		code_section = std::make_shared<CodeSection>();
	}

	VM(MemorySystem_ptr memory)
	{
		constant_pool = memory->get_constant_pool();
		code_section = memory->get_code_section();
	}

	void execute();
};

using VM_ptr = std::shared_ptr<VM>;
