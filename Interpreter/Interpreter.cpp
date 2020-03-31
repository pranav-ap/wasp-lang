#pragma once

#include "pch.h"
#include "Interpreter.h"
#include "Module.h"
#include "ObjectSystem.h"

#include <memory>
#include <string>

using std::string;
using std::make_shared;

void Interpreter::execute()
{
	for (auto statement : this->mod.get_statements())
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

	this->env->create_and_set_variable(name, is_public, is_mutable, type, result);
}

void Interpreter::visit(Assignment_ptr assignment)
{
	auto name = assignment->name;
	auto info = this->env->get_variable(name);

	if (!info->is_mutable)
	{
		return;
	}

	auto result = assignment->expression->interpret(*this);

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
		this->evaluate_branch_block(branch->consequence);
	}
	else
	{
		this->evaluate_branch_block(branch->alternative);
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
}

void Interpreter::visit(Continue_ptr statement)
{
}

void Interpreter::visit(ExpressionStatement_ptr statement)
{
	auto _x = statement->expression->interpret(*this);
}

// 0.2

void Interpreter::visit(Alias_ptr statement)
{
}

void Interpreter::visit(RecordDefinition_ptr statement)
{
}

void Interpreter::visit(FunctionDefinition_ptr statement)
{
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

Object_ptr Interpreter::visit(MapLiteral_ptr map_literal)
{
	auto map_object = make_shared<MapObject>();

	for (auto const& [key_expr, value_expr] : map_literal->pairs)
	{
		auto key_object = key_expr->interpret(*this);
		auto value_object = value_expr->interpret(*this);

		map_object->add(key_object, value_object);
	}

	return map_object;
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
	auto info = this->env->get_variable(name);

	return info->value;
}

Object_ptr Interpreter::visit(Unary_ptr unary_expression)
{
	auto operand = unary_expression->operand->interpret(*this);
	auto token_type = unary_expression->op->type;

	if (typeid(operand) == typeid(NumberObject))
	{
		auto operand_number_object = dynamic_pointer_cast<NumberObject>(operand);
		this->perform_unary_operation(token_type, operand_number_object);
	}
	else if (typeid(operand) == typeid(BooleanObject))
	{
		auto operand_boolean_object = dynamic_pointer_cast<BooleanObject>(operand);
		this->perform_unary_operation(token_type, operand_boolean_object);
	}

	return operand;
}

Object_ptr Interpreter::visit(Binary_ptr binary_expression)
{
	auto left = binary_expression->left->interpret(*this);
	auto right = binary_expression->right->interpret(*this);
	auto token_type = binary_expression->op->type;

	if (typeid(left) == typeid(NumberObject) && typeid(right) == typeid(NumberObject))
	{
		auto left_number_object = dynamic_pointer_cast<NumberObject>(left);
		auto right_number_object = dynamic_pointer_cast<NumberObject>(right);
		return this->perform_binary_operation(token_type, left_number_object, right_number_object);
	}
	else if (typeid(left) == typeid(BooleanObject) && typeid(right) == typeid(BooleanObject))
	{
		auto left_boolean_object = dynamic_pointer_cast<BooleanObject>(left);
		auto right_boolean_object = dynamic_pointer_cast<BooleanObject>(right);
		return this->perform_binary_operation(token_type, left_boolean_object, right_boolean_object);
	}

	return nullptr;
}

// 0.2

Object_ptr Interpreter::visit(MemberAccess_ptr expression)
{
	return nullptr;
}

Object_ptr Interpreter::visit(RecordMemberAccess_ptr expression)
{
	return nullptr;
}

Object_ptr Interpreter::visit(FunctionCall_ptr expression)
{
	return nullptr;
}

Object_ptr Interpreter::visit(InclusiveRange_ptr expression)
{
	return nullptr;
}

Object_ptr Interpreter::visit(ExclusiveRange_ptr expression)
{
	return nullptr;
}

// UTILS

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

Object_ptr Interpreter::perform_unary_operation(WTokenType token_type, NumberObject_ptr operand)
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

Object_ptr Interpreter::perform_unary_operation(WTokenType token_type, BooleanObject_ptr operand)
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

Object_ptr Interpreter::perform_binary_operation(WTokenType token_type, BooleanObject_ptr left, BooleanObject_ptr right)
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

Object_ptr Interpreter::perform_binary_operation(WTokenType token_type, NumberObject_ptr left, NumberObject_ptr right)
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
}