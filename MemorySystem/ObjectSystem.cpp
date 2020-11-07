#pragma once
#include "pch.h"
#include "ObjectSystem.h"
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

ObjectSystem::ObjectSystem()
{
	next_id = 0;
	type_pool = std::make_shared<ConstantPool>();

	// add common type objects to pool

	type_pool->set(next_id++, MAKE_OBJECT_VARIANT(AnyType()));	   // 0
	type_pool->set(next_id++, MAKE_OBJECT_VARIANT(IntType()));	   // 1
	type_pool->set(next_id++, MAKE_OBJECT_VARIANT(FloatType()));   // 2
	type_pool->set(next_id++, MAKE_OBJECT_VARIANT(StringType()));  // 3
	type_pool->set(next_id++, MAKE_OBJECT_VARIANT(BooleanType())); // 4
	type_pool->set(next_id++, MAKE_OBJECT_VARIANT(NoneType()));    // 5
}

Object_ptr ObjectSystem::get_any_type()
{
	return type_pool->get(0);
}

Object_ptr ObjectSystem::get_int_type()
{
	return type_pool->get(1);
}

Object_ptr ObjectSystem::get_float_type()
{
	return type_pool->get(2);
}

Object_ptr ObjectSystem::get_string_type()
{
	return type_pool->get(3);
}

Object_ptr ObjectSystem::get_bool_type()
{
	return type_pool->get(4);
}

Object_ptr ObjectSystem::get_none_type()
{
	return type_pool->get(5);
}

// equal

bool ObjectSystem::equal(SymbolScope_ptr scope, const Object_ptr type_1, const Object_ptr type_2) const
{
	return std::visit(overloaded{
		[&](AnyType const& type_1, AnyType const& type_2) { return true; },

		[&](IntType const& type_1, IntType const& type_2) { return true; },
		[&](FloatType const& type_1, FloatType const& type_2) { return true; },
		[&](BooleanType const& type_1, BooleanType const& type_2) { return true; },
		[&](StringType const& type_1, StringType const& type_2) { return true; },

		[&](IntLiteralType const& type_1, IntLiteralType const& type_2) { return type_1.value == type_2.value; },
		[&](FloatLiteralType const& type_1, FloatLiteralType const& type_2) { return type_1.value == type_2.value; },
		[&](BooleanLiteralType const& type_1, BooleanLiteralType const& type_2) { return type_1.value == type_2.value; },
		[&](StringLiteralType const& type_1, StringLiteralType const& type_2) { return type_1.value == type_2.value; },

		[&](ListType const& type_1, ListType const& type_2)
		{
			return equal(scope, type_1.element_type, type_2.element_type);
		},

		[&](SetType const& type_1, SetType const& type_2)
		{
			return equal(scope, type_1.element_types, type_2.element_types);
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
			Symbol_ptr symbol_1 = scope->lookup(type_1.name);
			NULL_CHECK(symbol_1);
			Symbol_ptr symbol_2 = scope->lookup(type_2.name);
			NULL_CHECK(symbol_2);

			return symbol_1->name == symbol_2->name;
		},

		[&](AliasType const& type_1, AliasType const& type_2)
		{
			Symbol_ptr symbol_1 = scope->lookup(type_1.name);
			NULL_CHECK(symbol_1);
			Symbol_ptr symbol_2 = scope->lookup(type_2.name);
			NULL_CHECK(symbol_2);

			return symbol_1->name == symbol_2->name;
		},

		[&](InterfaceType const& type_1, InterfaceType const& type_2)
		{
			Symbol_ptr symbol_1 = scope->lookup(type_1.name);
			NULL_CHECK(symbol_1);
			Symbol_ptr symbol_2 = scope->lookup(type_2.name);
			NULL_CHECK(symbol_2);

			return symbol_1->name == symbol_2->name;
		},

		[&](EnumType const& type_1, EnumType const& type_2)
		{
			Symbol_ptr symbol_1 = scope->lookup(type_1.enum_name);
			NULL_CHECK(symbol_1);
			Symbol_ptr symbol_2 = scope->lookup(type_2.enum_name);
			NULL_CHECK(symbol_2);

			return symbol_1->name == symbol_2->name;
		},

		[&](NoneType const& type_1, NoneType const& type_2) { return true; },

		[&](FunctionType const& type_1, FunctionType const& type_2) { return true; },
		[&](GeneratorType const& type_1, GeneratorType const& type_2) { return true; },

		[&](FunctionMemberType const& type_1, FunctionMemberType const& type_2) { return true; },
		[&](GeneratorMemberType const& type_1, GeneratorMemberType const& type_2) { return true; },

		[&](OperatorType const& type_1, OperatorType const& type_2) { return true; },

		[](auto, auto) { return false; }
		}, *type_1, *type_2);
}

