#include "pch.h"
#include "TypeSystem.h"
#include "Type.h"
#include <memory>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_TYPE(x) std::make_shared<Type>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

TypeSystem::TypeSystem()
{
	type_pool.insert({ 0, MAKE_TYPE(BooleanType()) });
	type_pool.insert({ 1, MAKE_TYPE(NumberType()) });
	type_pool.insert({ 2, MAKE_TYPE(StringType()) });
}

bool TypeSystem::is_boolean_type(const Type_ptr type) const
{
	return holds_alternative<BooleanType>(*type);
}

bool TypeSystem::is_number_type(const Type_ptr type) const
{
	return holds_alternative<NumberType>(*type);
}

bool TypeSystem::is_string_type(const Type_ptr type) const
{
	return holds_alternative<StringType>(*type);
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
	[&](NumberType const& type) { return true; },
	[&](BooleanType const& type) { return true; },
	[&](StringType const& type) { return true; },

	[](auto) { return false; }
		}, *key_type);
}

// equal

bool TypeSystem::equal(const Type_ptr type_1, const Type_ptr type_2) const
{
	return std::visit(overloaded{
		[&](NumberType const& type_1, NumberType const& type_2) { return true; },
		[&](BooleanType const& type_1, BooleanType const& type_2) { return true; },
		[&](StringType const& type_1, StringType const& type_2) { return true; },

		[&](ListType const& type_1, ListType const& type_2)
		{
			return equal(type_1.element_type, type_2.element_type);
		},

		[&](TupleType const& type_1, TupleType const& type_2)
		{
			return equal(type_1.element_types, type_2.element_types);
		},

		[&](VariantType const& type_1, VariantType const& type_2)
		{
			return equal(type_1.types, type_2.types);
		},

		[&](MapType const& type_1, MapType const& type_2)
		{
			bool key_compare = equal(type_1.key_type, type_2.key_type);
			bool value_compare = equal(type_1.value_type, type_2.value_type);
			return key_compare && value_compare;
		},

		[&](UDTType const& type_1, UDTType const& type_2) { return type_1.name == type_2.name; },
		[&](EnumType const& type_1, EnumType const& type_2) { return type_1.enum_name == type_2.enum_name; },

		[&](NoneType const& type_1, NoneType const& type_2) { return true; },

		[](auto, auto) { return false; }
		}, *type_1, *type_2);
}

bool TypeSystem::equal(const TypeVector type_vector_1, const TypeVector type_vector_2) const
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

		if (!equal(type_1, type_2))
		{
			return false;
		}
	}

	return true;
}

// assignable

bool TypeSystem::assignable(const Type_ptr lhs_type, const Type_ptr rhs_type) const
{
	return std::visit(overloaded{
		[&](AnyType const&, auto) { return true; },
		[&](VariantType const& lhs_variant_type, auto) { return assignable(lhs_variant_type, rhs_type); },
		[&](auto, auto) { return equal(lhs_type, rhs_type); }
		}, *lhs_type, *rhs_type);
}

bool TypeSystem::assignable(VariantType const& lhs_variant_type, Type_ptr const& rhs_type) const
{
	for (auto const& type : lhs_variant_type.types)
	{
		if (equal(type, rhs_type))
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

Type_ptr TypeSystem::get_number_type() const
{
	return type_pool.at(1);
}

Type_ptr TypeSystem::get_string_type() const
{
	return type_pool.at(2);
}