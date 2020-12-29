#pragma once
#include "pch.h"
#include "MemorySystem.h"

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)

using std::byte;
using std::vector;
using std::move;
using std::wstring;
using std::to_wstring;
using std::to_string;
using std::make_shared;

MemorySystem::MemorySystem()
{
	object_store = std::make_shared<ObjectStore>();
	code_object = std::make_shared<CodeObject>();
}

void MemorySystem::print()
{

}
