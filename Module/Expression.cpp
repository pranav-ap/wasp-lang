#include "pch.h"
#include "Expression.h"
#include "ExpressionVisitor.h"
#include "ObjectSystem.h"

#define INIT_PTR_P(name) std::shared_ptr<name> p{ shared_from_this() }

// Interpret

ObjectVariant_ptr StringLiteral::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(StringLiteral);
	return visitor.visit(p);
}

ObjectVariant_ptr NumberLiteral::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(NumberLiteral);
	return visitor.visit(p);
}

ObjectVariant_ptr BooleanLiteral::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(BooleanLiteral);
	return visitor.visit(p);
}

ObjectVariant_ptr VectorLiteral::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(VectorLiteral);
	return visitor.visit(p);
}

ObjectVariant_ptr UDTLiteral::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(UDTLiteral);
	return visitor.visit(p);
}

ObjectVariant_ptr VectorMemberAccess::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(VectorMemberAccess);
	return visitor.visit(p);
}

ObjectVariant_ptr UDTMemberAccess::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(UDTMemberAccess);
	return visitor.visit(p);
}

ObjectVariant_ptr EnumMemberAccess::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(EnumMemberAccess);
	return visitor.visit(p);
}

ObjectVariant_ptr Identifier::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(Identifier);
	return visitor.visit(p);
}

ObjectVariant_ptr FunctionCall::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(FunctionCall);
	return visitor.visit(p);
}

ObjectVariant_ptr Range::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(Range);
	return visitor.visit(p);
}

ObjectVariant_ptr Unary::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(Unary);
	return visitor.visit(p);
}

ObjectVariant_ptr Binary::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(Binary);
	return visitor.visit(p);
}