bool ObjectSystem::equal(SymbolScope_ptr scope, const ObjectVector type_vector_1, const ObjectVector type_vector_2) const
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

bool ObjectSystem::assignable(SymbolScope_ptr scope, const Object_ptr lhs_type, const Object_ptr rhs_type) const
{
	if (equal(scope, lhs_type, rhs_type))
	{
		return true;
	}

	return std::visit(overloaded{
		[&](AnyType const& type_1, auto) { return true; },

		[&](IntType const& type_1, IntLiteralType const& type_2) { return true; },
		[&](FloatType const& type_1, FloatLiteralType const& type_2) { return true; },
		[&](BooleanType const& type_1, BooleanLiteralType const& type_2) { return true; },
		[&](StringType const& type_1, StringLiteralType const& type_2) { return true; },

		[&](IntLiteralType const& type_1, IntType const& type_2) { return true; },
		[&](FloatLiteralType const& type_1, FloatType const& type_2) { return true; },
		[&](BooleanLiteralType const& type_1, BooleanType const& type_2) { return true; },
		[&](StringLiteralType const& type_1, StringType const& type_2) { return true; },

		[&](ListType const& type_1, ListType const& type_2)
		{
			return assignable(scope, type_1.element_type, type_2.element_type);
		},

		[&](SetType const& type_1, SetType const& type_2)
		{
			return assignable(scope, type_1.element_types, type_2.element_types);
		},

		[&](TupleType const& type_1, TupleType const& type_2)
		{
			return assignable(scope, type_1.element_types, type_2.element_types);
		},

		[&](MapType const& type_1, MapType const& type_2)
		{
			bool key_compare = assignable(scope, type_1.key_type, type_2.key_type);
			bool value_compare = assignable(scope, type_1.value_type, type_2.value_type);
			return key_compare && value_compare;
		},

		[&](VariantType const& lhs_variant_type, auto)
		{
			return all_of(begin(lhs_variant_type.types), end(lhs_variant_type.types), [&](Object_ptr type) { return assignable(scope, type, rhs_type); });
		},

		[](auto, auto) { return false; }
		}, *lhs_type, *rhs_type);
}

bool ObjectSystem::assignable(SymbolScope_ptr scope, const ObjectVector type_vector_1, const ObjectVector type_vector_2) const
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

		if (!assignable(scope, type_1, type_2))
		{
			return false;
		}
	}

	return true;
}

// inference

Object_ptr ObjectSystem::infer(SymbolScope_ptr scope, Object_ptr left_type, WTokenType op, Object_ptr right_type)
{
	switch (op)
	{
	case WTokenType::PLUS:
	case WTokenType::STAR:
	case WTokenType::POWER:
	{
		if (is_number_type(left_type))
		{
			expect_number_type(right_type);
			return is_int_type(right_type) ? get_int_type() : get_float_type();
		}
		else if (is_string_type(left_type))
		{
			ASSERT(is_number_type(right_type) || is_string_type(right_type), "Number or string operand is expected");
			return get_string_type();
		}

		FATAL("Number or string operand is expected");
		break;
	}
	case WTokenType::MINUS:
	case WTokenType::DIVISION:
	case WTokenType::REMINDER:
	{
		expect_number_type(left_type);
		expect_number_type(right_type);

		if (is_float_type(left_type) || is_float_type(right_type))
		{
			return get_float_type();
		}

		return get_int_type();
	}
	case WTokenType::LESSER_THAN:
	case WTokenType::LESSER_THAN_EQUAL:
	case WTokenType::GREATER_THAN:
	case WTokenType::GREATER_THAN_EQUAL:
	{
		expect_number_type(left_type);
		expect_number_type(right_type);
		return get_boolean_type();
	}
	case WTokenType::EQUAL_EQUAL:
	case WTokenType::BANG_EQUAL:
	{
		if (is_number_type(left_type))
		{
			expect_number_type(right_type);
			return get_boolean_type();
		}
		else if (is_string_type(left_type))
		{
			expect_string_type(right_type);
			return get_boolean_type();
		}
		else if (is_boolean_type(left_type))
		{
			expect_boolean_type(right_type);
			return get_boolean_type();
		}

		FATAL("Number or string or boolean operand is expected");
		break;
	}
	case WTokenType::AND:
	case WTokenType::OR:
	{
		expect_boolean_type(left_type);
		expect_boolean_type(right_type);
		return get_boolean_type();
	}
	case WTokenType::QUESTION_QUESTION:
	{
		if (equal(scope, left_type, right_type))
		{
			return left_type;
		}

		auto type = this->allocate(scope, MAKE_OBJECT_VARIANT(VariantType({ left_type, right_type })));
		return type;
	}
	case WTokenType::IS:
	{
		return get_boolean_type();
	}
	default:
	{
		FATAL("What the hell is this Binary statement?");
		break;
	}
	}

	return get_none_type();
}

