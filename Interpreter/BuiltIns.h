#pragma once

#ifdef INTERPRETER_EXPORTS
#define INTERPRETER_API __declspec(dllexport)
#else
#define INTERPRETER_API __declspec(dllimport)
#endif

#include "ObjectSystem.h"
#include <string>

class INTERPRETER_API Core : public BuiltInsObject
{
public:
	void echo(Object_ptr content);
	void ask(Object_ptr text, Object_ptr variable);
};
