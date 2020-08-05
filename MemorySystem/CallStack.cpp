#pragma once
#include "pch.h"
#include "CallStack.h"
#include "StackFrame.h"
#include "Assertion.h"
#include <memory>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")

void CallStack::push(StackFrame_ptr stack_frame)
{
	NULL_CHECK(stack_frame);
	call_stack.push(std::move(stack_frame));
}

StackFrame_ptr CallStack::pop()
{
	auto record = call_stack.top();
	NULL_CHECK(record);
	call_stack.pop();

	return record;
}

StackFrame_ptr CallStack::peek()
{
	auto record = call_stack.top();
	NULL_CHECK(record);
	return record;
}

int CallStack::length()
{
	return call_stack.size();
}