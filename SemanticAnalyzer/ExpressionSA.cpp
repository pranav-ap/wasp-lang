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

Object_ptr SemanticAnalyzer::visit(Expression_ptr expression)
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
		[&](NewObject& expr) { return visit(expr); },
		[&](EnumMember& expr) { return visit(expr); },
		[&](MemberAccess& expr) { return visit(expr); },
		[&](Identifier& expr) { return visit(expr); },
		[&](Prefix& expr) { return visit(expr); },
		[&](Infix& expr) { return visit(expr); },
		[&](Postfix& expr) { return visit(expr); },
		[&](Call& expr) { return visit(expr); },
		[&](TagPattern& expr) { return visit(expr); },
		[&](Assignment& expr) { return visit(expr); },
		[&](Spread& expr) { return visit(expr); },
		[&](TernaryCondition& expr) { return visit(expr); },

		[&](auto)
		{
			FATAL("Never Seen this Expression before!");
			return type_system->type_pool->get_none_type();
		}
		}, *expression);

	return type;
}

ObjectVector SemanticAnalyzer::visit(ExpressionVector& expressions)
{
	ObjectVector types;

	for (auto &argument : expressions)
	{
		Object_ptr type = visit(argument);
		types.push_back(type);
	}

	return types;
}

Object_ptr SemanticAnalyzer::visit(int expr)
{
	return type_system->type_pool->get_int_type();
}

Object_ptr SemanticAnalyzer::visit(double expr)
{
	return type_system->type_pool->get_float_type();
}

Object_ptr SemanticAnalyzer::visit(std::wstring expr)
{
	return type_system->type_pool->get_string_type();
}

Object_ptr SemanticAnalyzer::visit(bool expr)
{
	return type_system->type_pool->get_boolean_type();
}

Object_ptr SemanticAnalyzer::visit(ListLiteral& expr)
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

Object_ptr SemanticAnalyzer::visit(TupleLiteral& expr)
{
	ObjectVector types = visit(expr.expressions);
	Object_ptr tuple_type = MAKE_OBJECT_VARIANT(TupleType(types));
	return tuple_type;
}

Object_ptr SemanticAnalyzer::visit(SetLiteral& expr)
{
	ObjectVector types = visit(expr.expressions);
	Object_ptr set_type = MAKE_OBJECT_VARIANT(SetType(types));
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

Object_ptr SemanticAnalyzer::visit(Assignment& expression)
{
	Object_ptr lhs_type = visit(expression.lhs_expression);
	Object_ptr rhs_type = visit(expression.rhs_expression);

	ASSERT(type_system->assignable(current_scope, lhs_type, rhs_type), "Expression mismatch in assignment");

	return rhs_type;
}

Object_ptr SemanticAnalyzer::visit(TernaryCondition& expression)
{
	enter_scope(ScopeType::CONDITIONAL);
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

Object_ptr SemanticAnalyzer::visit(Spread& expr)
{
	Object_ptr operand_type = visit(expr.expression);
	type_system->expect_spreadable_type(current_scope, operand_type);

	// Need to expand this type before returning
	return operand_type;
}

Object_ptr SemanticAnalyzer::visit(TagPattern& expr)
{
	FATAL("TagPattern must be handled by parent nodes");
	return type_system->type_pool->get_none_type();
}

Object_ptr SemanticAnalyzer::visit(NewObject& expr)
{
	ObjectVector types = visit(expr.expressions);

	auto symbol = current_scope->lookup(expr.type_name);
	auto class_type = type_system->extract_class_type(symbol->type);

	wstring constructor_name = L"constructor";
	auto exists = class_type->members.contains(constructor_name);
	ASSERT(exists, "Required constructor must exist");

	return symbol->type;
}

Object_ptr SemanticAnalyzer::visit(EnumMember& expr)
{
	Symbol_ptr symbol = current_scope->lookup(expr.member_chain.front());
	auto enum_type = type_system->extract_enum_type(symbol->type);
	wstring enum_string = concat(expr.member_chain, L"::");
	ASSERT(enum_type->members.contains(enum_string), "Enum does not contain this member");

	return symbol->type;
}

Object_ptr SemanticAnalyzer::visit(Call& expr)
{
	Symbol_ptr symbol = current_scope->lookup(expr.name);
	ObjectVector argument_types = visit(expr.arguments);

	Object_ptr return_type = std::visit(overloaded{
		[&](FunctionType& type)
		{
			ASSERT(type_system->equal(current_scope, argument_types, type.input_types), "Argument mismatch in call");
			return type.return_type.value_or(type_system->type_pool->get_none_type());
		},
		[&](GeneratorType& type)
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

Object_ptr SemanticAnalyzer::visit(Identifier& expr)
{
	Symbol_ptr symbol = current_scope->lookup(expr.name);
	return symbol->type;
}

Object_ptr SemanticAnalyzer::visit(MemberAccess& expr)
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
