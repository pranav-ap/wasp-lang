#pragma once
#include "Objects.h"
#include <memory>

struct CallFrame
{
	int ip;
	int base_pointer;
	CodeObject_ptr code_object;

	CallFrame(CodeObject_ptr code_object, int base_pointer)
		: code_object(std::move(code_object)), ip(base_pointer), base_pointer(base_pointer) {};
};

using CallFrame_ptr = std::shared_ptr<CallFrame>;
