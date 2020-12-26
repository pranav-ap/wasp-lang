#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#include "SymbolScope.h"

struct AST_API AnnotatedNode
{
	SymbolScope_ptr scope;
	AnnotatedNode() : scope(nullptr) {};
};
