#include "Interpreter.h"

void Interpreter::evaluate_statement(StatementNode_ptr statement)
{
}

void Interpreter::execute()
{
	for (auto const& statement : this->mod.get_nodes())
		this->evaluate_statement(statement);
}