Object_ptr ObjectSystem::infer(SymbolScope_ptr scope, Object_ptr operand_type, WTokenType op)
{
	switch (op)
	{
	case WTokenType::PLUS:
	case WTokenType::MINUS:
	{
		expect_number_type(operand_type);
		return is_int_type(operand_type) ? get_int_type() : get_float_type();
	}
	case WTokenType::BANG:
	{
		expect_boolean_type(operand_type);
		return get_boolean_type();
	}
	case WTokenType::TYPE_OF:
	{
		return operand_type;
	}
	default:
	{
		FATAL("What the hell is this unary statement?");
		break;
	}
	}

	return get_none_type();
}

// Is _ type?

bool ObjectSystem::is_boolean_type(const Object_ptr type) const
{
	return holds_alternative<BooleanType>(*type);
}

bool ObjectSystem::is_number_type(const Object_ptr type) const
{
	return holds_alternative<IntType>(*type) || holds_alternative<FloatType>(*type);
}

bool ObjectSystem::is_int_type(const Object_ptr type) const
{
	return holds_alternative<IntType>(*type);
}

bool ObjectSystem::is_float_type(const Object_ptr type) const
{
	return holds_alternative<FloatType>(*type);
}

bool ObjectSystem::is_string_type(const Object_ptr type) const
{
	return holds_alternative<StringType>(*type);
}

bool ObjectSystem::is_none_type(const Object_ptr type) const
{
	return holds_alternative<NoneType>(*type);
}

bool ObjectSystem::is_condition_type(SymbolScope_ptr scope, const Object_ptr condition_type) const
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

bool ObjectSystem::is_spreadable_type(SymbolScope_ptr scope, const Object_ptr candidate_type) const
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

		[&](TypeIdentifier const& type)
		{
			Symbol_ptr symbol = scope->lookup(type.name);
			return is_spreadable_type(scope, symbol->type);
		},

		[](auto) { return false; }
		}, *candidate_type);
}

bool ObjectSystem::is_iterable_type(SymbolScope_ptr scope, const Object_ptr candidate_type) const
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

bool ObjectSystem::is_key_type(SymbolScope_ptr scope, const Object_ptr key_type) const
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

bool ObjectSystem::is_class_type(const Object_ptr type) const
{
	return holds_alternative<ClassType>(*type);
}

bool ObjectSystem::is_interface_type(const Object_ptr type) const
{
	return holds_alternative<InterfaceType>(*type);
}

bool ObjectSystem::is_enum_type(const Object_ptr type) const
{
	return holds_alternative<EnumType>(*type);
}

bool ObjectSystem::is_function_type(const Object_ptr type) const
{
	return holds_alternative<FunctionType>(*type);
}

bool ObjectSystem::is_generator_type(const Object_ptr type) const
{
	return holds_alternative<GeneratorType>(*type);
}

bool ObjectSystem::is_function_method_type(const Object_ptr type) const
{
	return holds_alternative<FunctionMemberType>(*type);
}

bool ObjectSystem::is_generator_method_type(const Object_ptr type) const
{
	return holds_alternative<GeneratorMemberType>(*type);
}

// assert type

