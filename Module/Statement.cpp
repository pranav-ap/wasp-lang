#include "pch.h"
#include "Statement.h"
#include "StatementVisitor.h"

#define ACCEPT(name) std::shared_ptr<name> p{ shared_from_this() }

class StatementVisitor;

ObjectVariant_ptr VariableDeclaration::interpret(StatementVisitor& visitor)
{
	ACCEPT(VariableDeclaration);
	return visitor.visit(p);
}

ObjectVariant_ptr Assignment::interpret(StatementVisitor& visitor)
{
	ACCEPT(Assignment);
	return visitor.visit(p);
}

ObjectVariant_ptr Branch::interpret(StatementVisitor& visitor)
{
	ACCEPT(Branch);
	return visitor.visit(p);
}

ObjectVariant_ptr Loop::interpret(StatementVisitor& visitor)
{
	ACCEPT(Loop);
	return visitor.visit(p);
}

ObjectVariant_ptr ForEachLoop::interpret(StatementVisitor& visitor)
{
	ACCEPT(ForEachLoop);
	return visitor.visit(p);
}

ObjectVariant_ptr Break::interpret(StatementVisitor& visitor)
{
	ACCEPT(Break);
	return visitor.visit(p);
}

ObjectVariant_ptr Continue::interpret(StatementVisitor& visitor)
{
	ACCEPT(Continue);
	return visitor.visit(p);
}

ObjectVariant_ptr UDTDefinition::interpret(StatementVisitor& visitor)
{
	ACCEPT(UDTDefinition);
	return visitor.visit(p);
}

ObjectVariant_ptr FunctionDefinition::interpret(StatementVisitor& visitor)
{
	ACCEPT(FunctionDefinition);
	return visitor.visit(p);
}

ObjectVariant_ptr Return::interpret(StatementVisitor& visitor)
{
	ACCEPT(Return);
	return visitor.visit(p);
}

ObjectVariant_ptr ExpressionStatement::interpret(StatementVisitor& visitor)
{
	ACCEPT(ExpressionStatement);
	return visitor.visit(p);
}

ObjectVariant_ptr Import::interpret(StatementVisitor& visitor)
{
	ACCEPT(Import);
	return visitor.visit(p);
}

ObjectVariant_ptr EnumDefinition::interpret(StatementVisitor& visitor)
{
	ACCEPT(EnumDefinition);
	return visitor.visit(p);
}