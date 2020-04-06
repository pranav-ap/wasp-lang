#include "pch.h"
#include "Statement.h"
#include "StatementVisitor.h"

#define ACCEPT(name)								\
	std::shared_ptr<name> p{ shared_from_this() };	\
	visitor.visit(p)

class StatementVisitor;

void VariableDeclaration::interpret(StatementVisitor& visitor)
{
	ACCEPT(VariableDeclaration);
}

void Assignment::interpret(StatementVisitor& visitor)
{
	ACCEPT(Assignment);
}

void Branch::interpret(StatementVisitor& visitor)
{
	ACCEPT(Branch);
}

void Loop::interpret(StatementVisitor& visitor)
{
	ACCEPT(Loop);
}

void ForEachLoop::interpret(StatementVisitor& visitor)
{
	ACCEPT(ForEachLoop);
}

void Break::interpret(StatementVisitor& visitor)
{
	ACCEPT(Break);
}

void Continue::interpret(StatementVisitor& visitor)
{
	ACCEPT(Continue);
}

void UDTDefinition::interpret(StatementVisitor& visitor)
{
	ACCEPT(UDTDefinition);
}

void FunctionDefinition::interpret(StatementVisitor& visitor)
{
	ACCEPT(FunctionDefinition);
}

void Return::interpret(StatementVisitor& visitor)
{
	ACCEPT(Return);
}

void ExpressionStatement::interpret(StatementVisitor& visitor)
{
	ACCEPT(ExpressionStatement);
}

void Import::interpret(StatementVisitor& visitor)
{
	ACCEPT(Import);
}

void ImportSTD::interpret(StatementVisitor& visitor)
{
	ACCEPT(ImportSTD);
}

void Enum::interpret(StatementVisitor& visitor)
{
	ACCEPT(Enum);
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

MODULE_API std::ostream& operator<<(std::ostream& os, const Enum_ptr stat)
{
	os << stat->name;
	return os;
}