void ObjectSystem::expect_boolean_type(const Object_ptr type) const
{
	ASSERT(is_boolean_type(type), "Must be a BooleanType");
}

void ObjectSystem::expect_number_type(const Object_ptr type) const
{
	ASSERT(is_number_type(type), "Must be a Number Type");
}

void ObjectSystem::expect_int_type(const Object_ptr type) const
{
	ASSERT(is_int_type(type), "Must be a IntType");
}

void ObjectSystem::expect_float_type(const Object_ptr type) const
{
	ASSERT(is_float_type(type), "Must be a FloatType");
}

void ObjectSystem::expect_string_type(const Object_ptr type) const
{
	ASSERT(is_string_type(type), "Must be a StringType");
}

void ObjectSystem::expect_none_type(const Object_ptr type) const
{
	ASSERT(is_none_type(type), "Must be a NoneType");
}

void ObjectSystem::expect_class_type(const Object_ptr type) const
{
	ASSERT(is_class_type(type), "Must be a ClassType");
}

void ObjectSystem::expect_interface_type(const Object_ptr type) const
{
	ASSERT(is_interface_type(type), "Must be a InterfaceType");
}

void ObjectSystem::expect_enum_type(const Object_ptr type) const
{
	ASSERT(is_enum_type(type), "Must be a EnumType");
}

void ObjectSystem::expect_function_type(const Object_ptr type) const
{
	ASSERT(is_function_type(type), "Must be a FunctionType");
}

void ObjectSystem::expect_generator_type(const Object_ptr type) const
{
	ASSERT(is_generator_type(type), "Must be a GeneratorType");
}

void ObjectSystem::expect_function_method_type(const Object_ptr type) const
{
	ASSERT(is_function_method_type(type), "Must be a FunctionMemberType");
}

void ObjectSystem::expect_generator_method_type(const Object_ptr type) const
{
	ASSERT(is_generator_method_type(type), "Must be a GeneratorMemberType");
}

void ObjectSystem::expect_condition_type(SymbolScope_ptr scope, const Object_ptr type) const
{
	ASSERT(is_condition_type(scope, type), "Must be a Condition Type");
}

void ObjectSystem::expect_spreadable_type(SymbolScope_ptr scope, const Object_ptr type) const
{
	ASSERT(is_spreadable_type(scope, type), "Must be a Spreadable type");
}

void ObjectSystem::expect_iterable_type(SymbolScope_ptr scope, const Object_ptr type) const
{
	ASSERT(is_iterable_type(scope, type), "Must be a iterable Type");
}

void ObjectSystem::expect_key_type(SymbolScope_ptr scope, const Object_ptr type) const
{
	ASSERT(is_key_type(scope, type), "Must be a key Type");
}

// Extract type from variant

ClassType* ObjectSystem::extract_class_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<ClassType>(*type), "Must be a ClassType");
	auto inner_type = get_if<ClassType>(&*type);
	return inner_type;
}

InterfaceType* ObjectSystem::extract_interface_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<InterfaceType>(*type), "Must be a InterfaceType");
	auto inner_type = get_if<InterfaceType>(&*type);
	return inner_type;
}

EnumType* ObjectSystem::extract_enum_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<EnumType>(*type), "Must be a EnumType");1
	auto inner_type = get_if<EnumType>(&*type);
	return inner_type;
}  

FunctionType* ObjectSystem::extract_function_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<FunctionType>(*type), "Must be a FunctionType");
	auto inner_type = get_if<FunctionType>(&*type);
	return inner_type;
}

GeneratorType* ObjectSystem::extract_generator_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<GeneratorType>(*type), "Must be a GeneratorType");
	auto inner_type = get_if<GeneratorType>(&*type);
	return inner_type;
}

FunctionMemberType* ObjectSystem::extract_function_member_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<FunctionMemberType>(*type), "Must be a FunctionMemberType");
	auto inner_type = get_if<FunctionMemberType>(&*type);
	return inner_type;
}

GeneratorMemberType* ObjectSystem::extract_generator_member_type(const Object_ptr type) const
{
	ASSERT(holds_alternative<GeneratorMemberType>(*type), "Must be a GeneratorMemberType");
	auto inner_type = get_if<GeneratorMemberType>(&*type);
	return inner_type;
}
}