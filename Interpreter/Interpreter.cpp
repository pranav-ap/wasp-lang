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
#include <algorithm>

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
using std::to_string;
using std::map;
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
			[&](Unary exp) { return interpret(exp); },
			[&](Binary exp) { return interpret(exp); },
			[&](MemberAccess exp) { return interpret(exp); },
			[&](FunctionCall exp) { return interpret(exp); },

			[](auto) { THROW("Never Seen this Expression before!"); }
		}, *expression);
}

// Statement Visitors

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
	auto result = interpret(assignment);

	auto info = env->get_variable_info(assignment.name);
	ASSERT(info->is_mutable, assignment.name + " is not mutable");
	info->value = move(result);

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
	env->enter_loop_scope();
	env->create_variable(
		statement.item_name,
		false,
		true,
		MAKE_TYPE(NumberType()),
		VOID
	);

	auto iterable = interpret(statement.iterable);

	if (holds_alternative<string>(*iterable))
	{
		auto identifier_name = get<string>(*iterable);
		auto info = env->get_variable_info(identifier_name);
		iterable = info->value;
		NULL_CHECK(iterable);
	}

	env->leave_scope();
	return VOID;
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
			auto result = interpret(statement.expression.value());
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
	env->create_function(
		def.name,
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

// Expression Visitors

Object_ptr Interpreter::interpret(std::string string_literal)
{
	return MAKE_OBJECT_VARIANT(string_literal);
}

Object_ptr Interpreter::interpret(double number_literal)
{
	return MAKE_OBJECT_VARIANT(number_literal);
}

Object_ptr Interpreter::interpret(bool bool_literal)
{
	return MAKE_OBJECT_VARIANT(bool_literal);
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
		[&](ListType& type, ListObject& value)
		{
			return value.get(accessor);
		},
		[&](TupleType& type, TupleObject& value)
		{
			return value.get(accessor);
		},
		[&](UDTType& type, DictionaryObject& value)
		{
			return value.get(accessor);
		},
		[&](MapType& type, DictionaryObject& value)
		{
			return value.get(accessor);
		},
		[&](EnumType& type, EnumMemberObject& value)
		{
			auto enum_info = env->get_enum_info(value.enum_name);

			auto x = enum_info->members;
			auto itr = std::find(x.begin(), x.end(), value.member_name);

			if (itr != x.cend())
			{
				return MAKE_OBJECT_VARIANT(std::distance(x.begin(), itr));
			}
		},

		[](auto, auto) { THROW("Unable to perform member access"); }
		}, *info->type, *info->value);
}

Object_ptr Interpreter::interpret(FunctionCall call_expression)
{
	auto info_variant = env->get_info(call_expression.name);

	return std::visit(overloaded{
		[&](FunctionInfo info)
		{
			env->enter_function_scope();
			auto result = evaluate_function_call(call_expression, info);
			env->leave_scope();
			return move(result);
		},
		[&](InBuiltFunctionInfo info)
		{
			return evaluate_function_call(call_expression, info);
		},

		[](auto) { THROW("It is neither a function nor a builtin function!"); }
		}, *info_variant);
}

// Perform Operation

