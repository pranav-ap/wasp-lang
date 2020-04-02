#pragma once

#include "pch.h"
#include "Interpreter.h"
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
using std::map;
using std::optional;
using std::make_shared;

void Interpreter::execute()
{
	for (auto statement : mod.get_statements())
		statement->interpret(*this);
}

// Statement Visitors

void Interpreter::visit(VariableDeclaration_ptr declaration)
{
	string name = declaration->name;

	bool is_public = declaration->is_public;
	bool is_mutable = declaration->is_mutable;
	Type_ptr type = declaration->type;

	auto result = declaration->expression->interpret(*this);

	env->create_variable(name, is_public, is_mutable, type, result);
}

void Interpreter::visit(Assignment_ptr assignment)
{
	auto name = assignment->name;
	auto info = env->get_variable(name);

	if (!info->is_mutable)
	{
		return;
	}

	auto result = assignment->expression->interpret(*this);

	if (result == nullptr)
	{
		// Error
		return;
	}

	info->value = result;
}

void Interpreter::visit(Branch_ptr branch)
{
	auto result = branch->condition->interpret(*this);

	if (typeid(result) != typeid(BooleanObject))
	{
		// ERROR
		return;
	}

	auto result_boolean_object = dynamic_pointer_cast<BooleanObject>(result);

	if (result_boolean_object->value)
	{
		evaluate_branch_block(branch->consequence);
	}
	else
	{
		evaluate_branch_block(branch->alternative);
	}
}

void Interpreter::visit(Loop_ptr loop)
{
	Block_ptr block = loop->block;
	bool must_continue = false;

	do
	{
		for (auto const& statement : *block)
		{
			if (typeid(statement) == typeid(Break))
			{
				break;
			}
			else if (typeid(statement) == typeid(Continue))
			{
				must_continue = true;
				break;
			}

			statement->interpret(*this);
		}
	} while (must_continue);
}

void Interpreter::visit(Break_ptr statement)
{
	// Error
}

void Interpreter::visit(Continue_ptr statement)
{
	// Error
}

void Interpreter::visit(ExpressionStatement_ptr statement)
{
	auto _x = statement->expression->interpret(*this);
}

void Interpreter::visit(UDTDefinition_ptr statement)
{
	bool is_public = statement->is_public;
	string name = statement->name;
	map<string, Type_ptr> member_types = statement->member_types;

	env->create_UDT(name, is_public, member_types);
}

void Interpreter::visit(FunctionDefinition_ptr statement)
{
	string name = statement->name;
	bool is_public = statement->is_public;
	map<string, Type_ptr> arguments = statement->arguments;
	optional<Type_ptr> return_type = statement->return_type;
	Block_ptr body = statement->body;

	env->create_function(
		name,
		is_public,
		arguments,
		return_type,
		body
	);
}

void Interpreter::visit(Return_ptr statement)
{
}

void Interpreter::visit(Import_ptr statement)
{
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

	return expression_objects;
}

Object_ptr Interpreter::visit(UDTLiteral_ptr udt_literal)
{
	auto udt_object = make_shared<UDTObject>();

	for (auto const& [key, value_expr] : udt_literal->pairs)
	{
		auto value_object = value_expr->interpret(*this);
		udt_object->add(key, value_object);
	}

	return udt_object;
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

	if (OPERAND_TYPEID == typeid(NumberObject))
	{
		auto operand_number_object = dynamic_pointer_cast<NumberObject>(operand);
		return perform_operation(token_type, operand_number_object);
	}
	else if (OPERAND_TYPEID == typeid(BooleanObject))
	{
		auto operand_boolean_object = dynamic_pointer_cast<BooleanObject>(operand);
		return perform_operation(token_type, operand_boolean_object);
	}

	return nullptr;
}

Object_ptr Interpreter::visit(Binary_ptr binary_expression)
{
	auto left = binary_expression->left->interpret(*this);
	auto right = binary_expression->right->interpret(*this);
	auto token_type = binary_expression->op->type;

	if (LEFT_TYPEID == typeid(NumberObject) && RIGHT_TYPEID == typeid(NumberObject))
	{
		auto left_number_object = dynamic_pointer_cast<NumberObject>(left);
		auto right_number_object = dynamic_pointer_cast<NumberObject>(right);
		return perform_operation(token_type, left_number_object, right_number_object);
	}
	else if (LEFT_TYPEID == typeid(BooleanObject) && RIGHT_TYPEID == typeid(BooleanObject))
	{
		auto left_boolean_object = dynamic_pointer_cast<BooleanObject>(left);
		auto right_boolean_object = dynamic_pointer_cast<BooleanObject>(right);
		return perform_operation(token_type, left_boolean_object, right_boolean_object);
	}
	else if (LEFT_TYPEID == typeid(StringObject) && RIGHT_TYPEID == typeid(StringObject))
	{
		auto left_string_object = dynamic_pointer_cast<StringObject>(left);
		auto right_string_object = dynamic_pointer_cast<StringObject>(right);
		return perform_operation(token_type, left_string_object, right_string_object);
	}

	return nullptr;
}

Object_ptr Interpreter::visit(VectorMemberAccess_ptr expression)
{
	auto name = expression->name;
	auto info = env->get_variable(name);

	auto index_expression = expression->expression;
	auto index_object = index_expression->interpret(*this);

	if (typeid(index_object) == typeid(NumberLiteral))
	{
		auto index_number_object = dynamic_pointer_cast<NumberObject>(index_object);
		double index = index_number_object->value;

		if (!info->value)
		{
			// Error
			return nullptr;
		}

		auto vector_object = dynamic_pointer_cast<VectorObject>(info->value);
		return vector_object->values[index];
	}

	return nullptr;
}

Object_ptr Interpreter::visit(UDTMemberAccess_ptr expression)
{
	auto UDT_name = expression->UDT_name;
	auto member_name = expression->member_name;

	auto info = env->get_variable(UDT_name);

	if (!info->value)
	{
		// Error
		return nullptr;
	}

	if (typeid(info->value) == typeid(UDTLiteral))
	{
		auto UDT_object = dynamic_pointer_cast<UDTObject>(info->value);
		return UDT_object->pairs[member_name];
	}

	return nullptr;
}

Object_ptr Interpreter::visit(FunctionCall_ptr expression)
{
	return nullptr;
}

Object_ptr Interpreter::visit(Range_ptr expression)
{
	return nullptr;
}

// Evaluate Block

void Interpreter::evaluate_branch_block(Block_ptr block)
{
	for (auto& statement : *block)
	{
		if (typeid(statement) == typeid(Break))
		{
			// pop scope from scopes
			break;
		}
		else if (typeid(statement) == typeid(Continue))
		{
			// ERROR
			break;
		}

		statement->interpret(*this);
	}
}

// Perform Operation

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

	return nullptr;
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

	return nullptr;
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

	return nullptr;
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

	return nullptr;
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

	return nullptr;
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

	return nullptr;
}