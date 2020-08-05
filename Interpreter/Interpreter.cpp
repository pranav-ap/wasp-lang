#pragma once

#include "pch.h"
#include "Interpreter.h"
#include "ObjectSystem.h"
#include "CallStack.h"
#include "StackFrame.h"
#include "Assertion.h"

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
#define THROW_IF(condition, message) if (!condition) { return std::make_shared<Object>(ErrorObject(message)); }

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::wstring;
using std::map;
using std::pair;
using std::vector;
using std::optional;
using std::make_shared;
using std::holds_alternative;

void Interpreter::execute(Module_ptr ast)
{
	call_stack = std::make_shared<CallStack>();
	call_stack->push(std::make_shared<StackFrame>());

	for (auto statement : ast->statements)
	{
		interpret(statement);
	}

	ASSERT(call_stack->length() == 1, "Call stack must contain just one record");
	call_stack->pop();
}

Object_ptr Interpreter::interpret(Statement_ptr statement)
{
	return std::visit(overloaded{
		[&](Assignment& stat) { return interpret(stat); },
		[&](Branching& stat) { return interpret(stat); },
		[&](WhileLoop& stat) { return interpret(stat); },
		[&](ForInLoop& stat) { return interpret(stat); },
		[&](Break& stat) { return interpret(stat); },
		[&](Continue& stat) { return interpret(stat); },
		[&](Pass& stat) { return interpret(stat); },
		[&](Return& stat) { return interpret(stat); },
		[&](YieldStatement& stat) { return interpret(stat); },
		[&](VariableDefinition& stat) { return interpret(stat); },
		[&](UDTDefinition& stat) { return interpret(stat); },
		[&](AliasDefinition& stat) { return interpret(stat); },
		[&](FunctionDefinition& stat) { return interpret(stat); },
		[&](GeneratorDefinition& stat) { return interpret(stat); },
		[&](EnumDefinition& stat) { return interpret(stat); },
		[&](ImportCustom& stat) { return interpret(stat); },
		[&](ImportInBuilt& stat) { return interpret(stat); },
		[&](ExpressionStatement& stat) { return interpret(stat); },
		[&](AssertStatement& stat) { return interpret(stat); },

		[](auto) { THROW(L"Never seen this Statement before!"); }
		}, *statement);
}

Object_ptr Interpreter::interpret(Assignment& statement)
{
	ASSERT(
		statement.lhs_expressions.size() == statement.lhs_expressions.size(),
		"LHS and RHS are not equal"
	);

	const auto length = statement.lhs_expressions.size();

	for (size_t index = 0; index < length; index++)
	{
		auto lhs = statement.lhs_expressions[index];
		auto rhs = statement.rhs_expressions[index];

		auto lhs_value = interpret(lhs);
		auto rhs_value = interpret(rhs);

		ASSERT(holds_alternative<Identifier>(*lhs_value), "RHS has to be an identifier");
		auto rhs_identifier = get<Identifier>(*lhs_value);

		call_stack->peek()->set(rhs_identifier->name, rhs_value);
	}

	return VOID;
}

Object_ptr Interpreter::interpret(Branching& statement)
{
	for (auto branch : statement.branches)
	{
		auto value = interpret(branch.first);
		ASSERT(holds_alternative<BooleanObject>(*value), "Condition must return a boolean");
		auto boolean_value = get<BooleanObject>(*value);

		if (boolean_value.value)
		{
			auto result = evaluate_block(*branch.second);
			return result;
		}
	}

	return VOID;
}

