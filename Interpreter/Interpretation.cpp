#pragma once
#include "pch.h"
#include "Statement.h"
#include "Module.h"
#include "Environment.h"
#include "Interpreter.h"

#include <memory>

using std::make_shared;

Interpreter::Interpreter(Module mod) : mod(mod)
{
	env_list.push_back(make_shared<Environment>());
};

void Interpreter::execute()
{
	for (auto const& statement : this->mod.get_statements())
		this->evaluate_statement(statement);
}