#pragma once
#include "pch.h"
#include "SemanticAnalyzer.h"
#include "Symbol.h"
#include "Assertion.h"
#include <variant>
#include <vector>
#include <memory>
#include <algorithm>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_SYMBOL(a, b, c, d, e) std::make_shared<Symbol>(a, b, c, d, e)
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::holds_alternative;
using std::wstring;
using std::get_if;
using std::begin;
using std::end;
using std::vector;
using std::make_shared;
using std::move;

Object_ptr SemanticAnalyzer::visit(const Expression_ptr expression)
{
	Object_ptr type = std::visit(overloaded{
		[&](int expr) { return visit(expr); },
		[&](double expr) { return visit(expr); },
		[&](std::wstring expr) { return visit(expr); },
		[&](bool expr) { return visit(expr); },
		[&](ListLiteral& expr) { return visit(expr); },
		[&](TupleLiteral& expr) {return  visit(expr); },
		[&](MapLiteral& expr) { return visit(expr); },
		[&](SetLiteral& expr) { return visit(expr); },
		[&](TernaryCondition& expr) { return visit(expr); },
		[&](Identifier& expr) { return visit(expr); },
		[&](Prefix& expr) { return visit(expr); },
		[&](Infix& expr) { return visit(expr); },
		[&](Postfix& expr) { return visit(expr); },
		[&](TypePattern& expr) { return visit(expr); },
		[&](UntypedAssignment& expr) { return visit(expr); },
		[&](TypedAssignment& expr) { return visit(expr); },
		[&](Call& expr) { return visit(expr); },
		[&](EnumMember& expr) { return visit(expr); },

		[&](auto)
		{
			FATAL("Never Seen this Expression before!");
			return type_system->type_pool->get_none_type();
		}
		}, *expression);

	return type;
}

ObjectVector SemanticAnalyzer::visit(ExpressionVector expressions)
{
	ObjectVector types;

	for (auto& argument : expressions)
	{
		Object_ptr type = visit(argument);
		types.push_back(type);
	}

	return types;
}

Object_ptr SemanticAnalyzer::visit(const int expr)
{
	return type_system->type_pool->get_int_type();
}

Object_ptr SemanticAnalyzer::visit(const double expr)
{
	return type_system->type_pool->get_float_type();
}

Object_ptr SemanticAnalyzer::visit(const std::wstring expr)
{
	return type_system->type_pool->get_string_type();
}

Object_ptr SemanticAnalyzer::visit(const bool expr)
{
	return type_system->type_pool->get_boolean_type();
}

Object_ptr SemanticAnalyzer::visit(ListLiteral& expr)
{
	ObjectVector types = visit(expr.expressions);
	types = remove_duplicates(types);

	if (types.size() == 1)
	{
		Object_ptr list_type = MAKE_OBJECT_VARIANT(ListType(types.front()));
		return list_type;
	}

	Object_ptr variant_type = MAKE_OBJECT_VARIANT(VariantType(types));
	Object_ptr list_type = MAKE_OBJECT_VARIANT(ListType(variant_type));
	return list_type;
}

Object_ptr SemanticAnalyzer::visit(TupleLiteral& expr)
{
	ObjectVector types = visit(expr.expressions);
	Object_ptr tuple_type = MAKE_OBJECT_VARIANT(TupleType(types));
	return tuple_type;
}

Object_ptr SemanticAnalyzer::visit(SetLiteral& expr)
{
	ObjectVector types = visit(expr.expressions);
	types = remove_duplicates(types);

	if (types.size() == 1)
	{
		Object_ptr set_type = MAKE_OBJECT_VARIANT(SetType(types.front()));
		return set_type;
	}

	Object_ptr variant_type = MAKE_OBJECT_VARIANT(VariantType(types));
	Object_ptr set_type = MAKE_OBJECT_VARIANT(SetType(variant_type));
	return set_type;
}

Object_ptr SemanticAnalyzer::visit(MapLiteral& expr)
{
	ObjectVector key_types;
	ObjectVector value_types;

	for (const auto x : expr.pairs)
	{
		Object_ptr key_type = visit(x.first);
		type_system->expect_key_type(current_scope, key_type);
		key_types.push_back(move(key_type));

		Object_ptr value_type = visit(x.second);
		value_types.push_back(move(value_type));
	}

	key_types = remove_duplicates(key_types);
	value_types = remove_duplicates(value_types);

	Object_ptr key_type = (key_types.size() == 1) ? key_types.front() : MAKE_OBJECT_VARIANT(VariantType(key_types));
	Object_ptr value_type = (value_types.size() == 1) ? value_types.front() : MAKE_OBJECT_VARIANT(VariantType(value_types));

	Object_ptr map_type = MAKE_OBJECT_VARIANT(MapType(key_type, value_type));
	return map_type;
}

