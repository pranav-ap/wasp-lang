#pragma once
#include "pch.h"
#include "TypeSystem.h"
#include "Type.h"
#include "SymbolScope.h"
#include "Assertion.h"
#include <memory>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_TYPE(x) std::make_shared<Type>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

TypeSystem::TypeSystem()
{
	type_pool.insert({ 0, MAKE_TYPE(BooleanType()) });
	type_pool.insert({ 1, MAKE_TYPE(IntType()) });
	type_pool.insert({ 2, MAKE_TYPE(FloatType()) });
	type_pool.insert({ 3, MAKE_TYPE(StringType()) });
	type_pool.insert({ 4, MAKE_TYPE(NoneType()) });
}

bool TypeSystem::is_boolean_type(const Type_ptr type) const
{
	return holds_alternative<BooleanType>(*type);
}

bool TypeSystem::is_number_type(const Type_ptr type) const
{
	return holds_alternative<IntType>(*type) || holds_alternative<FloatType>(*type);
}

bool TypeSystem::is_int_type(const Type_ptr type) const
{
	return holds_alternative<IntType>(*type);
}

bool TypeSystem::is_float_type(const Type_ptr type) const
{
	return holds_alternative<FloatType>(*type);
}

bool TypeSystem::is_string_type(const Type_ptr type) const
{
	return holds_alternative<StringType>(*type);
}

bool TypeSystem::is_none_type(const Type_ptr type) const
{
	return holds_alternative<NoneType>(*type);
}

bool TypeSystem::is_condition_type(const Type_ptr condition_type) const
{
	return std::visit(overloaded{
	[&](IntType const& type) { return true; },
	[&](FloatType const& type) { return true; },
	[&](BooleanType const& type) { return true; },
	[&](StringType const& type) { return true; },
	[&](ListType const& type) { return true; },
	[&](TupleType const& type) { return true; },
	[&](VariantType const& type) { return true; },
	[&](MapType const& type) { return true; },
	[&](ClassType const& type) { return true; },
	[&](EnumType const& type) { return true; },
	[&](NoneType const& type) { return true; },
	[&](FunctionType const& type) { return true; },
	[&](GeneratorType const& type) { return true; },

	[](auto) { return false; }
		}, *condition_type);
}

bool TypeSystem::is_spreadable_type(const Type_ptr condition_type) const
{
	return std::visit(overloaded{
		[&](StringType const& type) { return true; },
		[&](ListType const& type) { return true; },
		[&](TupleType const& type) { return true; },
		[&](MapType const& type) { return true; },

		[](auto) { return false; }
		}, *condition_type);
}

bool TypeSystem::is_iterable_type(const Type_ptr type) const
{
	return std::visit(overloaded{
		[&](StringType const& stat) { return true; },
		[&](ListType const& stat) { return true; },
		[&](MapType const& stat) { return true; },

		[](auto) { return false; }
		}, *type);
}

bool TypeSystem::is_key_type(const Type_ptr key_type) const
{
	return std::visit(overloaded{
	[&](IntType const& type) { return true; },
	[&](FloatType const& type) { return true; },
	[&](BooleanType const& type) { return true; },
	[&](StringType const& type) { return true; },

	[](auto) { return false; }
		}, *key_type);
}

// equal

