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

#define PUBLIC_SYMBOL true
#define PRIVATE_SYMBOL false
#define MUTABLE_SYMBOL true
#define CONST_SYMBOL false

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


Object_ptr SemanticAnalyzer::visit(const ScopedExpression_ptr expression)
{
	enter_scope(ScopeType::CONDITIONAL);
	auto result = visit(expression->expression);
	leave_scope();
	return result;
}

Object_ptr SemanticAnalyzer::visit(const Expression_ptr expression)
{
	Object_ptr type = std::visit(overloaded{
		[&](int expr) { return visit(expr); },
		[&](double expr) { return visit(expr); },
		[&](std::wstring expr) { return visit(expr); },
		[&](bool expr) { return visit(expr); },
		[&](ListLiteral const& expr) { return visit(expr); },
		[&](TupleLiteral const& expr) {return  visit(expr); },
		[&](MapLiteral const& expr) { return visit(expr); },
		[&](SetLiteral const& expr) { return visit(expr); },
		[&](NewObject const& expr) { return visit(expr); },
		[&](TernaryCondition const& expr) { return visit(expr); },
		[&](EnumMember const& expr) { return visit(expr); },
		[&](MemberAccess const& expr) { return visit(expr); },
		[&](Identifier const& expr) { return visit(expr); },
		[&](Prefix const& expr) { return visit(expr); },
		[&](Infix const& expr) { return visit(expr); },
		[&](Postfix const& expr) { return visit(expr); },
		[&](Call const& expr) { return visit(expr); },
		[&](TypePattern const& expr) { return visit(expr); },
		[&](Assignment const& expr) { return visit(expr); },
		[&](Spread const& expr) { return visit(expr); },

		[&](auto)
		{
			FATAL("Never Seen this Expression before!");
			return type_system->type_pool->get_none_type();
		}
		}, *expression);

	return type;
}

