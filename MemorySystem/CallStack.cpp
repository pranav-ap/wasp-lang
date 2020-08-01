#pragma once
#include "pch.h"
#include "CallStack.h"
#include "Assertion.h"
#include <memory>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")

void CallStack::push(ActivationRecord_ptr activation_record)
{
	NULL_CHECK(activation_record);
	call_stack.push(std::move(activation_record));
}

ActivationRecord_ptr CallStack::pop()
{
	auto record = call_stack.top();
	NULL_CHECK(record);
	call_stack.pop();

	return record;
}

ActivationRecord_ptr CallStack::peek()
{
	auto record = call_stack.top();
	NULL_CHECK(record);
	return record;
}