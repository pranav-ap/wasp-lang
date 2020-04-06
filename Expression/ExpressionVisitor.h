#pragma once

#ifdef EXPRESSION_EXPORTS
#define EXPRESSION_API __declspec(dllexport)
#else
#define EXPRESSION_API __declspec(dllimport)
#endif

#include "Expression.h"
#include "ObjectSystem.h"

class EXPRESSION_API ExpressionVisitor
{
public:
	virtual Object_ptr visit(StringLiteral_ptr expression) = 0;
	virtual Object_ptr visit(NumberLiteral_ptr expression) = 0;
	virtual Object_ptr visit(BooleanLiteral_ptr expression) = 0;
	virtual Object_ptr visit(VectorLiteral_ptr expression) = 0;
	virtual Object_ptr visit(UDTLiteral_ptr expression) = 0;
	virtual Object_ptr visit(Identifier_ptr expression) = 0;
	virtual Object_ptr visit(Unary_ptr expression) = 0;
	virtual Object_ptr visit(Binary_ptr expression) = 0;
	virtual Object_ptr visit(VectorMemberAccess_ptr expression) = 0;
	virtual Object_ptr visit(UDTMemberAccess_ptr expression) = 0;
	virtual Object_ptr visit(EnumMemberAccess_ptr expression) = 0;
	virtual Object_ptr visit(FunctionCall_ptr expression) = 0;
	virtual Object_ptr visit(Range_ptr expression) = 0;
};