Object_ptr Interpreter::interpret(WhileLoop& statement)
{
	while (true)
	{
		auto result = interpret(statement.condition);
		ASSERT(holds_alternative<BooleanObject>(*result), "Condition must return a boolean");
		auto boolean_value = get<BooleanObject>(*result);

		result = evaluate_block(statement);

		if (holds_alternative<ReturnObject>(*result) || holds_alternative<ErrorObject>(*result))
		{
			call_stack->pop();
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

Object_ptr Interpreter::interpret(ForInLoop& statement)
{
	return VOID;
}

Object_ptr Interpreter::interpret(Break& statement)
{
	return MAKE_OBJECT_VARIANT(BreakObject());
}

Object_ptr Interpreter::interpret(Continue& statement)
{
	return MAKE_OBJECT_VARIANT(ContinueObject());
}

Object_ptr Interpreter::interpret(Pass& statement)
{
	return VOID;
}

Object_ptr Interpreter::interpret(Return& statement)
{
	if (statement.expression.has_value())
	{
		auto result = interpret(statement.expression.value());
		return result;
	}

	return VOID;
}

Object_ptr Interpreter::interpret(YieldStatement& statement)
{
	if (statement.expression.has_value())
	{
		auto result = interpret(statement.expression.value());
		return result;
	}

	return VOID;
}

Object_ptr Interpreter::interpret(VariableDefinition& statement)
{
	auto result = interpret(statement.expression);
	return VOID;
}

Object_ptr Interpreter::interpret(UDTDefinition& statement)
{
	return VOID;
}

Object_ptr Interpreter::interpret(AliasDefinition& statement)
{
	return VOID;
}

Object_ptr Interpreter::interpret(FunctionDefinition& statement)
{
	return VOID;
}

Object_ptr Interpreter::interpret(GeneratorDefinition& statement)
{
	return VOID;
}

Object_ptr Interpreter::interpret(EnumDefinition& statement)
{
	return VOID;
}

Object_ptr Interpreter::interpret(ImportCustom& statement)
{
	return VOID;
}

Object_ptr Interpreter::interpret(ImportInBuilt& statement)
{
	return VOID;
}

Object_ptr Interpreter::interpret(ExpressionStatement& statement)
{
	auto result = interpret(statement.expression);
	return result;
}

Object_ptr Interpreter::interpret(AssertStatement& statement)
{
	auto result = interpret(statement.expression);
	ASSERT(holds_alternative<BooleanObject>(*result), "Assert expression must return boolean");
	return VOID;
}

Object_ptr Interpreter::interpret(Expression_ptr expression)
{
	return std::visit(overloaded{
		[&](std::wstring exp) { return interpret(exp); },
		[&](double exp) { return interpret(exp); },
		[&](bool exp) { return interpret(exp); },
		[&](ListLiteral& exp) { return interpret(exp); },
		[&](TupleLiteral& exp) { return interpret(exp); },
		[&](MapLiteral& exp) { return interpret(exp); },
		[&](UDTConstruct& exp) { return interpret(exp); },
		[&](UDTMemberAccess& exp) { return interpret(exp); },
		[&](EnumMember& exp) { return interpret(exp); },
		[&](Identifier& exp) { return interpret(exp); },
		[&](Call& exp) { return interpret(exp); },
		[&](Unary& exp) { return interpret(exp); },
		[&](Binary& exp) { return interpret(exp); },

		[](auto) { THROW(L"Never seen this Expression before!"); }
		}, *expression);
}

Object_ptr Interpreter::interpret(double expression)
{
	return MAKE_OBJECT_VARIANT(NumberObject(expression));
}

Object_ptr Interpreter::interpret(std::wstring expression)
{
	return MAKE_OBJECT_VARIANT(StringObject(expression));
}

Object_ptr Interpreter::interpret(bool expression)
{
	return MAKE_OBJECT_VARIANT(BooleanObject(expression));
}

Object_ptr Interpreter::interpret(ListLiteral& list_literal)
{
	auto list_object = ListObject();

	for (const auto expression : list_literal.expressions)
	{
		auto result = interpret(expression);
		list_object.append(result);
	}

	return MAKE_OBJECT_VARIANT(list_object);
}

Object_ptr Interpreter::interpret(TupleLiteral& tuple_literal)
{
	std::vector<Object_ptr> values;

	for (const auto expression : tuple_literal.expressions)
	{
		auto result = interpret(expression);
		values.push_back(result);
	}

	auto tuple_object = TupleObject(values);
	return MAKE_OBJECT_VARIANT(tuple_object);
}

Object_ptr Interpreter::interpret(MapLiteral& map_literal)
{
	auto map_object = MapObject();

	for (auto const& [key, value_expr] : map_literal.pairs)
	{
		auto value_object = interpret(value_expr);

		//Object_ptr key_object = std::visit(overloaded{
		//	[&](std::wstring exp) { return interpret(exp); },
		//	[&](double exp) { return interpret(exp); },
		//	[&](bool exp) { return interpret(exp); },
		//	[&](EnumMember& exp) { return interpret(exp); },
		//	[&](Identifier& exp) { return interpret(exp); },
		//	[](auto) { THROW(L"Not a valid key type"); }
		//	}, *key);

		//map_object.insert(MAKE_OBJECT_VARIANT(key_object), value_object);
	}

	return MAKE_OBJECT_VARIANT(map_object);
}

Object_ptr Interpreter::interpret(UDTConstruct& expr)
{
	return VOID;
}

Object_ptr Interpreter::interpret(UDTMemberAccess& expr)
{
	return VOID;
}

Object_ptr Interpreter::interpret(EnumMember& expr)
{
	return VOID;
}

Object_ptr Interpreter::interpret(Identifier& expr)
{
	return VOID;
}

Object_ptr Interpreter::interpret(Call& expr)
{
	return VOID;
}

Object_ptr Interpreter::interpret(Unary& expr)
{
	auto operand = interpret(expr.operand);
	auto token_type = expr.op->type;

	return std::visit(overloaded{
		[&](double number) { return perform_operation(token_type, number); },
		[&](bool boolean) { return perform_operation(token_type, boolean); },

		[](auto) { THROW(L"The Unary Operation is not defined for this operand"); }
		}, *operand);
}

Object_ptr Interpreter::interpret(Binary& binary_expression)
{
	auto left_variant = interpret(binary_expression.left);
	auto right_variant = interpret(binary_expression.right);
	auto token_type = binary_expression.op->type;

	return std::visit(overloaded{
		[&](bool left, bool right) { return perform_operation(token_type, left, right); },
		[&](double left, double right) { return perform_operation(token_type, left, right); },
		[&](wstring left, wstring right) { return perform_operation(token_type, left, right); },
		[&](wstring left, double right) { return perform_operation(token_type, left, right); },

		[](auto, auto) { THROW(L"The Binary Operation is not defined for these operands"); }
		}, *left_variant, *right_variant);
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, double operand)
{
	switch (token_type)
	{
	case WTokenType::UNARY_MINUS:
	{
		return MAKE_OBJECT_VARIANT(NumberObject(-operand));
	}
	default:
	{
		THROW(L"Operation not supported");
	}
	}
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, bool operand)
{
	switch (token_type)
	{
	case WTokenType::BANG:
	{
		return MAKE_OBJECT_VARIANT(NumberObject(!operand));
	}
	default:
	{
		THROW(L"Operation not supported");
	}
	}
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
	default:
	{
		THROW(L"Operation not supported");
	}
	}
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
	default:
	{
		THROW(L"Operation not supported");
	}
	}
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, std::wstring& left, std::wstring& right)
{
	return VOID;
}

Object_ptr Interpreter::perform_operation(WTokenType token_type, std::wstring& left, double right)
{
	return VOID;
}

Object_ptr Interpreter::evaluate_block(ScopedBlock& block)
{
	return VOID;
}

Object_ptr Interpreter::loop_over_iterable(std::wstring item_name, ScopedBlock& block, ListObject& vector_object)
{
	return VOID;
}

Object_ptr Interpreter::loop_over_iterable(std::wstring pair_name, ScopedBlock& block, MapLiteral& map_object)
{
	return VOID;
}

Object_ptr Interpreter::evaluate_function_call(Call& call_expression)
{
	vector<Object_ptr> formal_arguments;

	for (auto const& argument : call_expression.arguments)
	{
		auto object = interpret(argument);
		formal_arguments.push_back(object);
	}

	return VOID;
}