#pragma once
#include "pch.h"
#include "Token.h"
#include "Statement.h"
#include "Expression.h"
#include "Module.h"
#include "TokenType.h"
#include "ObjectSystem.h"
#include "Interpreter.h"

#include <memory>
#include <typeinfo>
#include <iostream>
#include <string>
#include <math.h>

#define STAT_TYPE_ID typeid(*statement)
#define EXPR_TYPE_ID typeid(expression)
#define OPERAND_TYPE_ID typeid(operand)
#define LEFT_TYPE_ID typeid(left)
#define RIGHT_TYPE_ID typeid(right)

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
		//this->store_UDT(statement);
	}
	else if (STAT_TYPE_ID == typeid(FunctionDefinition))
	{
		//this->store_function(statement);
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
		return convert_to_string_object(expression);
	}
	else if (EXPR_TYPE_ID == typeid(NumberLiteral))
	{
		return convert_to_number_object(expression);
	}
	else if (EXPR_TYPE_ID == typeid(BooleanLiteral))
	{
		return convert_to_boolean_object(expression);
	}
	else if (EXPR_TYPE_ID == typeid(TupleLiteral))
	{
		return this->convert_to_tuple_object(expression);
	}
	else if (EXPR_TYPE_ID == typeid(VectorLiteral))
	{
		return this->convert_to_vector_object(expression);
	}
	else if (EXPR_TYPE_ID == typeid(Identifier))
	{
		auto identifier = dynamic_pointer_cast<Identifier>(expression);
		auto info = this->get_variable(identifier->get_name());
		return info->value;
	}
	else if (EXPR_TYPE_ID == typeid(Unary))
	{
		return this->evaluate_unary_expression(expression);
	}
	else if (EXPR_TYPE_ID == typeid(Binary))
	{
		return this->evaluate_binary_expression(expression);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_unary_expression(Expression_ptr expression)
{
	auto unary_expression = dynamic_pointer_cast<Unary>(expression);

	auto operand_expression = unary_expression->get_expression();
	auto evaluated_operand_expression = this->evaluate_expression(operand_expression);

	Token_ptr operator_token = unary_expression->get_operator();
	auto operator_token_type = operator_token->get_type();

	switch (operator_token_type)
	{
	case WTokenType::BANG:
	{
		return this->evaluate_bang(evaluated_operand_expression);
	}
	case WTokenType::UNARY_MINUS:
	{
		return this->evaluate_unary_minus(evaluated_operand_expression);
	}
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_binary_expression(Expression_ptr expression)
{
	auto binary_expression = dynamic_pointer_cast<Binary>(expression);

	auto left_operand_expression = binary_expression->get_left_expression();
	auto right_operand_expression = binary_expression->get_right_expression();

	auto evaluated_left_operand_object = this->evaluate_expression(left_operand_expression);
	auto evaluated_right_operand_object = this->evaluate_expression(right_operand_expression);

	Token_ptr operator_token = binary_expression->get_operator();
	auto operator_token_type = operator_token->get_type();

	switch (operator_token_type)
	{
	case WTokenType::POWER:
	{
		return this->evaluate_power(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	case WTokenType::DIVISION:
	{
		return this->evaluate_division(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	case WTokenType::STAR:
	{
		return this->evaluate_star(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	case WTokenType::REMINDER:
	{
		return this->evaluate_reminder(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	case WTokenType::PLUS:
	{
		return this->evaluate_plus(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	case WTokenType::MINUS:
	{
		return this->evaluate_minus(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	case WTokenType::GREATER_THAN:
	{
		return this->evaluate_greater_than(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	case WTokenType::GREATER_THAN_EQUAL:
	{
		return this->evaluate_greater_than_equal(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	case WTokenType::LESSER_THAN:
	{
		return this->evaluate_less_than(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	case WTokenType::LESSER_THAN_EQUAL:
	{
		return this->evaluate_less_than_equal(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	case WTokenType::EQUAL_EQUAL:
	{
		return this->evaluate_equal_equal(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	case WTokenType::BANG_EQUAL:
	{
		return this->evaluate_bang_equal(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	case WTokenType::AND:
	{
		return this->evaluate_and(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	case WTokenType::OR:
	{
		return this->evaluate_or(
			evaluated_left_operand_object,
			evaluated_right_operand_object
		);
	}
	}

	return nullptr;
}

// Unary Evaluation

Object_ptr Interpreter::evaluate_bang(Object_ptr operand)
{
	if (OPERAND_TYPE_ID == typeid(BooleanObject))
	{
		auto boolean_object = dynamic_pointer_cast<BooleanObject>(operand);
		boolean_object->value = !boolean_object->value;

		return boolean_object;
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_unary_minus(Object_ptr operand)
{
	if (OPERAND_TYPE_ID == typeid(NumberObject))
	{
		auto number_object = dynamic_pointer_cast<NumberObject>(operand);
		number_object->value = -number_object->value;

		return number_object;
	}

	return nullptr;
}

// Binary Evaluation

Object_ptr Interpreter::evaluate_power(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(NumberObject) && RIGHT_TYPE_ID == typeid(NumberObject))
	{
		auto left_number_object = dynamic_pointer_cast<NumberObject>(left);
		auto right_number_object = dynamic_pointer_cast<NumberObject>(right);

		return make_shared<NumberObject>(
			std::pow(
				left_number_object->value,
				right_number_object->value
			)
			);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_division(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(NumberObject) && RIGHT_TYPE_ID == typeid(NumberObject))
	{
		auto left_number_object = dynamic_pointer_cast<NumberObject>(left);
		auto right_number_object = dynamic_pointer_cast<NumberObject>(right);

		return make_shared<NumberObject>(
			left_number_object->value / right_number_object->value
			);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_reminder(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(NumberObject) && RIGHT_TYPE_ID == typeid(NumberObject))
	{
		auto left_number_object = dynamic_pointer_cast<NumberObject>(left);
		auto right_number_object = dynamic_pointer_cast<NumberObject>(right);

		return make_shared<NumberObject>(
			std::remainder(
				left_number_object->value,
				right_number_object->value
			)
			);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_plus(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(NumberObject) && RIGHT_TYPE_ID == typeid(NumberObject))
	{
		auto left_number_object = dynamic_pointer_cast<NumberObject>(left);
		auto right_number_object = dynamic_pointer_cast<NumberObject>(right);

		return make_shared<NumberObject>(
			left_number_object->value + right_number_object->value
			);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_minus(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(NumberObject) && RIGHT_TYPE_ID == typeid(NumberObject))
	{
		auto left_number_object = dynamic_pointer_cast<NumberObject>(left);
		auto right_number_object = dynamic_pointer_cast<NumberObject>(right);

		return make_shared<NumberObject>(
			left_number_object->value - right_number_object->value
			);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_star(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(NumberObject) && RIGHT_TYPE_ID == typeid(NumberObject))
	{
		auto left_number_object = dynamic_pointer_cast<NumberObject>(left);
		auto right_number_object = dynamic_pointer_cast<NumberObject>(right);

		return make_shared<NumberObject>(
			left_number_object->value * right_number_object->value
			);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_greater_than(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(BooleanObject) && RIGHT_TYPE_ID == typeid(BooleanObject))
	{
		auto left_boolean_object = dynamic_pointer_cast<BooleanObject>(left);
		auto right_boolean_object = dynamic_pointer_cast<BooleanObject>(right);

		return make_shared<BooleanObject>(
			left_boolean_object->value > right_boolean_object->value
			);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_greater_than_equal(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(BooleanObject) && RIGHT_TYPE_ID == typeid(BooleanObject))
	{
		auto left_boolean_object = dynamic_pointer_cast<BooleanObject>(left);
		auto right_boolean_object = dynamic_pointer_cast<BooleanObject>(right);

		return make_shared<BooleanObject>(
			left_boolean_object->value >= right_boolean_object->value
			);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_less_than(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(BooleanObject) && RIGHT_TYPE_ID == typeid(BooleanObject))
	{
		auto left_boolean_object = dynamic_pointer_cast<BooleanObject>(left);
		auto right_boolean_object = dynamic_pointer_cast<BooleanObject>(right);

		return make_shared<BooleanObject>(
			left_boolean_object->value < right_boolean_object->value
			);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_less_than_equal(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(BooleanObject) && RIGHT_TYPE_ID == typeid(BooleanObject))
	{
		auto left_boolean_object = dynamic_pointer_cast<BooleanObject>(left);
		auto right_boolean_object = dynamic_pointer_cast<BooleanObject>(right);

		return make_shared<BooleanObject>(
			left_boolean_object->value <= right_boolean_object->value
			);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_equal_equal(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(BooleanObject) && RIGHT_TYPE_ID == typeid(BooleanObject))
	{
		auto left_boolean_object = dynamic_pointer_cast<BooleanObject>(left);
		auto right_boolean_object = dynamic_pointer_cast<BooleanObject>(right);

		return make_shared<BooleanObject>(
			left_boolean_object->value == right_boolean_object->value
			);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_bang_equal(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(BooleanObject) && RIGHT_TYPE_ID == typeid(BooleanObject))
	{
		auto left_boolean_object = dynamic_pointer_cast<BooleanObject>(left);
		auto right_boolean_object = dynamic_pointer_cast<BooleanObject>(right);

		return make_shared<BooleanObject>(
			left_boolean_object->value != right_boolean_object->value
			);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_and(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(BooleanObject) && RIGHT_TYPE_ID == typeid(BooleanObject))
	{
		auto left_boolean_object = dynamic_pointer_cast<BooleanObject>(left);
		auto right_boolean_object = dynamic_pointer_cast<BooleanObject>(right);

		return make_shared<BooleanObject>(
			left_boolean_object->value && right_boolean_object->value
			);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_or(Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPE_ID == typeid(BooleanObject) && RIGHT_TYPE_ID == typeid(BooleanObject))
	{
		auto left_boolean_object = dynamic_pointer_cast<BooleanObject>(left);
		auto right_boolean_object = dynamic_pointer_cast<BooleanObject>(right);

		return make_shared<BooleanObject>(
			left_boolean_object->value || right_boolean_object->value
			);
	}

	return nullptr;
}

// Statements

void Interpreter::create_variable(Statement_ptr statement)
{
	auto declaration = dynamic_pointer_cast<VariableDeclaration>(statement);

	string name = declaration->get_variable_name();

	bool is_public = declaration->is_public_declaration();
	bool is_mutable = declaration->is_mutable_declaration();
	Type_ptr type = declaration->get_type();

	Expression_ptr expression = declaration->get_expression();
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
	auto loop = dynamic_pointer_cast<Loop>(statement);

	if (STAT_TYPE_ID == typeid(Break))
	{
	}
	else if (STAT_TYPE_ID == typeid(Continue))
	{
	}
}