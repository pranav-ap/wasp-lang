#include "pch.h"
#include "Statement.h"
#include "StatementVisitor.h"

#define ACCEPT(name) std::shared_ptr<name> p{ shared_from_this() }

class StatementVisitor;

Object_ptr VariableDeclaration::interpret(StatementVisitor& visitor)
{
	ACCEPT(VariableDeclaration);
	return visitor.visit(p);
}

Object_ptr Assignment::interpret(StatementVisitor& visitor)
{
	ACCEPT(Assignment);
	return visitor.visit(p);
}

Object_ptr Branch::interpret(StatementVisitor& visitor)
{
	ACCEPT(Branch);
	return visitor.visit(p);
}

Object_ptr Loop::interpret(StatementVisitor& visitor)
{
	ACCEPT(Loop);
	return visitor.visit(p);
}

Object_ptr ForEachLoop::interpret(StatementVisitor& visitor)
{
	ACCEPT(ForEachLoop);
	return visitor.visit(p);
}

Object_ptr Break::interpret(StatementVisitor& visitor)
{
	ACCEPT(Break);
	return visitor.visit(p);
}

Object_ptr Continue::interpret(StatementVisitor& visitor)
{
	ACCEPT(Continue);
	return visitor.visit(p);
}

Object_ptr UDTDefinition::interpret(StatementVisitor& visitor)
{
	ACCEPT(UDTDefinition);
	return visitor.visit(p);
}

Object_ptr FunctionDefinition::interpret(StatementVisitor& visitor)
{
	ACCEPT(FunctionDefinition);
	return visitor.visit(p);
}

Object_ptr Return::interpret(StatementVisitor& visitor)
{
	ACCEPT(Return);
	return visitor.visit(p);
}

Object_ptr ExpressionStatement::interpret(StatementVisitor& visitor)
{
	ACCEPT(ExpressionStatement);
	return visitor.visit(p);
}

Object_ptr Import::interpret(StatementVisitor& visitor)
{
	ACCEPT(Import);
	return visitor.visit(p);
}

Object_ptr ImportSTD::interpret(StatementVisitor& visitor)
{
	ACCEPT(ImportSTD);
	return visitor.visit(p);
}

Object_ptr EnumDefinition::interpret(StatementVisitor& visitor)
{
	ACCEPT(EnumDefinition);
	return visitor.visit(p);
}

// Printers

MODULE_API std::ostream& operator<<(std::ostream& os, const VariableDeclaration_ptr stat)
{
	os << stat->name << " : " << stat->type; // << " " << stat->is_public << " " << stat->is_mutable;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const Assignment_ptr stat)
{
	os << stat->name << " " << stat->expression;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const Branch_ptr stat)
{
	os << stat->condition << " " << stat->consequence << " " << stat->alternative;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const Loop_ptr stat)
{
	os << "LOOP";
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const ForEachLoop_ptr stat)
{
	os << "ForEachLoop";
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const Break_ptr stat)
{
	os << "Break";
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const Continue_ptr stat)
{
	os << "Continue";
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const UDTDefinition_ptr stat)
{
	os << "Record : " << stat->name;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const FunctionDefinition_ptr stat)
{
	os << "Function Definition : " << stat->name;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const Return_ptr stat)
{
	if (stat->expression.has_value()) {
		os << stat->expression.value();
	}
	else {
		os << "None";
	}

	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const ExpressionStatement_ptr stat)
{
	os << stat->expression;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const Import_ptr stat)
{
	os << stat->path;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const ImportSTD_ptr stat)
{
	os << stat->name;
	return os;
}

MODULE_API std::ostream& operator<<(std::ostream& os, const EnumDefinition_ptr stat)
{
	os << stat->name;
	return os;
}