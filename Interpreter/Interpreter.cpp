#pragma once

#include "pch.h"
#include "Interpreter.h"
#include "Module.h"
#include "Types.h"
#include "ObjectSystem.h"
#include "Builtins.h"
#include "DispatchTables.h"
#include "CommonAssertion.h"

#include <memory>
#include <string>
#include <map>
#include <optional>
#include <variant>

#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define MAKE_TYPE(x) std::make_shared<Type>(x)
#define VOID std::make_shared<Object>(ReturnObject())
#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define THROW(message) return std::make_shared<Object>(ErrorObject(message))

#define THROW_ASSERT(condition, message)								\
	if (!condition) {													\
		spdlog::error(message);											\
		return std::make_shared<Object>(ErrorObject(message));	\
	}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::string;
using std::map;
using std::pair;
using std::vector;
using std::optional;
using std::make_shared;
using std::holds_alternative;

void Interpreter::execute(Module mod)
{
	for (auto statement : mod.nodes)
	{
		interpret(statement);
	}
}

Object_ptr Interpreter::interpret(Statement_ptr statement)
{
	return std::visit(overloaded{
		[&](Assignment stat) { return interpret(stat); },
		[&](MultipleAssignment stat) { return interpret(stat); },

		[&](ConditionalBranch stat) { return interpret(stat); },
		[&](IfLetBranch stat) { return interpret(stat); },

		[&](InfiniteLoop stat) { return interpret(stat); },
		[&](ForEachLoop stat) { return interpret(stat); },
		[&](Break stat) { return interpret(stat); },
		[&](Continue stat) { return interpret(stat); },

		[&](VariableDefinition stat) { return interpret(stat); },
		[&](UDTDefinition stat) { return interpret(stat); },
		[&](FunctionDefinition stat) { return interpret(stat); },
		[&](EnumDefinition stat) { return interpret(stat); },

		[&](ExpressionStatement stat) { return interpret(stat); },
		[&](Return stat) { return interpret(stat); },
		[&](ImportCustom stat) { return interpret(stat); },
		[&](ImportInBuilt stat) { return interpret(stat); },

		[](auto) { THROW("Never Seen this Statement before!"); }
		}, *statement);
}

Object_ptr Interpreter::interpret(Expression_ptr expression)
{
	return std::visit(overloaded{
			[&](std::string exp) { return interpret(exp); },
			[&](double exp) { return interpret(exp); },
			[&](bool exp) { return interpret(exp); },
			[&](VectorLiteral exp) { return interpret(exp); },
			[&](DictionaryLiteral exp) { return interpret(exp); },
			[&](Identifier exp) { return interpret(exp); },
			[&](EnumMember exp) { return interpret(exp); },
			[&](Unary exp) { return interpret(exp); },
			[&](Binary exp) { return interpret(exp); },
			[&](MemberAccess exp) { return interpret(exp); },
			[&](FunctionCall exp) { return interpret(exp); },

			[](auto) { THROW("Never Seen this Expression before!"); }
		}, *expression);
}

// Statement Evaluation

Object_ptr Interpreter::interpret(VariableDefinition declaration)
{
	auto result = interpret(declaration.expression);

	env->create_variable(
		declaration.name,
		declaration.is_public,
		declaration.is_mutable,
		declaration.type,
		move(result)
	);

	return VOID;
}

Object_ptr Interpreter::interpret(Assignment assignment)
{
	auto result = interpret(assignment.expression);

	env->set_variable(assignment.name, result);

	return VOID;
}

Object_ptr Interpreter::interpret(MultipleAssignment statement)
{
	ASSERT(
		statement.names.size() == statement.expressions.size(),
		"Mismatch in number of identifiers and RHS expressions"
	);

	int index = 0;
	for (auto name : statement.names)
	{
		auto expression = statement.expressions[index];
		index++;

		auto result = interpret(expression);
		env->set_variable(name, move(result));
	}

	return VOID;
}

