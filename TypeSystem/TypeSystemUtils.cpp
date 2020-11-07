#pragma once
#include "pch.h"
#include "TypeSystem.h"
#include "SymbolScope.h"
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

Object_ptr TypeSystem::get_any_type()
{
	return type_pool->get(0);
}

Object_ptr TypeSystem::get_int_type()
{
	return type_pool->get(1);
}

Object_ptr TypeSystem::get_float_type()
{
	return type_pool->get(2);
}

Object_ptr TypeSystem::get_string_type()
{
	return type_pool->get(3);
}

Object_ptr TypeSystem::get_boolean_type()
{
	return type_pool->get(4);
}

Object_ptr TypeSystem::get_none_type()
{
	return type_pool->get(5);
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
		[&](FunctionType const& type) { return false; },
		[&](GeneratorType const& type) { return false; },
		[&](FunctionMemberType const& type) { return false; },
		[&](GeneratorMemberType const& type) { return false; },
		[&](OperatorType const& type) { return false; },

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
		[&](StringType const& type) { return true; },
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
	[&](EnumType const& type) { return true; },

	[&](VariantType const& type)
	{
		return all_of(begin(type.types), end(type.types), [&](Object_ptr t) { return is_key_type(scope, t); });
	},

	[](auto) { return false; }
		}, *key_type);
}

bool TypeSystem::is_class_type(const Object_ptr type) const
{
	return holds_alternative<ClassType>(*type);
}

bool TypeSystem::is_interface_type(const Object_ptr type) const
{
	return holds_alternative<InterfaceType>(*type);
}

bool TypeSystem::is_enum_type(const Object_ptr type) const
{
	return holds_alternative<EnumType>(*type);
}

bool TypeSystem::is_function_type(const Object_ptr type) const
{
	return holds_alternative<FunctionType>(*type);
}

bool TypeSystem::is_generator_type(const Object_ptr type) const
{
	return holds_alternative<GeneratorType>(*type);
}

bool TypeSystem::is_function_method_type(const Object_ptr type) const
{
	return holds_alternative<FunctionMemberType>(*type);
}

bool TypeSystem::is_generator_method_type(const Object_ptr type) const
{
	return holds_alternative<GeneratorMemberType>(*type);
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

void TypeSystem::expect_class_type(const Object_ptr type) const
{
	ASSERT(is_class_type(type), "Must be a ClassType");
}

void TypeSystem::expect_interface_type(const Object_ptr type) const
{
	ASSERT(is_interface_type(type), "Must be a InterfaceType");
}

void TypeSystem::expect_enum_type(const Object_ptr type) const
{
	ASSERT(is_enum_type(type), "Must be a EnumType");
}

void TypeSystem::expect_function_type(const Object_ptr type) const
{
	ASSERT(is_function_type(type), "Must be a FunctionType");
}

void TypeSystem::expect_generator_type(const Object_ptr type) const
{
	ASSERT(is_generator_type(type), "Must be a GeneratorType");
}

void TypeSystem::expect_function_method_type(const Object_ptr type) const
{
	ASSERT(is_function_method_type(type), "Must be a FunctionMemberType");
}

void TypeSystem::expect_generator_method_type(const Object_ptr type) const
{
	ASSERT(is_generator_method_type(type), "Must be a GeneratorMemberType");
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

// Extract type from variant

ClassType* TypeSystem::extract_class_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<ClassType>(*type), "Must be a ClassType");
	auto inner_type = get_if<ClassType>(&*type);
	return inner_type;
}

InterfaceType* TypeSystem::extract_interface_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<InterfaceType>(*type), "Must be a InterfaceType");
	auto inner_type = get_if<InterfaceType>(&*type);
	return inner_type;
}

EnumType* TypeSystem::extract_enum_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<EnumType>(*type), "Must be a EnumType");
	auto inner_type = get_if<EnumType>(&*type);
	return inner_type;
}

FunctionType* TypeSystem::extract_function_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<FunctionType>(*type), "Must be a FunctionType");
	auto inner_type = get_if<FunctionType>(&*type);
	return inner_type;
}

GeneratorType* TypeSystem::extract_generator_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<GeneratorType>(*type), "Must be a GeneratorType");
	auto inner_type = get_if<GeneratorType>(&*type);
	return inner_type;
}

FunctionMemberType* TypeSystem::extract_function_member_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<FunctionMemberType>(*type), "Must be a FunctionMemberType");
	auto inner_type = get_if<FunctionMemberType>(&*type);
	return inner_type;
}

GeneratorMemberType* TypeSystem::extract_generator_member_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<GeneratorMemberType>(*type), "Must be a GeneratorMemberType");
	auto inner_type = get_if<GeneratorMemberType>(&*type);
	return inner_type;
}
