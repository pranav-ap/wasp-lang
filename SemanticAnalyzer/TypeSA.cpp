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


Object_ptr SemanticAnalyzer::visit(AnyTypeNode const& expr)
{
	return type_system->type_pool->get_any_type();
}

Object_ptr SemanticAnalyzer::visit(IntLiteralTypeNode const& expr)
{
	return MAKE_OBJECT_VARIANT(IntLiteralType(expr.value));
}

Object_ptr SemanticAnalyzer::visit(FloatLiteralTypeNode const& expr)
{
	return MAKE_OBJECT_VARIANT(FloatLiteralType(expr.value));
}

Object_ptr SemanticAnalyzer::visit(StringLiteralTypeNode const& expr)
{
	return MAKE_OBJECT_VARIANT(StringLiteralType(expr.value));
}

Object_ptr SemanticAnalyzer::visit(BooleanLiteralTypeNode const& expr)
{
	return MAKE_OBJECT_VARIANT(BooleanLiteralType(expr.value));
}

Object_ptr SemanticAnalyzer::visit(IntTypeNode const& expr)
{
	return type_system->type_pool->get_int_type();
}

Object_ptr SemanticAnalyzer::visit(FloatTypeNode const& expr)
{
	return type_system->type_pool->get_float_type();
}

Object_ptr SemanticAnalyzer::visit(StringTypeNode const& expr)
{
	return type_system->type_pool->get_string_type();
}

Object_ptr SemanticAnalyzer::visit(BooleanTypeNode const& expr)
{
	return type_system->type_pool->get_boolean_type();
}

Object_ptr SemanticAnalyzer::visit(NoneTypeNode const& expr)
{
	return type_system->type_pool->get_none_type();
}

Object_ptr SemanticAnalyzer::visit(ListTypeNode const& expr)
{
	auto type = visit(expr.element_type);
	return MAKE_OBJECT_VARIANT(ListType(type));
}

Object_ptr SemanticAnalyzer::visit(TupleTypeNode const& expr)
{
	auto type = visit(expr.element_types);
	return MAKE_OBJECT_VARIANT(TupleType(type));
}

Object_ptr SemanticAnalyzer::visit(SetTypeNode const& expr)
{
	auto type = visit(expr.element_types);
	return MAKE_OBJECT_VARIANT(SetType(type));
}

Object_ptr SemanticAnalyzer::visit(MapTypeNode const& expr)
{
	auto key_type = visit(expr.key_type);
	auto value_type = visit(expr.value_type);

	return MAKE_OBJECT_VARIANT(MapType(key_type, value_type));
}

Object_ptr SemanticAnalyzer::visit(VariantTypeNode const& expr)
{
	auto types = visit(expr.types);
	return MAKE_OBJECT_VARIANT(VariantType(types));
}

Object_ptr SemanticAnalyzer::visit(FunctionTypeNode const& expr)
{
	auto input_types = visit(expr.input_types);

	if (expr.return_type.has_value())
	{
		auto return_type = visit(expr.return_type.value());
		return MAKE_OBJECT_VARIANT(FunctionType(input_types, return_type));
	}

	return MAKE_OBJECT_VARIANT(FunctionType(input_types, std::nullopt));
}

Object_ptr SemanticAnalyzer::visit(GeneratorTypeNode const& expr)
{
	auto input_types = visit(expr.input_types);

	if (expr.return_type.has_value())
	{
		auto return_type = visit(expr.return_type.value());
		return MAKE_OBJECT_VARIANT(GeneratorType(input_types, return_type));
	}

	return MAKE_OBJECT_VARIANT(GeneratorType(input_types, std::nullopt));
}

Object_ptr SemanticAnalyzer::visit(FunctionMemberTypeNode const& expr)
{
	auto input_types = visit(expr.input_types);

	if (expr.return_type.has_value())
	{
		auto return_type = visit(expr.return_type.value());
		return MAKE_OBJECT_VARIANT(FunctionMemberType(expr.type_name, input_types, return_type));
	}

	return MAKE_OBJECT_VARIANT(FunctionMemberType(expr.type_name, input_types, std::nullopt));
}

Object_ptr SemanticAnalyzer::visit(GeneratorMemberTypeNode const& expr)
{
	auto input_types = visit(expr.input_types);

	if (expr.return_type.has_value())
	{
		auto return_type = visit(expr.return_type.value());
		return MAKE_OBJECT_VARIANT(GeneratorMemberType(expr.type_name, input_types, return_type));
	}

	return MAKE_OBJECT_VARIANT(GeneratorMemberType(expr.type_name, input_types, std::nullopt));
}

Object_ptr SemanticAnalyzer::visit(TypeIdentifierTypeNode const& expr)
{
	Symbol_ptr symbol = current_scope->lookup(expr.name);
	return symbol->type;
}

