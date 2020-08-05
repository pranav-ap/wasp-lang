#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include <stack>
#include "StackFrame.h"

class MEMORYSYSTEM_API CallStack
{
	std::stack<StackFrame_ptr> call_stack;

public:
	void push(StackFrame_ptr stack_frame);
	StackFrame_ptr pop();
	StackFrame_ptr peek();
	int length();
};

using CallStack_ptr = MEMORYSYSTEM_API std::shared_ptr<CallStack>;
