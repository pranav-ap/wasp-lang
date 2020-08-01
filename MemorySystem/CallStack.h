#pragma once
#include <stack>
#include "ActivationRecord.h"

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

class MEMORYSYSTEM_API CallStack
{
	std::stack<ActivationRecord_ptr> call_stack;

public:
	void push(ActivationRecord_ptr activation_record);
	ActivationRecord_ptr pop();
	ActivationRecord_ptr peek();
};