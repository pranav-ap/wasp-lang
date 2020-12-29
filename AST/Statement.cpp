#include "pch.h"
#include "Statement.h"

void Module::add_statement(Statement_ptr statement)
{
	statements.push_back(statement);
}