Object_ptr Interpreter::interpret(ConditionalBranch branch)
{
	auto condition = interpret(branch.condition);
	THROW_ASSERT(holds_alternative<bool>(*condition), "Condition has to return bool");

	bool result = get<bool>(*condition);
	auto block = result ? branch.consequence : branch.alternative;

	env->enter_branch_scope();
	auto block_result = evaluate_block(move(block));
	env->leave_scope();

	return move(block_result);
}

Object_ptr Interpreter::interpret(IfLetBranch statement)
{
	return Object_ptr();
}

Object_ptr Interpreter::interpret(InfiniteLoop loop)
{
	env->enter_loop_scope();

	while (true)
	{
		auto result = evaluate_block(loop.block);

		if (holds_alternative<ReturnObject>(*result) || holds_alternative<ErrorObject>(*result))
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

Object_ptr Interpreter::interpret(ForEachLoop statement)
{
	auto iterable = interpret(statement.iterable);

	if (holds_alternative<string>(*iterable))
	{
		auto variable_name = get<string>(*iterable);
		auto variable_info = env->get_variable_info(variable_name);
		iterable = variable_info->value;
	}

	env->enter_loop_scope();

	env->create_variable(
		statement.item_name,
		false,
		true,
		statement.item_type,
		MAKE_OBJECT_VARIANT(NoneObject())
	);

	auto result = std::visit(overloaded{
		[&](ListObject& value)
		{ return loop_over_iterable(statement.item_name, statement.block, value); },
		[&](DictionaryObject& value)
		{ return loop_over_iterable(statement.item_name, statement.block, value); },

		[](auto) { THROW("Unable to loop over this object"); }
		}, *iterable);

	env->leave_scope();
	return result;
}

Object_ptr Interpreter::interpret(Break)
{
	if (env->is_inside_loop_scope())
		return MAKE_OBJECT_VARIANT(BreakObject());

	THROW("Break must be used within a loop");
}

Object_ptr Interpreter::interpret(Continue)
{
	if (env->is_inside_loop_scope())
		return MAKE_OBJECT_VARIANT(ContinueObject());

	THROW("Continue must be used within a loop");
}

Object_ptr Interpreter::interpret(Return statement)
{
	if (env->is_inside_function_scope())
	{
		if (statement.expression.has_value())
		{
			auto result = interpret(statement.expression.value);
			return MAKE_OBJECT_VARIANT(ReturnObject(result));
		}

		return VOID;
	}

	THROW("Return must be used inside a function");
}

Object_ptr Interpreter::interpret(ExpressionStatement statement)
{
	return interpret(statement.expression);
}

Object_ptr Interpreter::interpret(UDTDefinition def)
{
	env->create_UDT(def.name, def.is_public, def.member_types);
	return VOID;
}

Object_ptr Interpreter::interpret(FunctionDefinition def)
{
	string mangled_name = def.name;

	for (auto [_, type] : def.arguments)
	{
		mangled_name.append("_" + get_type_string(type));
	}

	env->create_function(
		def.name,
		mangled_name,
		def.is_public,
		def.arguments,
		def.return_type,
		def.body
	);

	return VOID;
}

Object_ptr Interpreter::interpret(EnumDefinition def)
{
	std::set<std::string> members_set(def.members.begin(), def.members.end());

	ASSERT(
		members_set.size() == def.members.size(),
		"Duplicate enum members are not allowed"
	);

	env->create_enum(def.name, def.is_public, members_set);

	return VOID;
}

Object_ptr Interpreter::interpret(ImportCustom statement)
{
	return VOID;
}

Object_ptr Interpreter::interpret(ImportInBuilt statement)
{
	std::string module_name = statement.module_name;

	for (auto const name : statement.goods)
	{
		auto function_visitor = get_inbuilt_function_visitor(module_name, name);
		env->import_builtin(name, function_visitor);
	}

	return VOID;
}

// Expression Evaluation

Object_ptr Interpreter::interpret(string string_literal)
{
	return MAKE_OBJECT_VARIANT(StringObject(string_literal));
}

Object_ptr Interpreter::interpret(double number_literal)
{
	return MAKE_OBJECT_VARIANT(NumberObject(number_literal));
}

Object_ptr Interpreter::interpret(bool bool_literal)
{
	return MAKE_OBJECT_VARIANT(BooleanObject(bool_literal));
}

Object_ptr Interpreter::interpret(VectorLiteral vector_literal)
{
	auto vector_object = ListObject();

	for (const auto expression : vector_literal.expressions)
	{
		auto result = interpret(expression);
		vector_object.append(result);
	}

	return MAKE_OBJECT_VARIANT(vector_object);
}

Object_ptr Interpreter::interpret(DictionaryLiteral dict_literal)
{
	auto dict_object = DictionaryObject();

	for (auto const& [key, value_expr] : dict_literal.pairs)
	{
		auto value_object = interpret(value_expr);
		dict_object.insert(MAKE_OBJECT_VARIANT(key->value), value_object);
	}

	return MAKE_OBJECT_VARIANT(dict_object);
}

Object_ptr Interpreter::interpret(Identifier expression)
{
	auto info = env->get_variable_info(expression.name);
	return move(info->value);
}

Object_ptr Interpreter::interpret(EnumMember expression)
{
	auto enum_info = env->get_enum_info(expression.enum_name);

	if (enum_info->members.contains(expression.member_name))
	{
		return MAKE_OBJECT_VARIANT(
			EnumMemberObject(expression.enum_name, expression.member_name)
		);
	}

	THROW("Enum " + expression.enum_name + " does not contain " + expression.member_name);
}

Object_ptr Interpreter::interpret(Unary unary_expression)
{
	auto operand = interpret(unary_expression.operand);
	auto token_type = unary_expression.op->type;

	return std::visit(overloaded{
		[&](double number) { return perform_operation(token_type, number); },
		[&](bool boolean) { return perform_operation(token_type, boolean); },

		[](auto) { THROW("The Unary Operation is not defined for this operand"); }
		}, *operand);
}

Object_ptr Interpreter::interpret(Binary binary_expression)
{
	auto left_variant = interpret(binary_expression.left);
	auto right_variant = interpret(binary_expression.right);
	auto token_type = binary_expression.op->type;

	return std::visit(overloaded{
		[&](bool left, bool right) { return perform_operation(token_type, left, right); },
		[&](double left, double right) { return perform_operation(token_type, left, right); },
		[&](string left, string right) { return perform_operation(token_type, left, right); },
		[&](string left, double right) { return perform_operation(token_type, left, right); },

		[](auto, auto) { THROW("The Binary Operation is not defined for these operands"); }
		}, *left_variant, *right_variant);
}

Object_ptr Interpreter::interpret(MemberAccess access_expression)
{
	auto accessor = interpret(access_expression.expression);

	auto name = access_expression.name;
	auto info = env->get_variable_info(name);

	return std::visit(overloaded{
		[&](ListType& type, ListObject& value) { return value.get(accessor); },
		[&](TupleType& type, TupleObject& value) { return value.get(accessor); },
		[&](UDTType& type, DictionaryObject& value) { return value.get(accessor); },
		[&](MapType& type, DictionaryObject& value) { return value.get(accessor); },

		[](auto, auto) { THROW("Unable to perform member access"); }
		}, *info->type, *info->value);
}

Object_ptr Interpreter::interpret(FunctionCall call_expression)
{
	if (auto info = env->get_inbuilt_function_info_if_exists(call_expression.name))
	{
		return evaluate_function_call(call_expression, info);
	}

	vector<Object_ptr> formal_arguments;

	for (auto const& argument : call_expression.arguments)
	{
		auto object = interpret(argument);
		formal_arguments.push_back(object);
	}

	string signature;

	auto info = env->get_info(signature);

	ASSERT(
		holds_alternative<FunctionInfo>(*info),
		signature + " is not a Function signature"
	);

	auto function_info = get_if<FunctionInfo>(&*info);
	return evaluate_function_call(call_expression, function_info, formal_arguments);
}

// Perform Operation

Object_ptr Interpreter::perform_operation(WTokenType token_type, double operand)
{
	switch (token_type)
	{
	case WTokenType::UNARY_MINUS:
	{
		return MAKE_OBJECT_VARIANT(NumberObject(-operand));
	}
	}

	THROW("Operation not supported");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, bool operand)
{
	switch (token_type)
	{
	case WTokenType::BANG:
	{
		return MAKE_OBJECT_VARIANT(NumberObject(!operand));
	}
	}

	THROW("Operation not supported");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, bool left, bool right)
{
	switch (token_type)
	{
	case WTokenType::EQUAL_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(BooleanObject(left == right));
	}
	case WTokenType::BANG_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(BooleanObject(left != right));
	}
	case WTokenType::AND:
	{
		return MAKE_OBJECT_VARIANT(BooleanObject(left && right));
	}
	case WTokenType::OR:
	{
		return MAKE_OBJECT_VARIANT(BooleanObject(left || right));
	}
	}

	THROW("Operation not supported");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, double left, double right)
{
	switch (token_type)
	{
	case WTokenType::POWER:
	{
		return MAKE_OBJECT_VARIANT(NumberObject(std::pow(left, right)));
	}
	case WTokenType::DIVISION:
	{
		return MAKE_OBJECT_VARIANT(NumberObject(left / right));
	}
	case WTokenType::REMINDER:
	{
		return MAKE_OBJECT_VARIANT(NumberObject(std::remainder(left, right)));
	}
	case WTokenType::STAR:
	{
		return MAKE_OBJECT_VARIANT(NumberObject(left * right));
	}
	case WTokenType::PLUS:
	{
		return MAKE_OBJECT_VARIANT(NumberObject(left + right));
	}
	case WTokenType::MINUS:
	{
		return MAKE_OBJECT_VARIANT(NumberObject(left - right));
	}
	case WTokenType::GREATER_THAN:
	{
		return MAKE_OBJECT_VARIANT(BooleanObject(left > right));
	}
	case WTokenType::GREATER_THAN_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(BooleanObject(left >= right));
	}
	case WTokenType::LESSER_THAN:
	{
		return MAKE_OBJECT_VARIANT(BooleanObject(left < right));
	}
	case WTokenType::LESSER_THAN_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(BooleanObject(left <= right));
	}
	case WTokenType::EQUAL_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(BooleanObject(left == right));
	}
	case WTokenType::BANG_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(BooleanObject(left != right));
	}
	}

	THROW("Operation not supported");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, string& left, string& right)
{
	switch (token_type)
	{
	case WTokenType::PLUS:
	{
		return MAKE_OBJECT_VARIANT(StringObject(left + right));
	}
	case WTokenType::EQUAL_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(BooleanObject(left == right));
	}
	case WTokenType::BANG_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(BooleanObject(left != right));
	}
	}

	THROW("Operation not supported");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, string& left, double right)
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

		return MAKE_OBJECT_VARIANT(StringObject(result));
	}
	}

	THROW("Operation not supported");
}

