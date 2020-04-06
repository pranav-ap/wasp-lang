#include "pch.h"
#include "Expression.h"
#include "ExpressionVisitor.h"
#include "ObjectSystem.h"

#define INIT_PTR_P(name) std::shared_ptr<name> p{ shared_from_this() }

// Interpret

Object_ptr StringLiteral::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(StringLiteral);
	return visitor.visit(p);
}

Object_ptr NumberLiteral::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(NumberLiteral);
	return visitor.visit(p);
}

Object_ptr BooleanLiteral::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(BooleanLiteral);
	return visitor.visit(p);
}

Object_ptr VectorLiteral::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(VectorLiteral);
	return visitor.visit(p);
}

Object_ptr UDTLiteral::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(UDTLiteral);
	return visitor.visit(p);
}

Object_ptr VectorMemberAccess::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(VectorMemberAccess);
	return visitor.visit(p);
}

Object_ptr UDTMemberAccess::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(UDTMemberAccess);
	return visitor.visit(p);
}

Object_ptr EnumMemberAccess::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(EnumMemberAccess);
	return visitor.visit(p);
}

Object_ptr Identifier::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(Identifier);
	return visitor.visit(p);
}

Object_ptr FunctionCall::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(FunctionCall);
	return visitor.visit(p);
}

Object_ptr Range::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(Range);
	return visitor.visit(p);
}

Object_ptr Unary::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(Unary);
	return visitor.visit(p);
}

Object_ptr Binary::interpret(ExpressionVisitor& visitor)
{
	INIT_PTR_P(Binary);
	return visitor.visit(p);
}

// Printers

MODULE_API std::ostream& operator<<(std::ostream& os, const StringLiteral_ptr exp)
{
	os << " " << exp->value << " ";
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const NumberLiteral_ptr exp)
{
	os << " " << exp->value << " ";
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const BooleanLiteral_ptr exp)
{
	os << " " << exp->value << " ";
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const VectorLiteral_ptr exp)
{
	os << " [ ";

	for (auto const& e : exp->expressions)
	{
		os << e << " ";
	}

	os << " ] ";

	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const UDTLiteral_ptr exp)
{
	for (auto const& [key, value] : exp->pairs)
	{
		os << key << " " << value << " ";
	}

	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const Identifier_ptr exp)
{
	os << exp->name;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const Unary_ptr exp)
{
	os << exp->op << exp->operand;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const Binary_ptr exp)
{
	os << exp->left << " " << exp->op << " " << exp->right;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const VectorMemberAccess_ptr exp)
{
	os << exp->name << "[" << exp->expression << "]";
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const UDTMemberAccess_ptr exp)
{
	os << exp->UDT_name << "." << exp->member_name;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const EnumMemberAccess_ptr exp)
{
	os << exp->enum_name << "::" << exp->member_name;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const FunctionCall_ptr exp)
{
	os << "Call" << exp->name;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const Range_ptr exp)
{
	auto op = exp->is_inclusive ? " ... " : "..";
	os << exp->left << op << exp->right;

	return os;
}