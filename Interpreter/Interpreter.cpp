#pragma once

#include "pch.h"
#include "logger.h"
#include "Interpreter.h"
#include "StatementVisitor.h"
#include "ExpressionVisitor.h"
#include "Module.h"
#include "ObjectSystem.h"

#include <memory>
#include <string>
#include <map>
#include <optional>

#define OPERAND_TYPEID typeid(*operand)
#define LEFT_TYPEID typeid(*left)
#define RIGHT_TYPEID typeid(*right)

using std::string;
using std::to_string;
using std::map;
using std::vector;
using std::optional;
using std::make_shared;

void Interpreter::execute()
{
	for (auto statement : mod.nodes)
		statement->interpret(*this);
}

// Statement Visitors

Object_ptr Interpreter::visit(VariableDeclaration_ptr declaration)
{
	string name = declaration->name;

	bool is_public = declaration->is_public;
	bool is_mutable = declaration->is_mutable;
	auto type = declaration->type;

	auto result = declaration->expression->interpret(*this);

	string message = "Cannot initialize " + name + " with a malformed expresion";
	FATAL_IF_NULLPTR(result, message);

	env->create_variable(name, is_public, is_mutable, type, result);

	return make_shared<VoidObject>();
}

Object_ptr Interpreter::visit(Assignment_ptr assignment)
{
	auto name = assignment->name;
	auto info = env->get_variable(name);

	string message = name + " is not mutable";
	FATAL_IF_FALSE(info->is_mutable, message);

	auto result = assignment->expression->interpret(*this);

	message = "Cannot assign a malformed expresion to " + name;
	FATAL_IF_NULLPTR(result, message);

	info->value = result;

	return make_shared<VoidObject>();
}

Object_ptr Interpreter::visit(Branch_ptr branch)
{
	auto condition_object = branch->condition->interpret(*this);
	FATAL_IF_NULLPTR(condition_object, "Condition is malformed");
	FATAL_IF_TRUE(typeid(condition_object) != typeid(BooleanObject), "Condition has to return boolean value");

	auto condition_boolean_object = to_boolean_object(condition_object);
	auto block = condition_boolean_object->value ?
		branch->consequence :
		branch->alternative;

	env->enter_branch_scope();

	auto block_result_object = evaluate_block(move(block));
	FATAL_IF_NULLPTR(block_result_object, "The branch's evaluated result is nullptr");

	env->leave_scope();

	return block_result_object;
}

Object_ptr Interpreter::visit(Loop_ptr loop)
{
	env->enter_loop_scope();

	auto block_result_object = evaluate_loop(loop->block);
	FATAL_IF_NULLPTR(block_result_object, "The loop's evaluated result is nullptr");

	env->leave_scope();

	return block_result_object;
}

Object_ptr Interpreter::visit(ForEachLoop_ptr statement)
{
	auto info = env->get_variable(statement->iterable_name);
	FATAL_IF_NULLPTR(info, "Vector Object does not exist");

	auto object = info->value;
	FATAL_IF_NULLPTR(object, "object is nullptr");

	auto vector_object = to_vector_object(object);
	FATAL_IF_NULLPTR(vector_object, "vector_object is nullptr");

	env->enter_loop_scope();
	env->create_variable(statement->item_name, false, true, info->type, make_shared<VoidObject>());

	auto vector_type = dynamic_pointer_cast<VectorType>(info->type);

	for (auto const& element : vector_object->values)
	{
		FATAL_IF_FALSE(are_same_type(element, vector_type->type), "Element has incorrect type");

		env->set_variable(statement->item_name, element);

		auto result = evaluate_block(statement->block);
		FATAL_IF_NULLPTR(result, "result is nullptr");

		if (typeid(*result) == typeid(ReturnObject))
		{
			env->leave_scope();
			return result;
		}
		else if (typeid(*result) == typeid(BreakObject))
		{
			break;
		}
		else if (typeid(*result) == typeid(ContinueObject))
		{
			continue;
		}
	}

	env->leave_scope();

	return make_shared<VoidObject>();
}

Object_ptr Interpreter::visit(Break_ptr statement)
{
	if (env->is_inside_loop_scope())
		return make_shared<BreakObject>();

	FATAL("Break must be used within a loop");
}

Object_ptr Interpreter::visit(Continue_ptr statement)
{
	if (env->is_inside_loop_scope())
		return make_shared<ContinueObject>();

	FATAL("Continue must be used within a loop");
}

Object_ptr Interpreter::visit(Return_ptr statement)
{
	if (env->is_inside_function_scope())
	{
		if (statement->expression.has_value())
		{
			auto result = statement->expression.value()->interpret(*this);
			FATAL_IF_NULLPTR(result, "The evaluated return expession results in nullptr");
			return make_shared<ReturnObject>(result);
		}

		return make_shared<ReturnObject>();
	}

	FATAL("Return must be used inside a function");
}

Object_ptr Interpreter::visit(ExpressionStatement_ptr statement)
{
	auto result = statement->expression->interpret(*this);
	FATAL_IF_NULLPTR(result, "The evaluated expession results in nullptr");

	return result;
}

