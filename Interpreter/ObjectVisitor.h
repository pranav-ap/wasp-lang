#pragma once
#include "ObjectSystem.h"

class ObjectVisitor
{
public:
	virtual void visit(OptionalObject_ptr object) = 0;

	virtual void visit(NumberObject_ptr object) = 0;
	virtual void visit(StringObject_ptr object) = 0;
	virtual void visit(BooleanObject_ptr object) = 0;

	virtual void visit(VectorObject_ptr object) = 0;
	virtual void visit(UDTObject_ptr object) = 0;
};