ObjectVector SemanticAnalyzer::visit(ExpressionVector const& expressions)
{
	ObjectVector types;

	for (auto const& argument : expressions)
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

Object_ptr SemanticAnalyzer::visit(ListLiteral const& expr)
{
	ObjectVector types = visit(expr.expressions);

	if (types.size() == 1)
	{
		Object_ptr list_type = MAKE_OBJECT_VARIANT(ListType(types.front()));
		return list_type;
	}

	Object_ptr variant_type = MAKE_OBJECT_VARIANT(VariantType(types));
	Object_ptr list_type = MAKE_OBJECT_VARIANT(ListType(variant_type));
	return list_type;
}

Object_ptr SemanticAnalyzer::visit(TupleLiteral const& expr)
{
	ObjectVector types = visit(expr.expressions);
	Object_ptr tuple_type = MAKE_OBJECT_VARIANT(TupleType(types));
	return tuple_type;
}

Object_ptr SemanticAnalyzer::visit(SetLiteral const& expr)
{
	ObjectVector types = visit(expr.expressions);
	Object_ptr set_type = MAKE_OBJECT_VARIANT(SetType(types));
	return set_type;
}

Object_ptr SemanticAnalyzer::visit(MapLiteral const& expr)
{
	ObjectVector key_types;
	ObjectVector value_types;

	for (const auto x : expr.pairs)
	{
		Object_ptr key_type = visit(x.first);
		type_system->expect_key_type(current_scope, key_type);

		std::any_of(begin(key_types), end(key_types), [&](auto k) {
			return type_system->equal(current_scope, key_type, k);
			});

		key_types.push_back(move(key_type));

		Object_ptr value_type = visit(x.second);

		std::any_of(begin(value_types), end(value_types), [&](auto k) {
			return type_system->equal(current_scope, value_type, k);
			});


		value_types.push_back(move(value_type));
	}

	Object_ptr key_type = (key_types.size() == 1) ? key_types.front() : MAKE_OBJECT_VARIANT(VariantType(key_types));
	Object_ptr value_type = (value_types.size() == 1) ? value_types.front() : MAKE_OBJECT_VARIANT(VariantType(value_types));

	Object_ptr map_type = MAKE_OBJECT_VARIANT(MapType(key_type, value_type));
	return map_type;
}

Object_ptr SemanticAnalyzer::visit(Assignment const& expression)
{
	Object_ptr lhs_type = visit(expression.lhs_expression);
	Object_ptr rhs_type = visit(expression.rhs_expression);

	ASSERT(type_system->assignable(current_scope, lhs_type, rhs_type), "TypeNode mismatch in assignment");

	return rhs_type;
}

Object_ptr SemanticAnalyzer::visit(TernaryCondition const& expression)
{
	enter_scope(ScopeType::CONDITIONAL);

	expression.condition->scope = current_scope;
	expression.true_expression->scope = current_scope;
	expression.false_expression->scope = current_scope;

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

Object_ptr SemanticAnalyzer::visit(Spread const& expr)
{
	Object_ptr operand_type = visit(expr.expression);
	type_system->expect_spreadable_type(current_scope, operand_type);

	// Need to expand this type before returning
	return operand_type;
}

Object_ptr SemanticAnalyzer::visit(TypePattern const& expr)
{
	FATAL("TypePattern must be handled by parent nodes");
	return type_system->type_pool->get_none_type();
}

Object_ptr SemanticAnalyzer::visit(NewObject const& expr)
{
	ObjectVector types = visit(expr.expressions);

	auto symbol = current_scope->lookup(expr.type_name);
	auto class_type = type_system->extract_class_type(symbol->type);

	wstring constructor_name = L"constructor";
	auto exists = class_type->members.contains(constructor_name);
	ASSERT(exists, "Required constructor must exist");

	return symbol->type;
}

Object_ptr SemanticAnalyzer::visit(EnumMember const& expr)
{
	Symbol_ptr symbol = current_scope->lookup(expr.member_chain.front());
	auto enum_type = type_system->extract_enum_type(symbol->type);
	wstring enum_string = concat(expr.member_chain, L"::");
	ASSERT(enum_type->members.contains(enum_string), "Enum does not contain this member");

	return symbol->type;
}

Object_ptr SemanticAnalyzer::visit(Call const& expr)
{
	Symbol_ptr symbol = current_scope->lookup(expr.name);
	ObjectVector argument_types = visit(expr.arguments);

	Object_ptr return_type = std::visit(overloaded{
		[&](FunctionType const& type)
		{
			ASSERT(type_system->equal(current_scope, argument_types, type.input_types), "Argument mismatch in call");
			return type.return_type.value_or(type_system->type_pool->get_none_type());
		},
		[&](GeneratorType const& type)
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

Object_ptr SemanticAnalyzer::visit(Prefix const& expr)
{
	Object_ptr operand_type = visit(expr.operand);
	Object_ptr result_type = type_system->infer(current_scope, operand_type, expr.op->type);
	return result_type;
}

Object_ptr SemanticAnalyzer::visit(Postfix const& expr)
{
	Object_ptr operand_type = visit(expr.operand);
	return type_system->type_pool->get_none_type();
}

Object_ptr SemanticAnalyzer::visit(Infix const& expr)
{
	Object_ptr lhs_operand_type = visit(expr.left);
	Object_ptr rhs_operand_type = visit(expr.right);

	Object_ptr result_type = type_system->infer(current_scope, lhs_operand_type, expr.op->type, rhs_operand_type);
	return result_type;
}

Object_ptr SemanticAnalyzer::visit(Identifier const& expr)
{
	Symbol_ptr symbol = current_scope->lookup(expr.name);
	return symbol->type;
}

Object_ptr SemanticAnalyzer::visit(MemberAccess const& expr)
{
	Object_ptr lhs_operand_type = visit(expr.left);
	auto lhs_class_type = type_system->extract_class_type(lhs_operand_type);

	return std::visit(overloaded{
		[&](Identifier const& identifier)
		{
			ASSERT(lhs_class_type->members.contains(identifier.name), "Identifier is not found in class");
			Object_ptr rhs_type = lhs_class_type->members.at(identifier.name);
			return rhs_type;
		},

		[&](Call const& call)
		{
			ASSERT(lhs_class_type->members.contains(call.name), "Call is not found in class");
			Object_ptr rhs_type = lhs_class_type->members.at(call.name);

			return std::visit(overloaded{
				[&](FunctionMemberType const& type)
				{
					return type.return_type.value_or(type_system->type_pool->get_none_type());
				},
				[&](GeneratorMemberType const& type)
				{
					return type.return_type.value_or(type_system->type_pool->get_none_type());
				},
				[&](auto)
				{
					FATAL("Expected a FunctionMemberType or GeneratorMemberType");
					return type_system->type_pool->get_none_type();
				}
				}, *rhs_type);
		},

		[&](auto)
		{
			FATAL("MemberAccess is invalid!");
			return type_system->type_pool->get_none_type();
		}
		}, *expr.right);
}