bool TypeSystem::equal(SymbolScope_ptr scope, const Type_ptr type_1, const Type_ptr type_2) const
{
	return std::visit(overloaded{
		[&](IntType const& type_1, IntType const& type_2) { return true; },
		[&](FloatType const& type_1, FloatType const& type_2) { return true; },
		[&](BooleanType const& type_1, BooleanType const& type_2) { return true; },
		[&](StringType const& type_1, StringType const& type_2) { return true; },

		[&](ListType const& type_1, ListType const& type_2)
		{
			return equal(scope, type_1.element_type, type_2.element_type);
		},

		[&](TupleType const& type_1, TupleType const& type_2)
		{
			return equal(scope, type_1.element_types, type_2.element_types);
		},

		[&](VariantType const& type_1, VariantType const& type_2)
		{
			return equal(scope, type_1.types, type_2.types);
		},
		[&](MapType const& type_1, MapType const& type_2)
		{
			bool key_compare = equal(scope, type_1.key_type, type_2.key_type);
			bool value_compare = equal(scope, type_1.value_type, type_2.value_type);
			return key_compare && value_compare;
		},

		[&](ClassType const& type_1, ClassType const& type_2)
		{
			std::optional<Symbol_ptr> symbol_1 = scope->lookup(type_1.name);
			OPT_CHECK(symbol_1);
			std::optional<Symbol_ptr> symbol_2 = scope->lookup(type_2.name);
			OPT_CHECK(symbol_2);

			bool udt_exists = std::visit(overloaded{
				[&](ClassSymbol const& sym_1, ClassSymbol const& sym_2) { return true; },

				[](auto, auto) { return false; }
				}, *symbol_1.value(), *symbol_2.value());

			return udt_exists && (type_1.name == type_2.name);
		},

		[&](EnumType const& type_1, EnumType const& type_2)
		{
			std::optional<Symbol_ptr> symbol_1 = scope->lookup(type_1.enum_name);
			OPT_CHECK(symbol_1);
			std::optional<Symbol_ptr> symbol_2 = scope->lookup(type_2.enum_name);
			OPT_CHECK(symbol_2);

			bool enum_reachable = std::visit(overloaded{
				[&](EnumSymbol const& sym_1, EnumSymbol const& sym_2) { return true; },

				[](auto, auto) { return false; }
				}, *symbol_1.value(), *symbol_2.value());

			return enum_reachable && (type_1.enum_name == type_2.enum_name);
		},

		[&](NoneType const& type_1, NoneType const& type_2) { return true; },
		[&](FunctionType const& type_1, FunctionType const& type_2) { return true; },
		[&](GeneratorType const& type_1, GeneratorType const& type_2) { return true; },

		[](auto, auto) { return false; }
		}, *type_1, *type_2);
}

bool TypeSystem::equal(SymbolScope_ptr scope, const TypeVector type_vector_1, const TypeVector type_vector_2) const
{
	int type_vector_1_length = type_vector_1.size();
	int type_vector_2_length = type_vector_2.size();

	if (type_vector_1_length != type_vector_2_length)
	{
		return false;
	}

	for (int index = 0; index < type_vector_1_length; index++)
	{
		auto type_1 = type_vector_1.at(index);
		auto type_2 = type_vector_2.at(index);

		if (!equal(scope, type_1, type_2))
		{
			return false;
		}
	}

	return true;
}

// assignable

bool TypeSystem::assignable(SymbolScope_ptr scope, const Type_ptr lhs_type, const Type_ptr rhs_type) const
{
	return std::visit(overloaded{
		[&](AnyType const&, auto) { return true; },
		[&](VariantType const& lhs_variant_type, auto) { return assignable(scope, lhs_variant_type, rhs_type); },
		[&](auto, auto) { return equal(scope, lhs_type, rhs_type); }
		}, *lhs_type, *rhs_type);
}

bool TypeSystem::assignable(SymbolScope_ptr scope, VariantType const& lhs_variant_type, Type_ptr const& rhs_type) const
{
	for (auto const& type : lhs_variant_type.types)
	{
		if (equal(scope, type, rhs_type))
		{
			return true;
		}
	}

	return false;
}

// utils

Type_ptr TypeSystem::get_boolean_type() const
{
	return type_pool.at(0);
}

Type_ptr TypeSystem::get_int_type() const
{
	return type_pool.at(1);
}

Type_ptr TypeSystem::get_float_type() const
{
	return type_pool.at(2);
}

Type_ptr TypeSystem::get_string_type() const
{
	return type_pool.at(3);
}

Type_ptr TypeSystem::get_none_type() const
{
	return type_pool.at(4);
}