Object_ptr Interpreter::visit(UDTDefinition_ptr statement)
{
	bool is_public = statement->is_public;
	string name = statement->name;
	auto member_types = statement->member_types;

	env->create_UDT(name, is_public, member_types);

	return make_shared<VoidObject>();
}

Object_ptr Interpreter::visit(FunctionDefinition_ptr statement)
{
	string name = statement->name;
	bool is_public = statement->is_public;
	auto arguments = statement->arguments;
	auto return_type = statement->return_type;
	auto body = statement->body;

	env->create_function(name, is_public, arguments, return_type, body);

	return make_shared<VoidObject>();
}

Object_ptr Interpreter::visit(EnumDefinition_ptr statement)
{
	string name = statement->name;
	bool is_public = statement->is_public;
	auto members = statement->members;

	env->create_enum(name, is_public, members);

	return make_shared<VoidObject>();
}

Object_ptr Interpreter::visit(Import_ptr statement)
{
	return make_shared<VoidObject>();
}

Object_ptr Interpreter::visit(ImportSTD_ptr statement)
{
	return make_shared<VoidObject>();
}

// Expression Visitors

Object_ptr Interpreter::visit(StringLiteral_ptr string_literal)
{
	return make_shared<StringObject>(string_literal->value);
}

Object_ptr Interpreter::visit(NumberLiteral_ptr number_literal)
{
	return make_shared<NumberObject>(number_literal->value);
}

Object_ptr Interpreter::visit(BooleanLiteral_ptr boolean_literal)
{
	return make_shared<BooleanObject>(boolean_literal->value);
}

Object_ptr Interpreter::visit(VectorLiteral_ptr vector_literal)
{
	auto expression_objects = make_shared<VectorObject>();

	for (const auto expression : vector_literal->expressions)
	{
		auto result = expression->interpret(*this);
		expression_objects->add(result);
	}

	return move(expression_objects);
}

Object_ptr Interpreter::visit(UDTLiteral_ptr udt_literal)
{
	auto UDT_object = make_shared<UDTObject>();

	for (auto const& [key, value_expr] : udt_literal->pairs)
	{
		auto value_object = value_expr->interpret(*this);
		UDT_object->add(key, value_object);
	}

	return move(UDT_object);
}

Object_ptr Interpreter::visit(Identifier_ptr expression)
{
	auto name = expression->name;
	auto info = env->get_variable(name);

	return info->value;
}

Object_ptr Interpreter::visit(Unary_ptr unary_expression)
{
	auto operand = unary_expression->operand->interpret(*this);
	auto token_type = unary_expression->op->type;

	return perform_operation(token_type, operand);
}

Object_ptr Interpreter::visit(Binary_ptr binary_expression)
{
	auto left = binary_expression->left->interpret(*this);
	auto right = binary_expression->right->interpret(*this);
	auto token_type = binary_expression->op->type;

	return perform_operation(token_type, left, right);
}

Object_ptr Interpreter::visit(VectorMemberAccess_ptr expression)
{
	auto name = expression->name;
	auto info = env->get_variable(name);

	string message = name + " is not a vector";
	FATAL_IF_TRUE(info->value, message);
	auto vector_object = to_vector_object(info->value);

	// Get Index

	auto index_expression = expression->expression;
	auto index_object = index_expression->interpret(*this);

	FATAL_IF_TRUE(
		typeid(index_object) != typeid(NumberLiteral),
		"Vector elements must be accessed by a numeric index"
	);

	auto index_number_object = to_number_object(index_object);
	double index = index_number_object->value;

	return vector_object->values[index];
}

Object_ptr Interpreter::visit(UDTMemberAccess_ptr expression)
{
	auto UDT_name = expression->UDT_name;
	auto info = env->get_variable(UDT_name);

	string message = UDT_name + " value is not a UDT";
	FATAL_IF_TRUE(typeid(info->value) != typeid(UDTLiteral), message);

	auto UDT_object = to_UDT_object(info->value);
	return UDT_object->pairs[expression->member_name];
}

Object_ptr Interpreter::visit(EnumMemberAccess_ptr expression)
{
	auto enum_name = expression->enum_name;
	auto info = env->get_enum(enum_name);

	string message = enum_name + " value is not a UDT";
	FATAL_IF_TRUE(typeid(info->members) != typeid(vector<string>), message);

	return make_shared<EnumObject>(enum_name, expression->member_name);
}

Object_ptr Interpreter::visit(FunctionCall_ptr expression)
{
	env->enter_function_scope();

	auto info = env->get_function(expression->name);
	auto formal_arguments = info->arguments;

	int index = 0;

	for (auto const& argument : expression->arguments)
	{
		auto formal_argument_name = formal_arguments[index].first;
		auto formal_argument_type = formal_arguments[index].second;

		auto object = argument->interpret(*this);

		env->create_variable(
			formal_argument_name,
			false,
			true,
			formal_argument_type,
			object
		);

		index++;
	}

	auto result = evaluate_block(info->body);
	env->leave_scope();
	return result;
}

Object_ptr Interpreter::visit(Range_ptr expression)
{
	FATAL("Range must be used along with vector slicing or a FOR loop");
}