Object_ptr Interpreter::perform_operation(WTokenType token_type, double operand)
{
	switch (token_type)
	{
	case WTokenType::UNARY_MINUS:
	{
		return MAKE_OBJECT_VARIANT(-operand);
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
		return MAKE_OBJECT_VARIANT(!operand);
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
		return MAKE_OBJECT_VARIANT(left == right);
	}
	case WTokenType::BANG_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(left != right);
	}
	case WTokenType::AND:
	{
		return MAKE_OBJECT_VARIANT(left && right);
	}
	case WTokenType::OR:
	{
		return MAKE_OBJECT_VARIANT(left || right);
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
		return MAKE_OBJECT_VARIANT(std::pow(left, right));
	}
	case WTokenType::DIVISION:
	{
		return MAKE_OBJECT_VARIANT(left / right);
	}
	case WTokenType::REMINDER:
	{
		return MAKE_OBJECT_VARIANT(std::remainder(left, right));
	}
	case WTokenType::STAR:
	{
		return MAKE_OBJECT_VARIANT(left * right);
	}
	case WTokenType::PLUS:
	{
		return MAKE_OBJECT_VARIANT(left + right);
	}
	case WTokenType::MINUS:
	{
		return MAKE_OBJECT_VARIANT(left - right);
	}
	case WTokenType::GREATER_THAN:
	{
		return MAKE_OBJECT_VARIANT(left > right);
	}
	case WTokenType::GREATER_THAN_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(left >= right);
	}
	case WTokenType::LESSER_THAN:
	{
		return MAKE_OBJECT_VARIANT(left < right);
	}
	case WTokenType::LESSER_THAN_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(left <= right);
	}
	case WTokenType::EQUAL_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(left == right);
	}
	case WTokenType::BANG_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(left != right);
	}
	}

	THROW("Operation not supported");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, string left, string right)
{
	switch (token_type)
	{
	case WTokenType::PLUS:
	{
		return MAKE_OBJECT_VARIANT(left + right);
	}
	case WTokenType::EQUAL_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(left == right);
	}
	case WTokenType::BANG_EQUAL:
	{
		return MAKE_OBJECT_VARIANT(left != right);
	}
	}

	THROW("Operation not supported");
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, string left, double right)
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

		return MAKE_OBJECT_VARIANT(result);
	}
	}

	THROW("Operation not supported");
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

Object_ptr Interpreter::loop_over_vector(string item_name, ListObject& vector_object, Type_ptr type, Block block)
{
	for (auto const& element : vector_object.values)
	{
		THROW_ASSERT(are_same_type(element, type), "Element has incorrect type");
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

Object_ptr Interpreter::loop_over_map(std::string pair_name, DictionaryObject& map_object, Type_ptr key_type, Type_ptr value_type, Block block)
{
	for (auto const& [key, value] : map_object.pairs)
	{
		//THROW_ASSERT(are_same_type(key, key_type), "Key has incorrect type");
		THROW_ASSERT(are_same_type(value, value_type), "Value has incorrect type");
		env->set_variable(pair_name, MAKE_OBJECT_VARIANT(key, value));

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

Object_ptr Interpreter::evaluate_function_call(FunctionCall call_expression, FunctionInfo info)
{
	auto formal_arguments = info.arguments;

	THROW_ASSERT(
		formal_arguments.size() == call_expression.arguments.size(),
		"Number of arguments in the function call " + call_expression->name + " is incorrect."
	);

	int index = 0;

	for (auto const& argument : call_expression.arguments)
	{
		auto formal_argument_name = formal_arguments[index].first;
		auto formal_argument_type = formal_arguments[index].second;

		auto object = interpret(argument);

		THROW_ASSERT(
			are_same_type(object, formal_argument_type),
			"The type of argument " + formal_argument_name + " in the function call " +
			call_expression.name + " is incorrect"
		);

		env->create_variable(
			formal_argument_name,
			false,
			true,
			formal_argument_type,
			move(object)
		);

		index++;
	}

	return evaluate_block(move(info.body));
}

Object_ptr Interpreter::evaluate_function_call(FunctionCall call_expression, InBuiltFunctionInfo info)
{
	vector<Object_ptr> argument_objects;

	for (auto const& argument : call_expression.arguments)
	{
		auto result = interpret(argument);
		argument_objects.push_back(move(result));
	}

	return info.func(argument_objects);
}

bool Interpreter::are_same_type(Object_ptr obj, Type_ptr type)
{
	return (
		holds_alternative<double>(*obj) && holds_alternative<NumberType>(*type) ||
		holds_alternative<string>(*obj) && holds_alternative<StringType>(*type) ||
		holds_alternative<bool>(*obj) && holds_alternative<BooleanType>(*type) ||
		holds_alternative<ListObject>(*obj) && holds_alternative<ListType>(*type) ||
		holds_alternative<UDTObject>(*obj) && holds_alternative<UDTType>(*type)
		);
}