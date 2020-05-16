#pragma once

#include "Environment.h"
#include "Module.h"
#include "ObjectSystem.h"
#include "Expression.h"
#include "Statement.h"

class Evaluator
{
	Environment_ptr env;

	Object_ptr evaluate(Statement_ptr statement);
	Object_ptr evaluate(Expression_ptr expression);

	// Statement Evaluators

	Object_ptr evaluate(Assignment statement);

	Object_ptr evaluate(Branching statement);

	Object_ptr evaluate(WhileLoop statement);
	Object_ptr evaluate(ForInLoop statement);

	Object_ptr evaluate(Break statement);
	Object_ptr evaluate(Continue statement);
	Object_ptr evaluate(Pass statement);
	Object_ptr evaluate(Return statement);
	Object_ptr evaluate(YieldStatement statement);

	Object_ptr evaluate(VariableDefinition statement);
	Object_ptr evaluate(UDTDefinition statement);
	Object_ptr evaluate(AliasDefinition statement);
	Object_ptr evaluate(FunctionDefinition statement);
	Object_ptr evaluate(GeneratorDefinition statement);
	Object_ptr evaluate(EnumDefinition statement);

	Object_ptr evaluate(ExpressionStatement statement);
	Object_ptr evaluate(AssertStatement statement);

	Object_ptr evaluate(ImportCustom statement);
	Object_ptr evaluate(ImportInBuilt statement);

	// Expression Evaluators

	Object_ptr evaluate(std::string string_literal);
	Object_ptr evaluate(double number_literal);
	Object_ptr evaluate(bool bool_literal);

	Object_ptr evaluate(ListLiteral expression);
	Object_ptr evaluate(TupleLiteral expression);
	Object_ptr evaluate(MapLiteral expression);
	Object_ptr evaluate(UDTLiteral expression);

	Object_ptr evaluate(MemberAccess expression);
	Object_ptr evaluate(EnumMember expression);

	Object_ptr evaluate(Identifier expression);
	Object_ptr evaluate(Call expression);

	Object_ptr evaluate(Unary expression);
	Object_ptr evaluate(Binary expression);

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
	Object_ptr loop_over_iterable(std::string pair_name, Block block, MapObject& map_object);

	Object_ptr evaluate_function_call(Call call_expression, FunctionInfo* info, std::vector<Object_ptr> formal_arguments);
	Object_ptr evaluate_function_call(Call call_expression, InBuiltFunctionInfo* info);

public:
	Evaluator(Environment_ptr global_env) : env(global_env) {};
	void execute(Module mod);
};
