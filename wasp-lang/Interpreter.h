#pragma once
#include "Module.h"
#include "Object.h"

class Environment
{
};

class Interpreter
{
	Module mod;
	Environment env;

	void evaluate_statement(StatementNode_ptr statement);
	ObjectVariant_ptr evaluate_expression();
	ObjectVariant_ptr evaluate_unary_expression();
	ObjectVariant_ptr evaluate_binary_expression();
public:
	Interpreter(Module mod) : mod(mod) {};
	void execute();
};