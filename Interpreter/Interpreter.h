#pragma once

#ifdef INTERPRETER_EXPORTS
#define INTERPRETER_API __declspec(dllexport)
#else
#define INTERPRETER_API __declspec(dllimport)
#endif

#include "Environment.h"
#include "Module.h"
#include "ObjectSystem.h"
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

	ObjectVariant_ptr visit(VariableDeclaration_ptr statement);
	ObjectVariant_ptr visit(Assignment_ptr statement);
	ObjectVariant_ptr visit(Branch_ptr statement);
	ObjectVariant_ptr visit(Loop_ptr statement);
	ObjectVariant_ptr visit(ForEachLoop_ptr statement);
	ObjectVariant_ptr visit(Break_ptr statement);
	ObjectVariant_ptr visit(Continue_ptr statement);
	ObjectVariant_ptr visit(ExpressionStatement_ptr statement);
	ObjectVariant_ptr visit(UDTDefinition_ptr statement);
	ObjectVariant_ptr visit(FunctionDefinition_ptr statement);
	ObjectVariant_ptr visit(Return_ptr statement);
	ObjectVariant_ptr visit(Import_ptr statement);
	ObjectVariant_ptr visit(ImportSTD_ptr statement);
	ObjectVariant_ptr visit(EnumDefinition_ptr statement);

	// Expression Visitors

	ObjectVariant_ptr visit(StringLiteral_ptr string_literal);
	ObjectVariant_ptr visit(NumberLiteral_ptr number_literal);
	ObjectVariant_ptr visit(BooleanLiteral_ptr bool_literal);
	ObjectVariant_ptr visit(VectorLiteral_ptr expression);
	ObjectVariant_ptr visit(UDTLiteral_ptr expression);
	ObjectVariant_ptr visit(Identifier_ptr expression);
	ObjectVariant_ptr visit(Unary_ptr expression);
	ObjectVariant_ptr visit(Binary_ptr expression);
	ObjectVariant_ptr visit(VectorMemberAccess_ptr expression);
	ObjectVariant_ptr visit(UDTMemberAccess_ptr expression);
	ObjectVariant_ptr visit(EnumMemberAccess_ptr expression);
	ObjectVariant_ptr visit(FunctionCall_ptr expression);
	ObjectVariant_ptr visit(Range_ptr expression);

	// Perform Operation

	ObjectVariant_ptr perform_operation(WTokenType token_type, double operand);
	ObjectVariant_ptr perform_operation(WTokenType token_type, bool operand);

	ObjectVariant_ptr perform_operation(WTokenType token_type, double left, double right);
	ObjectVariant_ptr perform_operation(WTokenType token_type, bool left, bool right);
	ObjectVariant_ptr perform_operation(WTokenType token_type, std::string left, std::string right);
	ObjectVariant_ptr perform_operation(WTokenType token_type, std::string left, double right);

	// Utils

	ObjectVariant_ptr evaluate_block(Block_ptr block);
	bool are_same_type(ObjectVariant_ptr obj, Type_ptr type);

public:
	Interpreter(Module mod) : mod(mod), env(std::make_shared<Environment>()) {};
	void execute();
};
