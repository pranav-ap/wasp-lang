#pragma once

#ifdef OBJECTSYSTEM_EXPORTS
#define OBJECTSYSTEM_API __declspec(dllexport)
#else
#define OBJECTSYSTEM_API __declspec(dllimport)
#endif

#include "ObjectSystem.h"

class OBJECTSYSTEM_API ObjectVisitor
{
public:
	virtual void visit(NumberObject_ptr obj) = 0;
	virtual void visit(StringObject_ptr obj) = 0;
	virtual void visit(BooleanObject_ptr obj) = 0;
	virtual void visit(VectorObject_ptr obj) = 0;
	virtual void visit(UDTObject_ptr obj) = 0;
	virtual void visit(OptionalObject_ptr obj) = 0;
};
