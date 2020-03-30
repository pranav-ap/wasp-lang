#pragma once
#include "pch.h"
#include "Statement.h"
#include "TokenType.h"
#include "ObjectSystem.h"
#include "Interpreter.h"

#include <memory>
#include <typeinfo>
#include <string>

using std::make_shared;
using std::string;

void Interpreter::evaluate_statement(Statement_ptr statement)
{
	if (STAT_TYPE_ID == typeid(VariableDeclaration))
	{
		this->create_variable(statement);
	}
	else if (STAT_TYPE_ID == typeid(Assignment))
	{
		this->update_variable(statement);
	}
	else if (STAT_TYPE_ID == typeid(Branch))
	{
		this->evaluate_branch(statement);
	}
	else if (STAT_TYPE_ID == typeid(Loop))
	{
		this->evaluate_loop(statement);
	}
	else if (STAT_TYPE_ID == typeid(ExpressionStatement))
	{
		auto expr = dynamic_pointer_cast<ExpressionStatement>(statement);
		auto result = this->evaluate_expression(expr->get_expression());
		// Print result?
	}
}

void Interpreter::create_variable(Statement_ptr statement)
{
	auto declaration = dynamic_pointer_cast<VariableDeclaration>(statement);

	string name = declaration->get_variable_name();

	bool is_public = declaration->is_public_declaration();
	bool is_mutable = declaration->is_mutable_declaration();
	Type_ptr type = declaration->get_type();

	Expression_ptr expression = declaration->get_expression();
	Object_ptr result_object = this->evaluate_expression(expression);

	this->create_and_set_variable(
		name,
		make_shared<VariableInfo>(is_public, is_mutable, type, result_object)
	);
}

void Interpreter::update_variable(Statement_ptr statement)
{
	auto assignment = dynamic_pointer_cast<Assignment>(statement);

	auto name = assignment->get_variable_name();
	auto info = this->get_variable(name);

	if (!info->is_mutable)
	{
		return;
	}

	auto expression = assignment->get_expression();
	auto result = this->evaluate_expression(expression);

	info->value = result;
}

void Interpreter::evaluate_branch(Statement_ptr statement)
{
	auto branch = dynamic_pointer_cast<Branch>(statement);

	Expression_ptr condition = branch->get_condition();
	auto result = this->evaluate_expression(condition);

	if (typeid(*result) != typeid(BooleanObject))
	{
		return;
	}

	auto result_boolean_object = dynamic_pointer_cast<BooleanObject>(result);

	if (result_boolean_object->value)
	{
		Block_ptr consequence = branch->get_consequence();
		this->evaluate_block(consequence);
	}
	else
	{
		Block_ptr alternative = branch->get_alternative();
		this->evaluate_block(alternative);
	}
}

void Interpreter::evaluate_loop(Statement_ptr statement)
{
	auto loop = dynamic_pointer_cast<Loop>(statement);
	Block_ptr block = loop->get_block();

	bool must_continue = false;

	do
	{
		for (auto const& statement : *block)
		{
			if (STAT_TYPE_ID == typeid(Break))
			{
				break;
			}
			else if (STAT_TYPE_ID == typeid(Continue))
			{
				must_continue = true;
				break;
			}

			this->evaluate_statement(statement);
		}
	} while (must_continue);
}

void Interpreter::evaluate_block(Block_ptr block)
{
	for (auto const& statement : *block)
	{
		this->evaluate_statement(statement);
	}
}