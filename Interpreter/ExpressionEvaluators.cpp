#pragma once
#include "pch.h"
#include "Token.h"
#include "Expression.h"
#include "ObjectSystem.h"
#include "Interpreter.h"

#include <memory>
#include <typeinfo>
#include <iostream>
#include <string>
#include <math.h>

using std::make_shared;

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

// Binary Evaluation

Object_ptr Interpreter::evaluate_binary_expression(Expression_ptr expression)
{
	auto binary_expression = dynamic_pointer_cast<Binary>(expression);

	auto left_operand_expression = binary_expression->get_left_expression();
	auto right_operand_expression = binary_expression->get_right_expression();

	auto evaluated_left_operand_object = this->evaluate_expression(left_operand_expression);
	auto evaluated_right_operand_object = this->evaluate_expression(right_operand_expression);

	Token_ptr operator_token = binary_expression->get_operator();
	auto operator_token_type = operator_token->type;

	if (typeid(evaluated_left_operand_object) == typeid(NumberObject) && typeid(evaluated_right_operand_object) == typeid(NumberObject))
	{
		return this->evaluate_numeric_expression(operator_token_type, evaluated_left_operand_object, evaluated_right_operand_object);
	}
	else if (typeid(evaluated_left_operand_object) == typeid(BooleanObject) && typeid(evaluated_right_operand_object) == typeid(BooleanObject))
	{
		return this->evaluate_boolean_expression(operator_token_type, evaluated_left_operand_object, evaluated_right_operand_object);
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_numeric_expression(WTokenType token_type, Object_ptr left_object, Object_ptr right_object)
{
	auto left = dynamic_pointer_cast<NumberObject>(left_object);
	auto right = dynamic_pointer_cast<NumberObject>(right_object);

	switch (token_type)
	{
	case WTokenType::POWER:
	{
		return make_shared<NumberObject>(std::pow(left->value, right->value));
	}
	case WTokenType::DIVISION:
	{
		return make_shared<NumberObject>(left->value / right->value);
	}
	case WTokenType::REMINDER:
	{
		return make_shared<NumberObject>(std::remainder(left->value, right->value));
	}
	case WTokenType::STAR:
	{
		return make_shared<NumberObject>(left->value * right->value);
	}
	case WTokenType::PLUS:
	{
		return make_shared<NumberObject>(left->value + right->value);
	}
	case WTokenType::MINUS:
	{
		return make_shared<NumberObject>(left->value - right->value);
	}
	case WTokenType::GREATER_THAN:
	{
		return make_shared<BooleanObject>(left->value > right->value);
	}
	case WTokenType::GREATER_THAN_EQUAL:
	{
		return make_shared<BooleanObject>(left->value >= right->value);
	}
	case WTokenType::LESSER_THAN:
	{
		return make_shared<BooleanObject>(left->value < right->value);
	}
	case WTokenType::LESSER_THAN_EQUAL:
	{
		return make_shared<BooleanObject>(left->value <= right->value);
	}
	case WTokenType::EQUAL_EQUAL:
	{
		return make_shared<BooleanObject>(left->value == right->value);
	}
	case WTokenType::BANG_EQUAL:
	{
		return make_shared<BooleanObject>(left->value != right->value);
	}
	}

	return nullptr;
}

Object_ptr Interpreter::evaluate_boolean_expression(WTokenType token_type, Object_ptr left_object, Object_ptr right_object)
{
	auto left = dynamic_pointer_cast<BooleanObject>(left_object);
	auto right = dynamic_pointer_cast<BooleanObject>(right_object);

	switch (token_type)
	{
	case WTokenType::EQUAL_EQUAL:
	{
		return make_shared<BooleanObject>(left->value == right->value);
	}
	case WTokenType::BANG_EQUAL:
	{
		return make_shared<BooleanObject>(left->value != right->value);
	}
	case WTokenType::AND:
	{
		return make_shared<BooleanObject>(left->value && right->value);
	}
	case WTokenType::OR:
	{
		return make_shared<BooleanObject>(left->value || right->value);
	}
	}

	return nullptr;
}

// Unary Evaluation

Object_ptr Interpreter::evaluate_unary_expression(Expression_ptr expression)
{
	auto unary_expression = dynamic_pointer_cast<Unary>(expression);

	auto operand_expression = unary_expression->get_expression();
	auto evaluated_operand_expression = this->evaluate_expression(operand_expression);

	Token_ptr operator_token = unary_expression->get_operator();
	auto operator_token_type = operator_token->type;

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