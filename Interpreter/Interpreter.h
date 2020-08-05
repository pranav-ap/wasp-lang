#pragma once

#ifdef INTERPRETER_EXPORTS
#define INTERPRETER_API __declspec(dllexport)
#else
#define INTERPRETER_API __declspec(dllimport)
#endif

#include "CallStack.h"
#include "StackFrame.h"
#include "ObjectSystem.h"
#include "ObjectVisitor.h"
#include "Expression.h"
#include "Statement.h"
#include "Token.h"

class INTERPRETER_API Interpreter : IObjectVisitor
{
	CallStack_ptr call_stack;

	Object_ptr interpret(Statement_ptr statement);

	Object_ptr interpret(Assignment& statement);
	Object_ptr interpret(Branching& statement);
	Object_ptr interpret(WhileLoop& statement);
	Object_ptr interpret(ForInLoop& statement);
	Object_ptr interpret(Break& statement);
	Object_ptr interpret(Continue& statement);
	Object_ptr interpret(Pass& statement);
	Object_ptr interpret(Return& statement);
	Object_ptr interpret(YieldStatement& statement);
	Object_ptr interpret(VariableDefinition& statement);
	Object_ptr interpret(UDTDefinition& statement);
	Object_ptr interpret(AliasDefinition& statement);
	Object_ptr interpret(FunctionDefinition& statement);
	Object_ptr interpret(GeneratorDefinition& statement);
	Object_ptr interpret(EnumDefinition& statement);
	Object_ptr interpret(ImportCustom& statement);
	Object_ptr interpret(ImportInBuilt& statement);
	Object_ptr interpret(ExpressionStatement& statement);
	Object_ptr interpret(AssertStatement& statement);

	Object_ptr interpret(Expression_ptr expr);

	Object_ptr interpret(double expr);
	Object_ptr interpret(std::wstring expr);
	Object_ptr interpret(bool expr);
	Object_ptr interpret(ListLiteral& expr);
	Object_ptr interpret(TupleLiteral& expr);
	Object_ptr interpret(MapLiteral& expr);
	Object_ptr interpret(UDTConstruct& expr);
	Object_ptr interpret(UDTMemberAccess& expr);
	Object_ptr interpret(EnumMember& expr);
	Object_ptr interpret(Identifier& expr);
	Object_ptr interpret(Call& expr);
	Object_ptr interpret(Unary& expr);
	Object_ptr interpret(Binary& expr);

	// Perform Operation

	Object_ptr perform_operation(WTokenType token_type, double operand);
	Object_ptr perform_operation(WTokenType token_type, bool operand);

	Object_ptr perform_operation(WTokenType token_type, double left, double right);
	Object_ptr perform_operation(WTokenType token_type, bool left, bool right);
	Object_ptr perform_operation(WTokenType token_type, std::wstring& left, std::wstring& right);
	Object_ptr perform_operation(WTokenType token_type, std::wstring& left, double right);

	// Utils

	Object_ptr evaluate_block(ScopedBlock& block);

	Object_ptr loop_over_iterable(std::wstring item_name, ScopedBlock& block, ListObject& vector_object);
	Object_ptr loop_over_iterable(std::wstring pair_name, ScopedBlock& block, MapLiteral& map_object);

	Object_ptr evaluate_function_call(Call& call_expression);

public:
	Interpreter() {};
	void execute(Module_ptr ast);
};
