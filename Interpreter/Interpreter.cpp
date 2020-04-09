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
#include <variant>

#define OPERAND_TYPEID typeid(*operand)
#define LEFT_TYPEID typeid(*left)
#define RIGHT_TYPEID typeid(*right)

using std::string;
using std::to_string;
using std::map;
using std::vector;
using std::optional;
using std::make_shared;
using std::holds_alternative;

void Interpreter::execute()
{
	for (auto statement : mod.nodes)
		statement->interpret(*this);
}

// Statement Visitors

ObjectVariant_ptr Interpreter::visit(VariableDeclaration_ptr declaration)
{
	auto result = declaration->expression->interpret(*this);

	env->create_variable(
		declaration->name,
		declaration->is_public,
		declaration->is_mutable,
		declaration->type,
		result
	);

	return VOID;
}

ObjectVariant_ptr Interpreter::visit(Assignment_ptr assignment)
{
	auto name = assignment->name;
	auto info = env->get_variable(name);

	FATAL_IF_FALSE(info.is_mutable, name + " is not mutable");

	auto result = assignment->expression->interpret(*this);
	info.value = result;

	return VOID;
}

ObjectVariant_ptr Interpreter::visit(Branch_ptr branch)
{
	auto condition = branch->condition->interpret(*this);
	FATAL_IF_FALSE(holds_alternative<bool>(*condition), "Condition has to return bool");

	bool result = get<bool>(*condition);
	auto block = result ? branch->consequence : branch->alternative;

	env->enter_branch_scope();
	auto block_result = evaluate_block(move(block));
	env->leave_scope();

	return block_result;
}

ObjectVariant_ptr Interpreter::visit(Loop_ptr loop)
{
	env->enter_loop_scope();

	while (true)
	{
		auto result = evaluate_block(loop->block);

		if (holds_alternative<ReturnObject>(*result))
		{
			env->leave_scope();
			return result;
		}
		else if (holds_alternative<ContinueObject>(*result))
		{
			continue;
		}
		else if (holds_alternative<BreakObject>(*result))
		{
			break;
		}
	}

	env->leave_scope();
	return VOID;
}

ObjectVariant_ptr Interpreter::visit(ForEachLoop_ptr statement)
{
	auto info = env->get_variable(statement->iterable_name);
	FATAL_IF_FALSE(holds_alternative<VectorObject>(*info.value), "Foreach can only iterate over Vector Objects");
	auto vector_object = get<VectorObject>(*info.value);

	env->enter_loop_scope();
	env->create_variable(
		statement->item_name,
		false,
		true,
		info.type,
		VOID
	);

	auto vector_type = dynamic_pointer_cast<VectorType>(info.type);

	for (auto const& element : vector_object.values)
	{
		FATAL_IF_FALSE(are_same_type(element, vector_type->type), "Element has incorrect type");
		env->set_variable(statement->item_name, element);

		auto result = evaluate_block(statement->block);

		if (holds_alternative<ReturnObject>(*result))
		{
			env->leave_scope();
			return result;
		}
		else if (holds_alternative<ContinueObject>(*result))
		{
			continue;
		}
		else if (holds_alternative<BreakObject>(*result))
		{
			break;
		}
	}

	env->leave_scope();
	return VOID;
}

ObjectVariant_ptr Interpreter::visit(Break_ptr statement)
{
	if (env->is_inside_loop_scope())
		return make_shared<ObjectVariant>(BreakObject());

	FATAL("Break must be used within a loop");
}

ObjectVariant_ptr Interpreter::visit(Continue_ptr statement)
{
	if (env->is_inside_loop_scope())
		return make_shared<ObjectVariant>(ContinueObject());

	FATAL("Continue must be used within a loop");
}

ObjectVariant_ptr Interpreter::visit(Return_ptr statement)
{
	if (env->is_inside_function_scope())
	{
		if (statement->expression.has_value())
		{
			auto result = statement->expression.value()->interpret(*this);
			FATAL_IF_NULLPTR(result, "The evaluated return expession results in nullptr");
			return make_shared<ObjectVariant>(ReturnObject(result));
		}

		return VOID;
	}

	FATAL("Return must be used inside a function");
}

ObjectVariant_ptr Interpreter::visit(ExpressionStatement_ptr statement)
{
	return statement->expression->interpret(*this);
}

ObjectVariant_ptr Interpreter::visit(UDTDefinition_ptr def)
{
	env->create_UDT(
		def->name,
		def->is_public,
		def->member_types
	);

	return VOID;
}

ObjectVariant_ptr Interpreter::visit(FunctionDefinition_ptr def)
{
	env->create_function(
		def->name,
		def->is_public,
		def->arguments,
		def->return_type,
		def->body
	);

	return VOID;
}

ObjectVariant_ptr Interpreter::visit(EnumDefinition_ptr def)
{
	env->create_enum(
		def->name,
		def->is_public,
		def->members
	);

	return VOID;
}

ObjectVariant_ptr Interpreter::visit(Import_ptr statement)
{
	return VOID;
}

ObjectVariant_ptr Interpreter::visit(ImportSTD_ptr statement)
{
	return VOID;
}

