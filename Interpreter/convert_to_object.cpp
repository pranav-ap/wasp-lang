#pragma once
#include "pch.h"
#include "Expression.h"
#include "ObjectSystem.h"
#include "Interpreter.h"

#include <memory>
#include <string>

using std::make_shared;
using std::shared_ptr;
using std::string;

NumberObject_ptr Interpreter::convert_to_number_object(Expression_ptr expression)
{
	auto number_literal = dynamic_pointer_cast<NumberLiteral>(expression);
	return make_shared<NumberObject>(number_literal->get_value());
}

StringObject_ptr Interpreter::convert_to_string_object(Expression_ptr expression)
{
	auto string_literal = dynamic_pointer_cast<StringLiteral>(expression);
	return make_shared<StringObject>(string_literal->get_value());
}

BooleanObject_ptr Interpreter::convert_to_boolean_object(Expression_ptr expression)
{
	auto boolean_literal = dynamic_pointer_cast<BooleanLiteral>(expression);
	return make_shared<BooleanObject>(boolean_literal->get_value());
}

VectorObject_ptr Interpreter::convert_to_vector_object(Expression_ptr expression)
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

TupleObject_ptr Interpreter::convert_to_tuple_object(Expression_ptr expression)
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

MapObject_ptr Interpreter::to_map_object(Expression_ptr expression)
{
	auto map_literal = dynamic_pointer_cast<MapLiteral>(expression);
	auto pairs = map_literal->get_pairs();

	auto map_object = make_shared<MapObject>();

	for (auto const& [key, value] : pairs)
	{
		auto key_object = this->evaluate_expression(key);
		auto key_scalar_object = dynamic_pointer_cast<ScalarObject>(key_object);

		auto value_object = this->evaluate_expression(value);

		map_object->add(key_scalar_object, value_object);
	}

	return map_object;
}

RecordObject_ptr Interpreter::to_record_object(Expression_ptr expression)
{
	auto record_literal = dynamic_pointer_cast<RecordLiteral>(expression);
	auto pairs = record_literal->get_pairs();

	auto record_object = make_shared<RecordObject>();

	for (auto const& [key, value] : pairs)
	{
		auto value_object = this->evaluate_expression(value);
		record_object->add(key, value_object);
	}

	return record_object;
}