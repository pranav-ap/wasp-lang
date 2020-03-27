#pragma once
#include "pch.h"
#include "Statement.h"
#include "Interpreter.h"
#include "ObjectSystem.h"

#include <memory>
#include <typeinfo>
#include <iostream>

#define STAT_TYPE_ID typeid(*statement)
#define EXPR_TYPE_ID typeid(expression)

using std::make_shared;

void Interpreter::execute()
{
	for (auto const& statement : this->mod.get_statements())
		this->evaluate_statement(statement);
}

void Interpreter::evaluate_statement(Statement_ptr statement)
{
	std::cout << "\n statement has type: " << typeid(*statement).name();

	if (STAT_TYPE_ID == typeid(VariableDeclaration))
	{
	}
	else if (STAT_TYPE_ID == typeid(Assignment))
	{
	}
	else if (STAT_TYPE_ID == typeid(Branch))
	{
	}
	else if (STAT_TYPE_ID == typeid(Loop))
	{
	}
	else if (STAT_TYPE_ID == typeid(Break))
	{
	}
	else if (STAT_TYPE_ID == typeid(Continue))
	{
	}
	else if (STAT_TYPE_ID == typeid(Alias))
	{
	}
	else if (STAT_TYPE_ID == typeid(RecordDefinition))
	{
	}
	else if (STAT_TYPE_ID == typeid(FunctionDefinition))
	{
	}
	else if (STAT_TYPE_ID == typeid(Return))
	{
	}
	else if (STAT_TYPE_ID == typeid(ExpressionStatement))
	{
	}
	else if (STAT_TYPE_ID == typeid(Import))
	{
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
		auto tuple_literal = dynamic_pointer_cast<TupleLiteral>(expression);
		auto expressions = tuple_literal->get_expressions();

		auto expression_objects = make_shared<TupleObject>();

		for (const Expression_ptr expression : expressions)
		{
			auto expression_object = this->evaluate_expression(expression);
			expression_objects->add(expression_object);
		}

		return expression_objects;
	}
	else if (EXPR_TYPE_ID == typeid(VectorLiteral))
	{
		auto vector_literal = dynamic_pointer_cast<VectorLiteral>(expression);
		auto expressions = vector_literal->get_expressions();

		auto expression_objects = make_shared<VectorObject>();

		for (const Expression_ptr expression : expressions)
		{
			auto expression_object = this->evaluate_expression(expression);
			expression_objects->add(expression_object);
		}

		return expression_objects;
	}
	else if (EXPR_TYPE_ID == typeid(MapLiteral))
	{
		auto map_literal = dynamic_pointer_cast<MapLiteral>(expression);
		auto pairs = map_literal->get_pairs();

		auto map_object = make_shared<MapObject>();

		for (auto const& [key, value] : pairs)
		{
		}

		return map_object;
	}
	else if (EXPR_TYPE_ID == typeid(RecordLiteral))
	{
		auto record_literal = dynamic_pointer_cast<RecordLiteral>(expression);
		auto pairs = record_literal->get_pairs();

		auto record_object = make_shared<RecordObject>();

		for (auto const& [key, value] : pairs)
		{
		}

		return record_object;
	}
	else if (EXPR_TYPE_ID == typeid(MemberAccess))
	{
	}
	else if (EXPR_TYPE_ID == typeid(RecordMemberAccess))
	{
	}
	else if (EXPR_TYPE_ID == typeid(Identifier))
	{
	}
	else if (EXPR_TYPE_ID == typeid(FunctionCall))
	{
	}
	else if (EXPR_TYPE_ID == typeid(InclusiveRange))
	{
	}
	else if (EXPR_TYPE_ID == typeid(ExclusiveRange))
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

// Statements

Object_ptr Interpreter::evaluate_variable_declaration()
{
	return nullptr;
}

Object_ptr Interpreter::evaluate_assignment()
{
	return nullptr;
}

Object_ptr Interpreter::evaluate_branch()
{
	return nullptr;
}

Object_ptr Interpreter::evaluate_loop()
{
	return nullptr;
}

Object_ptr Interpreter::evaluate_break()
{
	return nullptr;
}

Object_ptr Interpreter::evaluate_continue()
{
	return nullptr;
}

Object_ptr Interpreter::evaluate_alias()
{
	return nullptr;
}

Object_ptr Interpreter::evaluate_record_definition()
{
	return nullptr;
}

Object_ptr Interpreter::evaluate_function_definition()
{
	return nullptr;
}

Object_ptr Interpreter::evaluate_return()
{
	return nullptr;
}

Object_ptr Interpreter::evaluate_import()
{
	return nullptr;
}