// Loop

Object_ptr Interpreter::loop_over_iterable(string item_name, Block block, ListObject& list_object)
{
	for (auto const& element : list_object.values)
	{
		env->set_variable(item_name, element);
		auto result = evaluate_block(block);

		if (holds_alternative<ReturnObject>(*result) || holds_alternative<ErrorObject>(*result))
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

	return VOID;
}

Object_ptr Interpreter::loop_over_iterable(string item_name, Block block, DictionaryObject& dict_object)
{
	for (auto const& [key, value] : dict_object.pairs)
	{
		NULL_CHECK(value);

		Object_ptr key_object = std::visit(overloaded{
			[](NumberObject num) { return MAKE_OBJECT_VARIANT(num); },
			[](StringObject text) { return MAKE_OBJECT_VARIANT(text); },
			[](BooleanObject boolean) { return MAKE_OBJECT_VARIANT(boolean); },

			[&](auto) { THROW("Cannot iterate over this datatype"); }
			}, *key);

		env->set_variable(
			item_name,
			MAKE_OBJECT_VARIANT(TupleObject({ key_object, value }))
		);

		auto result = evaluate_block(block);

		if (holds_alternative<ReturnObject>(*result) || holds_alternative<ErrorObject>(*result))
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

	return VOID;
}

// Evaluate function

Object_ptr Interpreter::evaluate_function_call(FunctionCall call_expression, FunctionInfo* info, vector<Object_ptr> formal_arguments)
{
	int index = 0;

	for (auto const& argument : info->arguments)
	{
		auto argument_name = argument.first;
		auto argument_type = argument.second;

		auto object = formal_arguments[index];

		THROW_ASSERT(
			are_same_type(object, argument_type),
			"The type of argument " + argument_name + " in the function call " +
			call_expression.name + " is incorrect"
		);

		env->create_variable(
			argument_name,
			false,
			true,
			argument_type,
			move(object)
		);

		index++;
	}

	auto result = evaluate_block(info->body);

	if (holds_alternative<ReturnObject>(*result))
	{
		return result;
	}

	FATAL("Function must return a ReturnObject");
}

Object_ptr Interpreter::evaluate_function_call(FunctionCall call_expression, InBuiltFunctionInfo* info)
{
	vector<Object_ptr> argument_objects;

	for (auto const& argument : call_expression.arguments)
	{
		auto result = interpret(argument);
		argument_objects.push_back(move(result));
	}

	return info->func(argument_objects);
}

// Utils

Object_ptr Interpreter::evaluate_block(Block block)
{
	for (auto& statement : block)
	{
		auto result = interpret(statement);

		if (
			holds_alternative<ReturnObject>(*result) ||
			holds_alternative<ErrorObject>(*result) ||
			holds_alternative<ContinueObject>(*result) ||
			holds_alternative<BreakObject>(*result)
			) {
			return result;
		}
	}

	return VOID;
}

bool Interpreter::are_same_type(Object_ptr obj, Type_ptr type)
{
	if (
		holds_alternative<NumberObject>(*obj) && holds_alternative<NumberType>(*type) ||
		holds_alternative<StringObject>(*obj) && holds_alternative<StringType>(*type) ||
		holds_alternative<BooleanObject>(*obj) && holds_alternative<BooleanType>(*type) ||
		holds_alternative<ListObject>(*obj) && holds_alternative<ListType>(*type) ||
		holds_alternative<TupleObject>(*obj) && holds_alternative<TupleType>(*type) ||
		holds_alternative<DictionaryObject>(*obj) && holds_alternative<MapType>(*type) ||
		holds_alternative<DictionaryObject>(*obj) && holds_alternative<UDTType>(*type) ||
		holds_alternative<EnumMemberObject>(*obj) && holds_alternative<EnumType>(*type) ||
		holds_alternative<AnyType>(*type)
		) {
		return true;
	}
	else if (holds_alternative<OptionalType>(*type))
	{
		return true;
	}
	else if (holds_alternative<VariantType>(*type))
	{
		return true;
	}

	return false;
}

Type_ptr Interpreter::get_object_type(Object_ptr object)
{
	return std::visit(overloaded{
			[&](NumberObject& obj) { return MAKE_TYPE(NumberType()); },
			[&](StringObject& obj) { return MAKE_TYPE(StringType()); },
			[&](BooleanObject& obj) { return MAKE_TYPE(BooleanType()); },

			[&](ListObject& obj) { return MAKE_TYPE(ListType(MAKE_TYPE(AnyType()))); },
			[&](TupleObject& obj) { return MAKE_TYPE(TupleType({ MAKE_TYPE(AnyType()) })); },
			[&](VariantObject& obj) { return MAKE_TYPE(VariantType({ MAKE_TYPE(AnyType()) })); },

			[&](EnumMemberObject& obj) { return MAKE_TYPE(EnumType(obj.enum_name)); },
			[&](DictionaryObject& obj) { return MAKE_TYPE(NumberType()); },

			[](auto) { return MAKE_TYPE(AnyType()); }
		}, *object);
}