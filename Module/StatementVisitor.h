#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

#include "Statement.h"
#include "ObjectSystem.h"

class MODULE_API StatementVisitor
{
public:
	virtual ObjectVariant_ptr visit(VariableDeclaration_ptr statement) = 0;
	virtual ObjectVariant_ptr visit(Assignment_ptr statement) = 0;
	virtual ObjectVariant_ptr visit(Branch_ptr statement) = 0;
	virtual ObjectVariant_ptr visit(Loop_ptr statement) = 0;
	virtual ObjectVariant_ptr visit(ForEachLoop_ptr statement) = 0;
	virtual ObjectVariant_ptr visit(Break_ptr statement) = 0;
	virtual ObjectVariant_ptr visit(Continue_ptr statement) = 0;
	virtual ObjectVariant_ptr visit(UDTDefinition_ptr statement) = 0;
	virtual ObjectVariant_ptr visit(FunctionDefinition_ptr statement) = 0;
	virtual ObjectVariant_ptr visit(Return_ptr statement) = 0;
	virtual ObjectVariant_ptr visit(ExpressionStatement_ptr statement) = 0;
	virtual ObjectVariant_ptr visit(Import_ptr statement) = 0;
	virtual ObjectVariant_ptr visit(ImportSTD_ptr statement) = 0;
	virtual ObjectVariant_ptr visit(EnumDefinition_ptr statement) = 0;
};
