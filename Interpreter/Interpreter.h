#pragma once

#ifdef INTERPRETER_EXPORTS
#define INTERPRETER_API __declspec(dllexport)
#else
#define INTERPRETER_API __declspec(dllimport)
#endif

#include "Environment.h"
#include "Module.h"
#include "Statement.h"
#include "Expression.h"
#include "ObjectSystem.h"

class INTERPRETER_API Interpreter : public StatementInterpreter, public ExpressionInterpreter
{
	Module mod;
	Environment_ptr env;

	// Statement Visitors

	void visit(VariableDeclaration_ptr statement);
	void visit(Assignment_ptr statement);
	void visit(Branch_ptr statement);
	void visit(Loop_ptr statement);
	void visit(Break_ptr statement);
	void visit(Continue_ptr statement);
	void visit(ExpressionStatement_ptr statement);

	// 0.2

	void visit(Alias_ptr statement);
	void visit(UDTDefinition_ptr statement);
	void visit(FunctionDefinition_ptr statement);
	void visit(Return_ptr statement);
	void visit(Import_ptr statement);

	// Expression Visitors

	Object_ptr visit(StringLiteral_ptr expression);
	Object_ptr visit(NumberLiteral_ptr expression);
	Object_ptr visit(BooleanLiteral_ptr expression);

	Object_ptr visit(VectorLiteral_ptr expression);
	Object_ptr visit(MapLiteral_ptr expression);
	Object_ptr visit(UDTLiteral_ptr expression);

	Object_ptr visit(Identifier_ptr expression);

	Object_ptr visit(Unary_ptr expression);
	Object_ptr visit(Binary_ptr expression);

	// 0.2

	Object_ptr visit(VectorMemberAccess_ptr expression);
	Object_ptr visit(UDTMemberAccess_ptr expression);
	Object_ptr visit(FunctionCall_ptr expression);

	Object_ptr visit(Range_ptr expression);

	// Utils

	void evaluate_branch_block(Block_ptr block);

	Object_ptr perform_unary_operation(WTokenType token_type, NumberObject_ptr operand);
	Object_ptr perform_unary_operation(WTokenType token_type, BooleanObject_ptr operand);

	Object_ptr perform_binary_operation(WTokenType token_type, NumberObject_ptr left, NumberObject_ptr right);
	Object_ptr perform_binary_operation(WTokenType token_type, BooleanObject_ptr left, BooleanObject_ptr right);

public:
	Interpreter(Module mod) : mod(mod), env(std::make_shared<Environment>()) {};
	void execute();
};
