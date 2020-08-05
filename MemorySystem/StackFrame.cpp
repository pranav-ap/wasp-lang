#include "pch.h"
#include "StackFrame.h"
#include "Assertion.h"

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")

Object_ptr StackFrame::get(std::wstring name)
{
	if (store.contains(name))
	{
		auto value = store.at(name);
		NULL_CHECK(value);
		ASSERT(value->index() != 0, "Activation Record value is a monostate");
		return value;
	}

	FATAL("Activation Record does not contain name");
}

void StackFrame::set(std::wstring name, Object_ptr value)
{
	store.insert({ name, value });
}