Object_ptr SemanticAnalyzer::visit(TernaryCondition& expression)
{
	enter_scope(ScopeType::EXPRESSION);
	expression.scope = current_scope;

	Object_ptr condition_type = visit(expression.condition);
	type_system->expect_condition_type(current_scope, condition_type);

	Object_ptr true_type = visit(expression.true_expression);
	Object_ptr false_type = visit(expression.false_expression);

	if (type_system->equal(current_scope, true_type, false_type))
	{
		return true_type;
	}

	return MAKE_OBJECT_VARIANT(VariantType({ true_type, false_type }));
}

Object_ptr SemanticAnalyzer::visit(TypePattern& expr)
{
	FATAL("TypePattern must be handled by parent nodes");
	return type_system->type_pool->get_none_type();
}

Object_ptr SemanticAnalyzer::visit(UntypedAssignment& expression)
{
	Object_ptr lhs_type = visit(expression.lhs_expression);
	Object_ptr rhs_type = visit(expression.rhs_expression);
	ASSERT(type_system->assignable(current_scope, lhs_type, rhs_type), "TypeNode mismatch in assignment");

	return lhs_type;
}

Object_ptr SemanticAnalyzer::visit(TypedAssignment& expression)
{
	Object_ptr type = visit(expression.type_node);
	Object_ptr rhs_type = visit(expression.rhs_expression);
	ASSERT(type_system->assignable(current_scope, type, rhs_type), "Type mismatch in assignment");

	return type;
}

Object_ptr SemanticAnalyzer::visit(Prefix& expr)
{
	Object_ptr operand_type = visit(expr.operand);
	Object_ptr result_type = type_system->infer(current_scope, operand_type, expr.op->type);
	return result_type;
}

Object_ptr SemanticAnalyzer::visit(Postfix& expr)
{
	Object_ptr operand_type = visit(expr.operand);
	return type_system->type_pool->get_none_type();
}

Object_ptr SemanticAnalyzer::visit(Infix& expr)
{
	Object_ptr lhs_operand_type = visit(expr.left);
	Object_ptr rhs_operand_type = visit(expr.right);

	Object_ptr result_type = type_system->infer(current_scope, lhs_operand_type, expr.op->type, rhs_operand_type);
	return result_type;
}

Object_ptr SemanticAnalyzer::visit(EnumMember& expr)
{
	Symbol_ptr symbol = current_scope->lookup(expr.chain.front());
	NULL_CHECK(symbol);

	auto enum_type = type_system->extract_enum_type(symbol->type);
	ASSERT(enum_type->members.contains(expr.chain_str), "Enum does not contain this member");

	return symbol->type;
}

Object_ptr SemanticAnalyzer::visit(Identifier& expr)
{
	Symbol_ptr symbol = current_scope->lookup(expr.name);
	return symbol->type;
}

Object_ptr SemanticAnalyzer::visit(Call& expr)
{
	Symbol_ptr symbol = current_scope->lookup(expr.name);
	expr.is_builtin = symbol->is_builtin;

	ObjectVector argument_types = visit(expr.arguments);

	Object_ptr return_type = std::visit(overloaded{
		[&](FunctionType const& type)
		{
			ASSERT(type_system->equal(current_scope, argument_types, type.input_types), "Argument mismatch in call");
			return type.return_type.value_or(type_system->type_pool->get_none_type());
		},
		[&](auto)
		{
			FATAL("Not a callable type");
			return type_system->type_pool->get_none_type();
		}
		}, *symbol->type);

	return return_type;
}

Object_ptr SemanticAnalyzer::visit(Spread& expr)
{
	Object_ptr operand_type = visit(expr.expression);

	if (current_scope->is_rvalue)
	{
		expr.is_rvalue = true;
		type_system->expect_spreadable_type(current_scope, operand_type);
		operand_type = type_system->spread_type(operand_type);
		return operand_type;
	}

	expr.is_rvalue = false;
	return operand_type;
}