#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

#include "Statement.h"

class MODULE_API StatementVisitor
{
public:
	virtual void visit(VariableDeclaration_ptr statement) = 0;
	virtual void visit(Assignment_ptr statement) = 0;
	virtual void visit(Branch_ptr statement) = 0;
	virtual void visit(Loop_ptr statement) = 0;
	virtual void visit(ForEachLoop_ptr statement) = 0;
	virtual void visit(Break_ptr statement) = 0;
	virtual void visit(Continue_ptr statement) = 0;
	virtual void visit(UDTDefinition_ptr statement) = 0;
	virtual void visit(FunctionDefinition_ptr statement) = 0;
	virtual void visit(Return_ptr statement) = 0;
	virtual void visit(ExpressionStatement_ptr statement) = 0;
	virtual void visit(Import_ptr statement) = 0;
	virtual void visit(ImportSTD_ptr statement) = 0;
	virtual void visit(Enum_ptr statement) = 0;
};
