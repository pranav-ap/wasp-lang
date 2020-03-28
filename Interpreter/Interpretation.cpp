#pragma once
#include "pch.h"
#include "Statement.h"
#include "Module.h"
#include "ObjectSystem.h"
#include "Interpreter.h"

#include <memory>
#include <typeinfo>
#include <iostream>
#include <string>

#define STAT_TYPE_ID typeid(*statement)
#define EXPR_TYPE_ID typeid(expression)

using std::make_shared;
using std::shared_ptr;
using std::string;

void Interpreter::execute()
{
	for (auto const& statement : this->mod.get_statements())
		this->evaluate_statement(statement);
}

void Interpreter::evaluate_statement(Statement_ptr statement)
{
	std::cout << "\n Statement has type: " << typeid(*statement).name();

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
	else if (STAT_TYPE_ID == typeid(RecordDefinition))
	{
		this->store_UDT(statement);
	}
	else if (STAT_TYPE_ID == typeid(FunctionDefinition))
	{
		this->store_function(statement);
	}
	else if (STAT_TYPE_ID == typeid(ExpressionStatement))
	{
		auto expr = dynamic_pointer_cast<ExpressionStatement>(statement);
		auto result = this->evaluate_expression(expr->get_expression());
		// Print result?
	}
}

// Expressions

Object_ptr Interpreter::evaluate_expression(Expression_ptr expression)
{
	if (EXPR_TYPE_ID == typeid(StringLiteral))
	{
		auto string_literal = dynamic_pointer_cast<StringLiteral>(expression);
		return make_shared<StringObject>(string_literal->get_value());
	}
	else if (EXPR_TYPE_ID == typeid(NumberLiteral))
	{
		auto number_literal = dynamic_pointer_cast<NumberLiteral>(expression);
		return make_shared<NumberObject>(number_literal->get_value());
	}
	else if (EXPR_TYPE_ID == typeid(BooleanLiteral))
	{
		auto boolean_literal = dynamic_pointer_cast<BooleanLiteral>(expression);
		return make_shared<BooleanObject>(boolean_literal->get_value());
	}
	else if (EXPR_TYPE_ID == typeid(TupleLiteral))
	{
		return this->to_tuple_object(expression);
	}
	else if (EXPR_TYPE_ID == typeid(VectorLiteral))
	{
		return this->to_vector_object(expression);
	}
	else if (EXPR_TYPE_ID == typeid(MapLiteral))
	{
		return this->to_map_object(expression);
	}
	else if (EXPR_TYPE_ID == typeid(RecordLiteral))
	{
		return this->to_record_object(expression);
	}
	else if (EXPR_TYPE_ID == typeid(MemberAccess))
	{
		auto access_expr = dynamic_pointer_cast<MemberAccess>(expression);

		auto container_name = access_expr->get_container_name();
		auto container = this->get_variable(container_name);

		auto index_expression = access_expr->get_index_expression();
		auto result = this->evaluate_expression(index_expression);

		// ???
	}
	else if (EXPR_TYPE_ID == typeid(RecordMemberAccess))
	{
		// ???
	}
	else if (EXPR_TYPE_ID == typeid(Identifier))
	{
		auto identifier = dynamic_pointer_cast<Identifier>(expression);
		auto info = this->get_variable(identifier->get_name());
		return info->value;
	}
	else if (EXPR_TYPE_ID == typeid(FunctionCall))
	{
	}
	else if (EXPR_TYPE_ID == typeid(Unary))
	{
	}
	else if (EXPR_TYPE_ID == typeid(Binary))
	{
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_unary_expression(Expression_ptr expression)
{
	return nullptr;
}

Object_ptr Interpreter::evaluate_binary_expression(Expression_ptr expression)
{
	return nullptr;
}

Object_ptr Interpreter::evaluate_function_call()
{
	return nullptr;
}

// Statements

void Interpreter::create_variable(Statement_ptr statement)
{
	auto s = dynamic_pointer_cast<VariableDeclaration>(statement);

	string name = s->get_variable_name();

	bool is_public = s->is_public_declaration();
	bool is_mutable = s->is_mutable_declaration();
	Type_ptr type = s->get_type();

	Expression_ptr expression = s->get_expression();
	Object_ptr expression_result = this->evaluate_expression(expression);

	this->set_variable(
		name,
		make_shared<VariableInfo>(is_public, is_mutable, type, expression_result)
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
}

void Interpreter::evaluate_loop(Statement_ptr statement)
{
	if (STAT_TYPE_ID == typeid(Break))
	{
	}
	else if (STAT_TYPE_ID == typeid(Continue))
	{
	}
}

void Interpreter::store_UDT(Statement_ptr statement)
{
}

void Interpreter::store_function(Statement_ptr statement)
{
	if (STAT_TYPE_ID == typeid(Return))
	{
	}
}

Object_ptr Interpreter::evaluate_return(Statement_ptr statement)
{
	return nullptr;
}