// Perform Operation

Object_ptr Interpreter::perform_operation(WTokenType token_type, Object_ptr operand)
{
	if (OPERAND_TYPEID == typeid(NumberObject))
	{
		return perform_operation(token_type, to_number_object(operand));
	}
	else if (OPERAND_TYPEID == typeid(BooleanObject))
	{
		return perform_operation(token_type, to_boolean_object(operand));
	}

	FATAL("The Unary Operation is not defined for this operand");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, Object_ptr left, Object_ptr right)
{
	if (LEFT_TYPEID == typeid(NumberObject) && RIGHT_TYPEID == typeid(NumberObject))
	{
		return perform_operation(token_type, to_number_object(left), to_number_object(right));
	}
	else if (LEFT_TYPEID == typeid(BooleanObject) && RIGHT_TYPEID == typeid(BooleanObject))
	{
		return perform_operation(token_type, to_boolean_object(left), to_boolean_object(right));
	}
	else if (LEFT_TYPEID == typeid(StringObject) && RIGHT_TYPEID == typeid(StringObject))
	{
		return perform_operation(token_type, to_string_object(left), to_string_object(right));
	}
	else if (LEFT_TYPEID == typeid(StringObject) && RIGHT_TYPEID == typeid(NumberObject))
	{
		return perform_operation(token_type, to_string_object(left), to_number_object(right));
	}

	FATAL("The Binary Operation is not defined for these operands");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, NumberObject_ptr operand)
{
	switch (token_type)
	{
	case WTokenType::UNARY_MINUS:
	{
		operand->value = -operand->value;
		return operand;
	}
	}

	FATAL("Operation not supported");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, BooleanObject_ptr operand)
{
	switch (token_type)
	{
	case WTokenType::BANG:
	{
		operand->value = !operand->value;
		return operand;
	}
	}

	FATAL("Operation not supported");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, BooleanObject_ptr left, BooleanObject_ptr right)
{
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

	FATAL("Operation not supported");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, NumberObject_ptr left, NumberObject_ptr right)
{
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

	FATAL("Operation not supported");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, StringObject_ptr left, StringObject_ptr right)
{
	switch (token_type)
	{
	case WTokenType::PLUS:
	{
		return make_shared<StringObject>(left->value + right->value);
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

	FATAL("Operation not supported");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, StringObject_ptr left, NumberObject_ptr right)
{
	switch (token_type)
	{
	case WTokenType::STAR:
	{
		int count = 0;
		auto repetitions = right->value;

		string result = "";

		while (count < repetitions)
		{
			result += left->value;
			count++;
		}

		return make_shared<StringObject>(result);
	}
	}

	FATAL("Operation not supported");
}

Object_ptr Interpreter::evaluate_block(Block_ptr block)
{
	for (auto& statement : *block)
	{
		auto result = statement->interpret(*this);
		FATAL_IF_NULLPTR(result, "result is null");

		if (
			typeid(*result) == typeid(ReturnObject) ||
			typeid(*result) == typeid(ContinueObject) ||
			typeid(*result) == typeid(BreakObject)
			) {
			return result;
		}
	}

	return make_shared<VoidObject>();
}

Object_ptr Interpreter::evaluate_loop(Block_ptr block)
{
	while (true)
	{
		auto result = evaluate_block(block);

		if (typeid(*result) == typeid(ReturnObject))
		{
			return result;
		}
		else if (typeid(*result) == typeid(ContinueObject))
		{
			continue;
		}
		else if (typeid(*result) == typeid(BreakObject))
		{
			break;
		}
	}

	return make_shared<VoidObject>();
}

bool Interpreter::are_same_type(Object_ptr obj, Type_ptr type)
{
	if (
		(typeid(*obj) == typeid(NumberObject) && typeid(*type) == typeid(NumberType)) ||
		(typeid(*obj) == typeid(StringObject) && typeid(*type) == typeid(StringType)) ||
		(typeid(*obj) == typeid(BooleanObject) && typeid(*type) == typeid(BooleanType)) ||
		(typeid(*obj) == typeid(VectorObject) && typeid(*type) == typeid(VectorType)) ||
		(typeid(*obj) == typeid(UDTObject) && typeid(*type) == typeid(UDTType))
		)
	{
		return true;
	}

	return false;
}

// Converters

NumberObject_ptr Interpreter::to_number_object(Object_ptr obj)
{
	return dynamic_pointer_cast<NumberObject>(obj);
}

StringObject_ptr Interpreter::to_string_object(Object_ptr obj)
{
	return dynamic_pointer_cast<StringObject>(obj);
}

BooleanObject_ptr Interpreter::to_boolean_object(Object_ptr obj)
{
	return dynamic_pointer_cast<BooleanObject>(obj);
}

VectorObject_ptr Interpreter::to_vector_object(Object_ptr obj)
{
	return dynamic_pointer_cast<VectorObject>(obj);
}

UDTObject_ptr Interpreter::to_UDT_object(Object_ptr obj)
{
	return dynamic_pointer_cast<UDTObject>(obj);
}