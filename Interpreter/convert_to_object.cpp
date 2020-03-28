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

shared_ptr<VectorObject> Interpreter::to_vector_object(Expression_ptr expression)
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

shared_ptr<TupleObject> Interpreter::to_tuple_object(Expression_ptr expression)
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

shared_ptr<MapObject> Interpreter::to_map_object(Expression_ptr expression)
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

shared_ptr<RecordObject> Interpreter::to_record_object(Expression_ptr expression)
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