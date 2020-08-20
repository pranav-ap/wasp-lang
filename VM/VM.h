#pragma once

#ifdef VM_EXPORTS
#define VM_API __declspec(dllexport)
#else
#define VM_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "Bytecode.h"
#include "ObjectSystem.h"

struct Frame
{
};

class VM
{
public:
	void execute();
};