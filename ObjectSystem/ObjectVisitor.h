#pragma once

#ifdef OBJECTSYSTEM_EXPORTS
#define OBJECTSYSTEM_API __declspec(dllexport)
#else
#define OBJECTSYSTEM_API __declspec(dllimport)
#endif

#include "ObjectSystem.h"

class IObjectVisitor
{
public:
	virtual Object_ptr visit(NumberObject& object) = 0;
	virtual Object_ptr visit(StringObject object) = 0;
	virtual Object_ptr visit(BooleanObject object) = 0;
	virtual Object_ptr visit(ListObject object) = 0;
	virtual Object_ptr visit(TupleObject object) = 0;
	virtual Object_ptr visit(MapObject object) = 0;

	virtual Object_ptr visit(EnumMemberObject object) = 0;
	virtual Object_ptr visit(VariantObject object) = 0;
	virtual Object_ptr visit(ReturnObject object) = 0;
	virtual Object_ptr visit(ErrorObject object) = 0;
	virtual Object_ptr visit(BreakObject object) = 0;
	virtual Object_ptr visit(ContinueObject object) = 0;
	virtual Object_ptr visit(BuiltInsObject object) = 0;
	virtual Object_ptr visit(NoneObject object) = 0;
};