// Expression Visitors

ObjectVariant_ptr Interpreter::visit(StringLiteral_ptr string_literal)
{
	return make_shared<ObjectVariant>(string_literal->value);
}

ObjectVariant_ptr Interpreter::visit(NumberLiteral_ptr number_literal)
{
	return make_shared<ObjectVariant>(number_literal->value);
}

ObjectVariant_ptr Interpreter::visit(BooleanLiteral_ptr bool_literal)
{
	return make_shared<ObjectVariant>(bool_literal->value);
}

ObjectVariant_ptr Interpreter::visit(VectorLiteral_ptr vector_literal)
{
	auto vector_variant = make_shared<ObjectVariant>(VectorObject());
	auto vector_object = get<VectorObject>(*vector_variant);

	for (const auto expression : vector_literal->expressions)
	{
		auto result = expression->interpret(*this);
		vector_object.add(result);
	}

	return move(vector_variant);
}

ObjectVariant_ptr Interpreter::visit(UDTLiteral_ptr udt_literal)
{
	auto UDT_variant = make_shared<ObjectVariant>(UDTObject());
	auto UDT_object = get<UDTObject>(*UDT_variant);

	for (auto const& [key, value_expr] : udt_literal->pairs)
	{
		auto value_object = value_expr->interpret(*this);
		UDT_object.add(key, value_object);
	}

	return move(UDT_variant);
}

ObjectVariant_ptr Interpreter::visit(Identifier_ptr expression)
{
	auto name = expression->name;
	auto info = env->get_variable(name);

	return move(info.value);
}

ObjectVariant_ptr Interpreter::visit(Unary_ptr unary_expression)
{
	auto operand = unary_expression->operand->interpret(*this);
	auto token_type = unary_expression->op->type;

	return std::visit(overloaded{
		[=](double number) { return perform_operation(token_type, number); },
		[=](bool boolean) { return perform_operation(token_type, boolean); },

		[](auto) {
			FATAL("The Unary Operation is not defined for this operand");
			return VOID;
			}
		}, *operand);
}

ObjectVariant_ptr Interpreter::visit(Binary_ptr binary_expression)
{
	auto left_variant = binary_expression->left->interpret(*this);
	auto right_variant = binary_expression->right->interpret(*this);
	auto token_type = binary_expression->op->type;

	return std::visit(overloaded{
		[=](bool left, bool right) { return perform_operation(token_type, left, right); },
		[=](double left, double right) { return perform_operation(token_type, left, right); },
		[=](string left, string right) { return perform_operation(token_type, left, right); },
		[=](string left, double right) { return perform_operation(token_type, left, right); },

		[](auto, auto) {
			FATAL("The Binary Operation is not defined for these operands");
			return VOID;
		}
		}, *left_variant, *right_variant);
}

ObjectVariant_ptr Interpreter::visit(VectorMemberAccess_ptr access_expression)
{
	auto name = access_expression->name;
	auto info = env->get_variable(name);

	FATAL_IF_FALSE(holds_alternative<VectorObject>(*info.value), name + " is not a vector");
	auto vector_object = get<VectorObject>(*info.value);

	// Get Index
	auto index_variant = access_expression->expression->interpret(*this);

	FATAL_IF_FALSE(holds_alternative<double>(*index_variant), "Vector elements must be accessed by a numeric index");
	double index = get<double>(*index_variant);

	return move(vector_object.values[index]);
}

ObjectVariant_ptr Interpreter::visit(UDTMemberAccess_ptr expression)
{
	auto UDT_name = expression->UDT_name;
	auto info = env->get_variable(UDT_name);

	FATAL_IF_FALSE(holds_alternative<UDTObject>(*info.value), UDT_name + " value is not a UDT");
	auto UDT_object = get<UDTObject>(*info.value);

	return move(UDT_object.pairs[expression->member_name]);
}

ObjectVariant_ptr Interpreter::visit(EnumMemberAccess_ptr expression)
{
	auto enum_name = expression->enum_name;
	auto info = env->get_enum(enum_name);

	return make_shared<ObjectVariant>(EnumObject(enum_name, expression->member_name));
}

ObjectVariant_ptr Interpreter::visit(FunctionCall_ptr call_expression)
{
	env->enter_function_scope();

	//if (expression->name == "echo" || expression->name == "ask")
	//{
	//	return call_builtin(expression->name, expression->arguments);
	//}

	auto info = env->get_function(call_expression->name);
	auto formal_arguments = info.arguments;

	FATAL_IF_FALSE(formal_arguments.size() == call_expression->arguments.size(),
		"Number of arguments in the function call " + call_expression->name + " is incorrect.");

	int index = 0;

	for (auto const& argument : call_expression->arguments)
	{
		auto formal_argument_name = formal_arguments[index].first;
		auto formal_argument_type = formal_arguments[index].second;

		auto object = argument->interpret(*this);

		FATAL_IF_FALSE(are_same_type(object, formal_argument_type),
			"The type of argument " + formal_argument_name + " in the function call " + call_expression->name + " is incorrect.");

		env->create_variable(
			formal_argument_name,
			false,
			true,
			formal_argument_type,
			move(object)
		);

		index++;
	}

	auto result = evaluate_block(move(info.body));
	env->leave_scope();
	return move(result);
}

