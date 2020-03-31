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

void Alias::interpret(StatementInterpreter& visitor)
{
	ACCEPT(Alias);
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