#pragma once
#include "pch.h"
#include "TypeSystem.h"
#include "Assertion.h"
#include <memory>
#include <algorithm>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::holds_alternative;
using std::wstring;
using std::get_if;
using std::vector;
using std::make_shared;
using std::move;
using std::all_of;
using std::begin;
using std::end;

Object_ptr TypeSystem::spread_type(Object_ptr type)
{
	return std::visit(overloaded{
		[&](ListType const& t)
		{
			return t.element_type;
		},

		[&](TupleType const& t)
		{
			return MAKE_OBJECT_VARIANT(VariantType(t.element_types));
		},

		[&](MapType const& t) 
		{ 
			return type; 
		},

		[&](AliasType const& t)
		{
			return spread_type(t.type);
		},

		[&](auto) 
		{ 
			FATAL("Cannot spread this type");
			return type_pool->get_none_type(); 
		}
		}, *type);
}

// Is _ type?

bool TypeSystem::is_boolean_type(const Object_ptr type) const
{
	return holds_alternative<BooleanType>(*type);
}

bool TypeSystem::is_number_type(const Object_ptr type) const
{
	return holds_alternative<IntType>(*type) || holds_alternative<FloatType>(*type);
}

bool TypeSystem::is_int_type(const Object_ptr type) const
{
	return holds_alternative<IntType>(*type);
}

bool TypeSystem::is_float_type(const Object_ptr type) const
{
	return holds_alternative<FloatType>(*type);
}

bool TypeSystem::is_string_type(const Object_ptr type) const
{
	return holds_alternative<StringType>(*type);
}

bool TypeSystem::is_none_type(const Object_ptr type) const
{
	return holds_alternative<NoneType>(*type);
}

bool TypeSystem::is_condition_type(SymbolScope_ptr scope, const Object_ptr condition_type) const
{
	return std::visit(overloaded{
		[&](VariantType const& type)
		{
			return all_of(begin(type.types), end(type.types), [&](Object_ptr t) { return is_condition_type(scope, t); });
		},

		[](auto) { return true; }
		}, *condition_type);
}

bool TypeSystem::is_spreadable_type(SymbolScope_ptr scope, const Object_ptr candidate_type) const
{
	return std::visit(overloaded{
		[&](ListType const& type) { return true; },
		[&](TupleType const& type) { return true; },
		[&](MapType const& type) { return true; },

		[&](VariantType const& type)
		{
			return all_of(begin(type.types), end(type.types), [&](Object_ptr t) { return is_spreadable_type(scope, t); });
		},

		[](auto) { return false; }
		}, *candidate_type);
}

bool TypeSystem::is_iterable_type(SymbolScope_ptr scope, const Object_ptr candidate_type) const
{
	return std::visit(overloaded{
		[&](StringType const& type) { return true; },
		[&](ListType const& type) { return true; },
		[&](MapType const& type) { return true; },

		[&](VariantType const& type)
		{
			return all_of(begin(type.types), end(type.types), [&](Object_ptr t) { return is_iterable_type(scope, t); });
		},

		[](auto) { return false; }
		}, *candidate_type);
}

bool TypeSystem::is_key_type(SymbolScope_ptr scope, const Object_ptr key_type) const
{
	return std::visit(overloaded{
	[&](IntType const& type) { return true; },
	[&](FloatType const& type) { return true; },
	[&](BooleanType const& type) { return true; },
	[&](StringType const& type) { return true; },
	[&](IntLiteralType const& type) { return true; },
	[&](FloatLiteralType const& type) { return true; },
	[&](StringLiteralType const& type) { return true; },
	[&](BooleanLiteralType const& type) { return true; },
	[&](VariantType const& type)
	{
		return all_of(begin(type.types), end(type.types), [&](Object_ptr t) { return is_key_type(scope, t); });
	},

	[](auto) { return false; }
		}, *key_type);
}

bool TypeSystem::is_enum_type(SymbolScope_ptr scope, const Object_ptr type) const
{
	return holds_alternative<EnumType>(*type);
}

bool TypeSystem::is_alias_type(SymbolScope_ptr scope, const Object_ptr type) const
{
	return holds_alternative<AliasType>(*type);
}

bool TypeSystem::is_class_type(SymbolScope_ptr scope, const Object_ptr type) const
{
	return holds_alternative<ClassType>(*type);
}

// assert type

void TypeSystem::expect_boolean_type(const Object_ptr type) const
{
	ASSERT(is_boolean_type(type), "Must be a BooleanType");
}

void TypeSystem::expect_number_type(const Object_ptr type) const
{
	ASSERT(is_number_type(type), "Must be a Number Type");
}

void TypeSystem::expect_int_type(const Object_ptr type) const
{
	ASSERT(is_int_type(type), "Must be a IntType");
}

void TypeSystem::expect_float_type(const Object_ptr type) const
{
	ASSERT(is_float_type(type), "Must be a FloatType");
}

void TypeSystem::expect_string_type(const Object_ptr type) const
{
	ASSERT(is_string_type(type), "Must be a StringType");
}

void TypeSystem::expect_none_type(const Object_ptr type) const
{
	ASSERT(is_none_type(type), "Must be a NoneType");
}

void TypeSystem::expect_condition_type(SymbolScope_ptr scope, const Object_ptr type) const
{
	ASSERT(is_condition_type(scope, type), "Must be a Condition Type");
}

void TypeSystem::expect_spreadable_type(SymbolScope_ptr scope, const Object_ptr type) const
{
	ASSERT(is_spreadable_type(scope, type), "Must be a Spreadable type");
}

void TypeSystem::expect_iterable_type(SymbolScope_ptr scope, const Object_ptr type) const
{
	ASSERT(is_iterable_type(scope, type), "Must be a iterable Type");
}

void TypeSystem::expect_key_type(SymbolScope_ptr scope, const Object_ptr type) const
{
	ASSERT(is_key_type(scope, type), "Must be a key Type");
}

// extract 

FunctionType* TypeSystem::extract_function_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<FunctionType>(*type), "Must be a FunctionType");
	auto inner_type = get_if<FunctionType>(&*type);
	return inner_type;
}

EnumType* TypeSystem::extract_enum_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<EnumType>(*type), "Must be a EnumType");
	auto inner_type = get_if<EnumType>(&*type);
	return inner_type;
}

AliasType* TypeSystem::extract_alias_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<AliasType>(*type), "Must be a AliasType");
	auto inner_type = get_if<AliasType>(&*type);
	return inner_type;
}

ClassType* TypeSystem::extract_class_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<ClassType>(*type), "Must be a ClassType");
	auto inner_type = get_if<ClassType>(&*type);
	return inner_type;
}