ObjectVariant_ptr Interpreter::visit(Range_ptr expression)
{
	return VOID;
	//FATAL("Range must be used along with vector slicing or a FOR loop");
}

// Perform Operation

ObjectVariant_ptr Interpreter::perform_operation(WTokenType token_type, double operand)
{
	switch (token_type)
	{
	case WTokenType::UNARY_MINUS:
	{
		return make_shared<ObjectVariant>(-operand);
	}
	}

	FATAL("Operation not supported");
}

ObjectVariant_ptr Interpreter::perform_operation(WTokenType token_type, bool operand)
{
	switch (token_type)
	{
	case WTokenType::BANG:
	{
		return make_shared<ObjectVariant>(!operand);
	}
	}

	FATAL("Operation not supported");
}

ObjectVariant_ptr Interpreter::perform_operation(WTokenType token_type, bool left, bool right)
{
	switch (token_type)
	{
	case WTokenType::EQUAL_EQUAL:
	{
		return make_shared<ObjectVariant>(left == right);
	}
	case WTokenType::BANG_EQUAL:
	{
		return make_shared<ObjectVariant>(left != right);
	}
	case WTokenType::AND:
	{
		return make_shared<ObjectVariant>(left && right);
	}
	case WTokenType::OR:
	{
		return make_shared<ObjectVariant>(left || right);
	}
	}

	FATAL("Operation not supported");
}

ObjectVariant_ptr Interpreter::perform_operation(WTokenType token_type, double left, double right)
{
	switch (token_type)
	{
	case WTokenType::POWER:
	{
		return make_shared<ObjectVariant>(std::pow(left, right));
	}
	case WTokenType::DIVISION:
	{
		return make_shared<ObjectVariant>(left / right);
	}
	case WTokenType::REMINDER:
	{
		return make_shared<ObjectVariant>(std::remainder(left, right));
	}
	case WTokenType::STAR:
	{
		return make_shared<ObjectVariant>(left * right);
	}
	case WTokenType::PLUS:
	{
		return make_shared<ObjectVariant>(left + right);
	}
	case WTokenType::MINUS:
	{
		return make_shared<ObjectVariant>(left - right);
	}
	case WTokenType::GREATER_THAN:
	{
		return make_shared<ObjectVariant>(left > right);
	}
	case WTokenType::GREATER_THAN_EQUAL:
	{
		return make_shared<ObjectVariant>(left >= right);
	}
	case WTokenType::LESSER_THAN:
	{
		return make_shared<ObjectVariant>(left < right);
	}
	case WTokenType::LESSER_THAN_EQUAL:
	{
		return make_shared<ObjectVariant>(left <= right);
	}
	case WTokenType::EQUAL_EQUAL:
	{
		return make_shared<ObjectVariant>(left == right);
	}
	case WTokenType::BANG_EQUAL:
	{
		return make_shared<ObjectVariant>(left != right);
	}
	}

	FATAL("Operation not supported");
}

ObjectVariant_ptr Interpreter::perform_operation(WTokenType token_type, string left, string right)
{
	switch (token_type)
	{
	case WTokenType::PLUS:
	{
		return make_shared<ObjectVariant>(left + right);
	}
	case WTokenType::EQUAL_EQUAL:
	{
		return make_shared<ObjectVariant>(left == right);
	}
	case WTokenType::BANG_EQUAL:
	{
		return make_shared<ObjectVariant>(left != right);
	}
	}

	FATAL("Operation not supported");
}

ObjectVariant_ptr Interpreter::perform_operation(WTokenType token_type, string left, double right)
{
	switch (token_type)
	{
	case WTokenType::STAR:
	{
		int count = 0;
		auto repetitions = right;

		string result = "";

		while (count < repetitions)
		{
			result += left;
			count++;
		}

		return make_shared<ObjectVariant>(result);
	}
	}

	FATAL("Operation not supported");
}

// Utils

ObjectVariant_ptr Interpreter::evaluate_block(Block_ptr block)
{
	for (auto& statement : *block)
	{
		auto result = statement->interpret(*this);

		if (
			holds_alternative<ReturnObject>(*result) ||
			holds_alternative<ContinueObject>(*result) ||
			holds_alternative<BreakObject>(*result)
			) {
			return result;
		}
	}

	return VOID;
}

bool Interpreter::are_same_type(ObjectVariant_ptr obj, Type_ptr type)
{
	if (
		(holds_alternative<double>(*obj) && typeid(*type) == typeid(NumberType)) ||
		(holds_alternative<string>(*obj) && typeid(*type) == typeid(StringType)) ||
		(holds_alternative<bool>(*obj) && typeid(*type) == typeid(BooleanType)) ||
		(holds_alternative<VectorObject>(*obj) && typeid(*type) == typeid(VectorType)) ||
		(holds_alternative<UDTObject>(*obj) && typeid(*type) == typeid(UDTType))
		)
	{
		return true;
	}

	return false;
}