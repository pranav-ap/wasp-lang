#pragma once

#ifdef INTERPRETER_EXPORTS
#define INTERPRETER_API __declspec(dllexport)
#else
#define INTERPRETER_API __declspec(dllimport)
#endif

#include "Environment.h"
#include "Module.h"
#include "ObjectSystem.h"
#include "ObjectVisitor.h"
#include "Expression.h"
#include "ExpressionVisitor.h"
#include "Statement.h"
#include "StatementVisitor.h"

class INTERPRETER_API Interpreter
	: public StatementVisitor, public ExpressionVisitor
{
	Module mod;
	Environment_ptr env;

	// Statement Visitors

	Object_ptr visit(VariableDeclaration_ptr statement);
	Object_ptr visit(Assignment_ptr statement);
	Object_ptr visit(Branch_ptr statement);
	Object_ptr visit(Loop_ptr statement);
	Object_ptr visit(ForEachLoop_ptr statement);
	Object_ptr visit(Break_ptr statement);
	Object_ptr visit(Continue_ptr statement);
	Object_ptr visit(ExpressionStatement_ptr statement);
	Object_ptr visit(UDTDefinition_ptr statement);
	Object_ptr visit(FunctionDefinition_ptr statement);
	Object_ptr visit(Return_ptr statement);
	Object_ptr visit(Import_ptr statement);
	Object_ptr visit(ImportSTD_ptr statement);
	Object_ptr visit(EnumDefinition_ptr statement);

	// Expression Visitors

	Object_ptr visit(StringLiteral_ptr expression);
	Object_ptr visit(NumberLiteral_ptr expression);
	Object_ptr visit(BooleanLiteral_ptr expression);
	Object_ptr visit(VectorLiteral_ptr expression);
	Object_ptr visit(UDTLiteral_ptr expression);
	Object_ptr visit(Identifier_ptr expression);
	Object_ptr visit(Unary_ptr expression);
	Object_ptr visit(Binary_ptr expression);
	Object_ptr visit(VectorMemberAccess_ptr expression);
	Object_ptr visit(UDTMemberAccess_ptr expression);
	Object_ptr visit(EnumMemberAccess_ptr expression);
	Object_ptr visit(FunctionCall_ptr expression);
	Object_ptr visit(Range_ptr expression);

	// Utils

	Object_ptr evaluate_block(Block_ptr block);
	Object_ptr evaluate_loop(Block_ptr block);

	Object_ptr perform_operation(WTokenType token_type, Object_ptr operand);

	Object_ptr perform_operation(WTokenType token_type, NumberObject_ptr operand);
	Object_ptr perform_operation(WTokenType token_type, BooleanObject_ptr operand);

	Object_ptr perform_operation(WTokenType token_type, Object_ptr left, Object_ptr right);

	Object_ptr perform_operation(WTokenType token_type, NumberObject_ptr left, NumberObject_ptr right);
	Object_ptr perform_operation(WTokenType token_type, BooleanObject_ptr left, BooleanObject_ptr right);
	Object_ptr perform_operation(WTokenType token_type, StringObject_ptr left, StringObject_ptr right);
	Object_ptr perform_operation(WTokenType token_type, StringObject_ptr left, NumberObject_ptr right);

public:
	Interpreter(Module mod) : mod(mod), env(std::make_shared<Environment>()) {};
	void execute();
};
