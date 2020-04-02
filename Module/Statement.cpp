#include "pch.h"
#include "Statement.h"

#define ACCEPT(name)								\
	std::shared_ptr<name> p{ shared_from_this() };	\
	visitor.visit(p)

void VariableDeclaration::interpret(StatementInterpreter& visitor)
{
	ACCEPT(VariableDeclaration);
}

void Assignment::interpret(StatementInterpreter& visitor)
{
	ACCEPT(Assignment);
}

void Branch::interpret(StatementInterpreter& visitor)
{
	ACCEPT(Branch);
}

void Loop::interpret(StatementInterpreter& visitor)
{
	ACCEPT(Loop);
}

void Break::interpret(StatementInterpreter& visitor)
{
	ACCEPT(Break);
}

void Continue::interpret(StatementInterpreter& visitor)
{
	ACCEPT(Continue);
}

void UDTDefinition::interpret(StatementInterpreter& visitor)
{
	ACCEPT(UDTDefinition);
}

void FunctionDefinition::interpret(StatementInterpreter& visitor)
{
	ACCEPT(FunctionDefinition);
}

void Return::interpret(StatementInterpreter& visitor)
{
	ACCEPT(Return);
}

void ExpressionStatement::interpret(StatementInterpreter& visitor)
{
	ACCEPT(ExpressionStatement);
}

void Import::interpret(StatementInterpreter& visitor)
{
	ACCEPT(Import);
}

// Printers

MODULE_API std::ostream& operator<<(std::ostream& os, const VariableDeclaration_ptr stat)
{
	os << stat->name << " : " << stat->type << " " << stat->is_public << " " << stat->is_public;
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