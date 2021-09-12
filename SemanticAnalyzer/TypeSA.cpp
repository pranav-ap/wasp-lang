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

Object_ptr SemanticAnalyzer::visit(const TypeNode_ptr type_node)
{
	Object_ptr type = std::visit(overloaded{
		[&](AnyTypeNode& node) { return visit(node); },
		[&](IntLiteralTypeNode& node) { return visit(node); },
		[&](FloatLiteralTypeNode& node) { return visit(node); },
		[&](StringLiteralTypeNode& node) { return visit(node); },
		[&](BooleanLiteralTypeNode& node) { return visit(node); },
		[&](IntTypeNode& node) { return visit(node); },
		[&](FloatTypeNode& node) { return visit(node); },
		[&](StringTypeNode& node) { return visit(node); },
		[&](BooleanTypeNode& node) { return visit(node); },
		[&](NoneTypeNode& node) { return visit(node); },
		[&](ListTypeNode& node) { return visit(node); },
		[&](TupleTypeNode& node) { return visit(node); },
		[&](SetTypeNode& node) { return visit(node); },
		[&](MapTypeNode& node) { return visit(node); },
		[&](VariantTypeNode& node) { return visit(node); },
		[&](FunctionTypeNode& node) { return visit(node); },
		[&](TypeIdentifierNode& node) { return visit(node); },

		[&](auto)
		{
			FATAL("Never Seen this TypeNode before!");
			return type_system->type_pool->get_none_type();
		}
		}, *type_node);

	return type;
}

ObjectVector SemanticAnalyzer::visit(std::vector<TypeNode_ptr>& type_nodes)
{
	ObjectVector types;

	for (const auto type_node : type_nodes)
	{
		auto type = visit(type_node);
		types.push_back(type);
	}

	return types;
}

Object_ptr SemanticAnalyzer::visit(AnyTypeNode& expr)
{
	return type_system->type_pool->get_any_type();
}

Object_ptr SemanticAnalyzer::visit(IntLiteralTypeNode& expr)
{
	return MAKE_OBJECT_VARIANT(IntLiteralType(expr.value));
}

Object_ptr SemanticAnalyzer::visit(FloatLiteralTypeNode& expr)
{
	return MAKE_OBJECT_VARIANT(FloatLiteralType(expr.value));
}

Object_ptr SemanticAnalyzer::visit(StringLiteralTypeNode& expr)
{
	return MAKE_OBJECT_VARIANT(StringLiteralType(expr.value));
}

Object_ptr SemanticAnalyzer::visit(BooleanLiteralTypeNode& expr)
{
	if (expr.value)
	{
		return type_system->type_pool->get_true_literal_type();
	}

	return type_system->type_pool->get_false_literal_type();
}

Object_ptr SemanticAnalyzer::visit(IntTypeNode& expr)
{
	return type_system->type_pool->get_int_type();
}

Object_ptr SemanticAnalyzer::visit(FloatTypeNode& expr)
{
	return type_system->type_pool->get_float_type();
}

Object_ptr SemanticAnalyzer::visit(StringTypeNode& expr)
{
	return type_system->type_pool->get_string_type();
}

Object_ptr SemanticAnalyzer::visit(BooleanTypeNode& expr)
{
	return type_system->type_pool->get_boolean_type();
}

Object_ptr SemanticAnalyzer::visit(NoneTypeNode& expr)
{
	return type_system->type_pool->get_none_type();
}

Object_ptr SemanticAnalyzer::visit(ListTypeNode& expr)
{
	auto type = visit(expr.element_type);
	return MAKE_OBJECT_VARIANT(ListType(type));
}

Object_ptr SemanticAnalyzer::visit(TupleTypeNode& expr)
{
	auto type = visit(expr.element_types);
	return MAKE_OBJECT_VARIANT(TupleType(type));
}

Object_ptr SemanticAnalyzer::visit(SetTypeNode& expr)
{
	auto type = visit(expr.element_type);
	return MAKE_OBJECT_VARIANT(SetType(type));
}

Object_ptr SemanticAnalyzer::visit(MapTypeNode& expr)
{
	auto key_type = visit(expr.key_type);
	auto value_type = visit(expr.value_type);

	return MAKE_OBJECT_VARIANT(MapType(key_type, value_type));
}

Object_ptr SemanticAnalyzer::visit(VariantTypeNode& expr)
{
	auto types = visit(expr.types);
	return MAKE_OBJECT_VARIANT(VariantType(types));
}

Object_ptr SemanticAnalyzer::visit(FunctionTypeNode& expr)
{
	auto input_types = visit(expr.input_types);

	if (expr.return_type.has_value())
	{
		auto return_type = visit(expr.return_type.value());
		return MAKE_OBJECT_VARIANT(FunctionType(input_types, return_type));
	}

	return MAKE_OBJECT_VARIANT(FunctionType(input_types));
}

Object_ptr SemanticAnalyzer::visit(TypeIdentifierNode& expr)
{
	auto type_name = expr.name;
	Symbol_ptr symbol = current_scope->lookup(type_name);

	Object_ptr type = std::visit(overloaded{
		[&](EnumDefinitionObject const& type_def_object)
		{
			return MAKE_OBJECT_VARIANT(EnumType(type_def_object.name));
		},
		[&](AliasDefinitionObject const& type_def_object)
		{
			return MAKE_OBJECT_VARIANT(AliasType(type_def_object.name));
		},
		[&](ClassDefinitionObject const& type_def_object)
		{
			return MAKE_OBJECT_VARIANT(ClassType(type_def_object.name));
		},
		[&](auto)
		{
			FATAL("Not a callable type");
			return type_system->type_pool->get_none_type();
		}
		}, *symbol->type);

	return type;
}
