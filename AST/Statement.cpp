#include "pch.h"
#include "Statement.h"

void File::add_statement(Statement_ptr statement)
{
	statements.push_back(statement);
}