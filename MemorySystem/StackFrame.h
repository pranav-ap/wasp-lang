#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include "ObjectSystem.h"
#include <memory>
#include <string>

enum class MEMORYSYSTEM_API StackFrameType
{
	PROGRAM,
	FUNCTION,
	GENERATOR
};

class MEMORYSYSTEM_API StackFrame
{
	std::map<std::wstring, Object_ptr> store;
public:
	std::wstring name;
	StackFrameType type;

	StackFrame() : name(L"Program"), type(StackFrameType::PROGRAM) {};
	StackFrame(std::wstring name, StackFrameType type) : name(name), type(type) {};;

	Object_ptr get(std::wstring name);
	void set(std::wstring name, Object_ptr value);
};

using StackFrame_ptr = MEMORYSYSTEM_API std::shared_ptr<StackFrame>;
