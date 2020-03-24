#pragma once

#ifdef INTERPRETER_EXPORTS
#define INTERPRETER_API __declspec(dllexport)
#else
#define INTERPRETER_API __declspec(dllimport)
#endif

#include "Module.h"
#include "StatementNodes.h"
#include "ObjectSystem.h"
#include <stack>
#include <map>

class INTERPRETER_API Environment
{
	std::map<std::string, Object_ptr> store;
};

class INTERPRETER_API Interpreter
{
	Module mod;
	std::stack<Environment> env_stack;

	void evaluate_statement(Statement_ptr statement);

	Object_ptr evaluate_expression(ExpressionStatement expression);
	Object_ptr evaluate_unary_expression();
	Object_ptr evaluate_binary_expression();
public:
	Interpreter(Module mod) : mod(mod) {};
	void execute();
};