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
#include "Statement.h"

class INTERPRETER_API Interpreter
{
	Environment_ptr env;

	Object_ptr interpret(Statement_ptr statement);
	Object_ptr interpret(Expression_ptr expression);

	// Statement Interpreters

	Object_ptr interpret(Assignment statement);
	Object_ptr interpret(MultipleAssignment statement);

	Object_ptr interpret(ConditionalBranch statement);
	Object_ptr interpret(IfLetBranch statement);

	Object_ptr interpret(InfiniteLoop statement);
	Object_ptr interpret(ForEachLoop statement);

	Object_ptr interpret(Break statement);
	Object_ptr interpret(Continue statement);

	Object_ptr interpret(VariableDefinition statement);
	Object_ptr interpret(UDTDefinition statement);
	Object_ptr interpret(FunctionDefinition statement);
	Object_ptr interpret(EnumDefinition statement);

	Object_ptr interpret(ExpressionStatement statement);
	Object_ptr interpret(Return statement);

	Object_ptr interpret(ImportCustom statement);
	Object_ptr interpret(ImportInBuilt statement);

	// Expression Interpreters

	Object_ptr interpret(std::string string_literal);
	Object_ptr interpret(double number_literal);
	Object_ptr interpret(bool bool_literal);

	Object_ptr interpret(VectorLiteral expression);
	Object_ptr interpret(DictionaryLiteral expression);

	Object_ptr interpret(Unary expression);
	Object_ptr interpret(Binary expression);

	Object_ptr interpret(Identifier expression);
	Object_ptr interpret(EnumMember expression);

	Object_ptr interpret(MemberAccess expression);
	Object_ptr interpret(FunctionCall expression);

	// Perform Operation

	Object_ptr perform_operation(WTokenType token_type, double operand);
	Object_ptr perform_operation(WTokenType token_type, bool operand);

	Object_ptr perform_operation(WTokenType token_type, double left, double right);
	Object_ptr perform_operation(WTokenType token_type, bool left, bool right);
	Object_ptr perform_operation(WTokenType token_type, std::string& left, std::string& right);
	Object_ptr perform_operation(WTokenType token_type, std::string& left, double right);

	// Utils

	Object_ptr evaluate_block(Block block);

	Object_ptr loop_over_iterable(std::string item_name, Block block, ListObject& vector_object);
	Object_ptr loop_over_iterable(std::string pair_name, Block block, DictionaryObject& map_object);

	Object_ptr evaluate_function_call(FunctionCall call_expression, FunctionInfo* info, std::vector<Object_ptr> formal_arguments);
	Object_ptr evaluate_function_call(FunctionCall call_expression, InBuiltFunctionInfo* info);

	bool are_same_type(Object_ptr obj, Type_ptr type);
	Type_ptr get_object_type(Object_ptr object);

public:
	Interpreter(Environment_ptr env) : env(env) {};
	void execute(Module mod);
};
