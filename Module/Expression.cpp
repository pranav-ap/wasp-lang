#include "pch.h"
#include "Expression.h"
#include "ObjectSystem.h"

#define INIT_PTR_P(name) std::shared_ptr<name> p{ shared_from_this() }

Object_ptr StringLiteral::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(StringLiteral);
	return visitor.visit(p);
}

Object_ptr NumberLiteral::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(NumberLiteral);
	return visitor.visit(p);
}

Object_ptr BooleanLiteral::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(BooleanLiteral);
	return visitor.visit(p);
}

Object_ptr VectorLiteral::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(VectorLiteral);
	return visitor.visit(p);
}

Object_ptr MapLiteral::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(MapLiteral);
	return visitor.visit(p);
}

Object_ptr UDTLiteral::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(UDTLiteral);
	return visitor.visit(p);
}

Object_ptr MemberAccess::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(MemberAccess);
	return visitor.visit(p);
}

Object_ptr RecordMemberAccess::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(RecordMemberAccess);
	return visitor.visit(p);
}

Object_ptr Identifier::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(Identifier);
	return visitor.visit(p);
}

Object_ptr FunctionCall::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(FunctionCall);
	return visitor.visit(p);
}

Object_ptr InclusiveRange::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(InclusiveRange);
	return visitor.visit(p);
}

Object_ptr ExclusiveRange::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(ExclusiveRange);
	return visitor.visit(p);
}

Object_ptr Unary::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(Unary);
	return visitor.visit(p);
}

Object_ptr Binary::interpret(ExpressionInterpreter& visitor)
{
	INIT_PTR_P(Binary);
	return visitor.visit(p);
}