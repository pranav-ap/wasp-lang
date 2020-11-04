#pragma once
#include "pch.h"
#include "TypeSystem.h"
#include "TypeNode.h"
#include "SymbolScope.h"
#include "Assertion.h"
#include <memory>
#include <algorithm>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_TYPE(x) std::make_shared<TypeNode>(x)

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

TypeSystem::TypeSystem()
{
	next_type_id = 0;

	type_pool.insert({ next_type_id++, MAKE_TYPE(BooleanTypeNode()) });
	type_pool.insert({ next_type_id++, MAKE_TYPE(IntTypeNode()) });
	type_pool.insert({ next_type_id++, MAKE_TYPE(FloatTypeNode()) });
	type_pool.insert({ next_type_id++, MAKE_TYPE(StringTypeNode()) });
	type_pool.insert({ next_type_id++, MAKE_TYPE(NoneTypeNode()) });
}

// equal

bool TypeSystem::equal(SymbolScope_ptr scope, const TypeNode_ptr type_1, const TypeNode_ptr type_2) const
{
	return std::visit(overloaded{
		[&](AnyTypeNode const& type_1, AnyTypeNode const& type_2) { return true; },

		[&](IntTypeNode const& type_1, IntTypeNode const& type_2) { return true; },
		[&](FloatTypeNode const& type_1, FloatTypeNode const& type_2) { return true; },
		[&](BooleanTypeNode const& type_1, BooleanTypeNode const& type_2) { return true; },
		[&](StringTypeNode const& type_1, StringTypeNode const& type_2) { return true; },

		[&](IntLiteralTypeNode const& type_1, IntLiteralTypeNode const& type_2) { return type_1.value == type_2.value; },
		[&](FloatLiteralTypeNode const& type_1, FloatLiteralTypeNode const& type_2) { return type_1.value == type_2.value; },
		[&](BooleanLiteralTypeNode const& type_1, BooleanLiteralTypeNode const& type_2) { return type_1.value == type_2.value; },
		[&](StringLiteralTypeNode const& type_1, StringLiteralTypeNode const& type_2) { return type_1.value == type_2.value; },

		[&](ListTypeNode const& type_1, ListTypeNode const& type_2)
		{
			return equal(scope, type_1.element_type, type_2.element_type);
		},

		[&](SetTypeNode const& type_1, SetTypeNode const& type_2)
		{
			return equal(scope, type_1.element_types, type_2.element_types);
		},

		[&](TupleTypeNode const& type_1, TupleTypeNode const& type_2)
		{
			return equal(scope, type_1.element_types, type_2.element_types);
		},

		[&](VariantTypeNode const& type_1, VariantTypeNode const& type_2)
		{
			return equal(scope, type_1.types, type_2.types);
		},

		[&](MapTypeNode const& type_1, MapTypeNode const& type_2)
		{
			bool key_compare = equal(scope, type_1.key_type, type_2.key_type);
			bool value_compare = equal(scope, type_1.value_type, type_2.value_type);
			return key_compare && value_compare;
		},

		[&](ClassTypeNode const& type_1, ClassTypeNode const& type_2)
		{
			Symbol_ptr symbol_1 = scope->lookup(type_1.name);
			NULL_CHECK(symbol_1);
			Symbol_ptr symbol_2 = scope->lookup(type_2.name);
			NULL_CHECK(symbol_2);

			return symbol_1->name == symbol_2->name;
		},

		[&](AliasTypeNode const& type_1, AliasTypeNode const& type_2)
		{
			Symbol_ptr symbol_1 = scope->lookup(type_1.name);
			NULL_CHECK(symbol_1);
			Symbol_ptr symbol_2 = scope->lookup(type_2.name);
			NULL_CHECK(symbol_2);

			return symbol_1->name == symbol_2->name;
		},

		[&](InterfaceTypeNode const& type_1, InterfaceTypeNode const& type_2)
		{
			Symbol_ptr symbol_1 = scope->lookup(type_1.name);
			NULL_CHECK(symbol_1);
			Symbol_ptr symbol_2 = scope->lookup(type_2.name);
			NULL_CHECK(symbol_2);

			return symbol_1->name == symbol_2->name;
		},

		[&](EnumTypeNode const& type_1, EnumTypeNode const& type_2)
		{
			Symbol_ptr symbol_1 = scope->lookup(type_1.enum_name);
			NULL_CHECK(symbol_1);
			Symbol_ptr symbol_2 = scope->lookup(type_2.enum_name);
			NULL_CHECK(symbol_2);

			return symbol_1->name == symbol_2->name;
		},

		[&](TypeIdentifierNode const& type_1, TypeIdentifierNode const& type_2)
		{
			Symbol_ptr symbol_1 = scope->lookup(type_1.name);
			NULL_CHECK(symbol_1);
			Symbol_ptr symbol_2 = scope->lookup(type_2.name);
			NULL_CHECK(symbol_2);

			return equal(scope, symbol_1->type, symbol_2->type);
		},

		[&](NoneTypeNode const& type_1, NoneTypeNode const& type_2) { return true; },

		[&](FunctionTypeNode const& type_1, FunctionTypeNode const& type_2) { return true; },
		[&](GeneratorTypeNode const& type_1, GeneratorTypeNode const& type_2) { return true; },

		[&](FunctionMemberTypeNode const& type_1, FunctionMemberTypeNode const& type_2) { return true; },
		[&](GeneratorMemberTypeNode const& type_1, GeneratorMemberTypeNode const& type_2) { return true; },

		[&](OperatorTypeNode const& type_1, OperatorTypeNode const& type_2) { return true; },

		[](auto, auto) { return false; }
		}, *type_1, *type_2);
}

bool TypeSystem::equal(SymbolScope_ptr scope, const TypeNodeVector type_vector_1, const TypeNodeVector type_vector_2) const
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

bool TypeSystem::assignable(SymbolScope_ptr scope, const TypeNode_ptr lhs_type, const TypeNode_ptr rhs_type) const
{
	if (equal(scope, lhs_type, rhs_type))
	{
		return true;
	}

	return std::visit(overloaded{
		[&](AnyTypeNode const& type_1, auto) { return true; },

		[&](IntTypeNode const& type_1, IntLiteralTypeNode const& type_2) { return true; },
		[&](FloatTypeNode const& type_1, FloatLiteralTypeNode const& type_2) { return true; },
		[&](BooleanTypeNode const& type_1, BooleanLiteralTypeNode const& type_2) { return true; },
		[&](StringTypeNode const& type_1, StringLiteralTypeNode const& type_2) { return true; },

		[&](IntLiteralTypeNode const& type_1, IntTypeNode const& type_2) { return true; },
		[&](FloatLiteralTypeNode const& type_1, FloatTypeNode const& type_2) { return true; },
		[&](BooleanLiteralTypeNode const& type_1, BooleanTypeNode const& type_2) { return true; },
		[&](StringLiteralTypeNode const& type_1, StringTypeNode const& type_2) { return true; },

		[&](ListTypeNode const& type_1, ListTypeNode const& type_2)
		{
			return assignable(scope, type_1.element_type, type_2.element_type);
		},

		[&](SetTypeNode const& type_1, SetTypeNode const& type_2)
		{
			return assignable(scope, type_1.element_types, type_2.element_types);
		},

		[&](TupleTypeNode const& type_1, TupleTypeNode const& type_2)
		{
			return assignable(scope, type_1.element_types, type_2.element_types);
		},

		[&](MapTypeNode const& type_1, MapTypeNode const& type_2)
		{
			bool key_compare = assignable(scope, type_1.key_type, type_2.key_type);
			bool value_compare = assignable(scope, type_1.value_type, type_2.value_type);
			return key_compare && value_compare;
		},

		[&](VariantTypeNode const& lhs_variant_type, auto)
		{
			return all_of(begin(lhs_variant_type.types), end(lhs_variant_type.types), [&](TypeNode_ptr type) { return assignable(scope, type, rhs_type); });
		},

		[](auto, auto) { return false; }
		}, *lhs_type, *rhs_type);
}

bool TypeSystem::assignable(SymbolScope_ptr scope, const TypeNodeVector type_vector_1, const TypeNodeVector type_vector_2) const
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

TypeNode_ptr TypeSystem::infer_type(SymbolScope_ptr scope, TypeNode_ptr left_type, WTokenType op, TypeNode_ptr right_type)
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

		auto type = this->allocate(scope, MAKE_TYPE(VariantTypeNode({ left_type, right_type })));
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

TypeNode_ptr TypeSystem::infer_type(SymbolScope_ptr scope, TypeNode_ptr operand_type, WTokenType op)
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

// TypeNode Pool

TypeNode_ptr TypeSystem::allocate(SymbolScope_ptr scope, TypeNode_ptr type)
{
	if (holds_alternative<TypeIdentifierNode>(*type), "Must be an TypeIdentifierNode")
	{
		auto type_identifier = get_if<TypeIdentifierNode>(&*type);
		auto symbol = scope->lookup(type_identifier->name);
		NULL_CHECK(symbol);
		return symbol->type;
	}

	for (auto const [id, value] : type_pool)
	{
		if (equal(scope, type, value))
		{
			return value;
		}
	}

	int id = next_type_id++;
	type_pool.insert({ id, type });
	return type;
}

TypeNode_ptr TypeSystem::get(int id)
{
	ASSERT(type_pool.contains(id), "TypeNode pool does not contain ID");
	return type_pool.at(id);
}

// Is _ type?

bool TypeSystem::is_boolean_type(const TypeNode_ptr type) const
{
	return holds_alternative<BooleanTypeNode>(*type);
}

bool TypeSystem::is_number_type(const TypeNode_ptr type) const
{
	return holds_alternative<IntTypeNode>(*type) || holds_alternative<FloatTypeNode>(*type);
}

bool TypeSystem::is_int_type(const TypeNode_ptr type) const
{
	return holds_alternative<IntTypeNode>(*type);
}

bool TypeSystem::is_float_type(const TypeNode_ptr type) const
{
	return holds_alternative<FloatTypeNode>(*type);
}

bool TypeSystem::is_string_type(const TypeNode_ptr type) const
{
	return holds_alternative<StringTypeNode>(*type);
}

bool TypeSystem::is_none_type(const TypeNode_ptr type) const
{
	return holds_alternative<NoneTypeNode>(*type);
}

bool TypeSystem::is_condition_type(SymbolScope_ptr scope, const TypeNode_ptr condition_type) const
{
	return std::visit(overloaded{
		[&](FunctionTypeNode const& type) { return false; },
		[&](GeneratorTypeNode const& type) { return false; },
		[&](FunctionMemberTypeNode const& type) { return false; },
		[&](GeneratorMemberTypeNode const& type) { return false; },
		[&](OperatorTypeNode const& type) { return false; },

		[&](TypeIdentifierNode const& type)
		{
			Symbol_ptr symbol = scope->lookup(type.name);
			return is_condition_type(scope, symbol->type);
		},

		[&](VariantTypeNode const& type)
		{
			return all_of(begin(type.types), end(type.types), [&](TypeNode_ptr t) { return is_condition_type(scope, t); });
		},

		[](auto) { return true; }
		}, *condition_type);
}

bool TypeSystem::is_spreadable_type(SymbolScope_ptr scope, const TypeNode_ptr candidate_type) const
{
	return std::visit(overloaded{
		[&](StringTypeNode const& type) { return true; },
		[&](ListTypeNode const& type) { return true; },
		[&](TupleTypeNode const& type) { return true; },
		[&](MapTypeNode const& type) { return true; },

		[&](VariantTypeNode const& type)
		{
			return all_of(begin(type.types), end(type.types), [&](TypeNode_ptr t) { return is_spreadable_type(scope, t); });
		},

		[&](TypeIdentifierNode const& type)
		{
			Symbol_ptr symbol = scope->lookup(type.name);
			return is_spreadable_type(scope, symbol->type);
		},

		[](auto) { return false; }
		}, *candidate_type);
}

bool TypeSystem::is_iterable_type(SymbolScope_ptr scope, const TypeNode_ptr candidate_type) const
{
	return std::visit(overloaded{
		[&](StringTypeNode const& type) { return true; },
		[&](ListTypeNode const& type) { return true; },
		[&](MapTypeNode const& type) { return true; },

		[&](VariantTypeNode const& type)
		{
			return all_of(begin(type.types), end(type.types), [&](TypeNode_ptr t) { return is_iterable_type(scope, t); });
		},

		[&](TypeIdentifierNode const& type)
		{
			Symbol_ptr symbol = scope->lookup(type.name);
			return is_iterable_type(scope, symbol->type);
		},

		[](auto) { return false; }
		}, *candidate_type);
}

bool TypeSystem::is_key_type(SymbolScope_ptr scope, const TypeNode_ptr key_type) const
{
	return std::visit(overloaded{
	[&](IntTypeNode const& type) { return true; },
	[&](FloatTypeNode const& type) { return true; },
	[&](BooleanTypeNode const& type) { return true; },
	[&](StringTypeNode const& type) { return true; },
	[&](IntLiteralTypeNode const& type) { return true; },
	[&](FloatLiteralTypeNode const& type) { return true; },
	[&](StringLiteralTypeNode const& type) { return true; },
	[&](BooleanLiteralTypeNode const& type) { return true; },
	[&](EnumTypeNode const& type) { return true; },

	[&](VariantTypeNode const& type)
	{
		return all_of(begin(type.types), end(type.types), [&](TypeNode_ptr t) { return is_key_type(scope, t); });
	},

	[&](TypeIdentifierNode const& type)
	{
		Symbol_ptr symbol = scope->lookup(type.name);
		return is_key_type(scope, symbol->type);
	},

	[](auto) { return false; }
		}, *key_type);
}

bool TypeSystem::is_class_type(const TypeNode_ptr type) const
{
	return holds_alternative<ClassTypeNode>(*type);
}

bool TypeSystem::is_interface_type(const TypeNode_ptr type) const
{
	return holds_alternative<InterfaceTypeNode>(*type);
}

bool TypeSystem::is_enum_type(const TypeNode_ptr type) const
{
	return holds_alternative<EnumTypeNode>(*type);
}

bool TypeSystem::is_function_type(const TypeNode_ptr type) const
{
	return holds_alternative<FunctionTypeNode>(*type);
}

bool TypeSystem::is_generator_type(const TypeNode_ptr type) const
{
	return holds_alternative<GeneratorTypeNode>(*type);
}

bool TypeSystem::is_function_method_type(const TypeNode_ptr type) const
{
	return holds_alternative<FunctionMemberTypeNode>(*type);
}

bool TypeSystem::is_generator_method_type(const TypeNode_ptr type) const
{
	return holds_alternative<GeneratorMemberTypeNode>(*type);
}

// assert type

void TypeSystem::expect_boolean_type(const TypeNode_ptr type) const
{
	ASSERT(is_boolean_type(type), "Must be a BooleanTypeNode");
}

void TypeSystem::expect_number_type(const TypeNode_ptr type) const
{
	ASSERT(is_number_type(type), "Must be a Number TypeNode");
}

void TypeSystem::expect_int_type(const TypeNode_ptr type) const
{
	ASSERT(is_int_type(type), "Must be a IntTypeNode");
}

void TypeSystem::expect_float_type(const TypeNode_ptr type) const
{
	ASSERT(is_float_type(type), "Must be a FloatTypeNode");
}

void TypeSystem::expect_string_type(const TypeNode_ptr type) const
{
	ASSERT(is_string_type(type), "Must be a StringTypeNode");
}

void TypeSystem::expect_none_type(const TypeNode_ptr type) const
{
	ASSERT(is_none_type(type), "Must be a NoneTypeNode");
}

void TypeSystem::expect_class_type(const TypeNode_ptr type) const
{
	ASSERT(is_class_type(type), "Must be a ClassTypeNode");
}

void TypeSystem::expect_interface_type(const TypeNode_ptr type) const
{
	ASSERT(is_interface_type(type), "Must be a InterfaceTypeNode");
}

void TypeSystem::expect_enum_type(const TypeNode_ptr type) const
{
	ASSERT(is_enum_type(type), "Must be a EnumTypeNode");
}

void TypeSystem::expect_function_type(const TypeNode_ptr type) const
{
	ASSERT(is_function_type(type), "Must be a FunctionTypeNode");
}

void TypeSystem::expect_generator_type(const TypeNode_ptr type) const
{
	ASSERT(is_generator_type(type), "Must be a GeneratorTypeNode");
}

void TypeSystem::expect_function_method_type(const TypeNode_ptr type) const
{
	ASSERT(is_function_method_type(type), "Must be a FunctionMemberTypeNode");
}

void TypeSystem::expect_generator_method_type(const TypeNode_ptr type) const
{
	ASSERT(is_generator_method_type(type), "Must be a GeneratorMemberTypeNode");
}

void TypeSystem::expect_condition_type(SymbolScope_ptr scope, const TypeNode_ptr type) const
{
	ASSERT(is_condition_type(scope, type), "Must be a Condition TypeNode");
}

void TypeSystem::expect_spreadable_type(SymbolScope_ptr scope, const TypeNode_ptr type) const
{
	ASSERT(is_spreadable_type(scope, type), "Must be a Spreadable type");
}

void TypeSystem::expect_iterable_type(SymbolScope_ptr scope, const TypeNode_ptr type) const
{
	ASSERT(is_iterable_type(scope, type), "Must be a iterable TypeNode");
}

void TypeSystem::expect_key_type(SymbolScope_ptr scope, const TypeNode_ptr type) const
{
	ASSERT(is_key_type(scope, type), "Must be a key TypeNode");
}

// Get primitive type

TypeNode_ptr TypeSystem::get_boolean_type() const
{
	return type_pool.at(0);
}

TypeNode_ptr TypeSystem::get_int_type() const
{
	return type_pool.at(1);
}

TypeNode_ptr TypeSystem::get_float_type() const
{
	return type_pool.at(2);
}

TypeNode_ptr TypeSystem::get_string_type() const
{
	return type_pool.at(3);
}

TypeNode_ptr TypeSystem::get_none_type() const
{
	return type_pool.at(4);
}

// Extract type from variant

ClassTypeNode* TypeSystem::extract_class_type(const TypeNode_ptr type) const
{
	ASSERT(holds_alternative<ClassTypeNode>(*type), "Must be a ClassTypeNode");
	auto inner_type = get_if<ClassTypeNode>(&*type);
	return inner_type;
}

InterfaceTypeNode* TypeSystem::extract_interface_type(const TypeNode_ptr type) const
{
	ASSERT(holds_alternative<InterfaceTypeNode>(*type), "Must be a InterfaceTypeNode");
	auto inner_type = get_if<InterfaceTypeNode>(&*type);
	return inner_type;
}

EnumTypeNode* TypeSystem::extract_enum_type(const TypeNode_ptr type) const
{
	ASSERT(holds_alternative<EnumTypeNode>(*type), "Must be a EnumTypeNode");
	auto inner_type = get_if<EnumTypeNode>(&*type);
	return inner_type;
}

FunctionTypeNode* TypeSystem::extract_function_type(const TypeNode_ptr type) const
{
	ASSERT(holds_alternative<FunctionTypeNode>(*type), "Must be a FunctionTypeNode");
	auto inner_type = get_if<FunctionTypeNode>(&*type);
	return inner_type;
}

GeneratorTypeNode* TypeSystem::extract_generator_type(const TypeNode_ptr type) const
{
	ASSERT(holds_alternative<GeneratorTypeNode>(*type), "Must be a GeneratorTypeNode");
	auto inner_type = get_if<GeneratorTypeNode>(&*type);
	return inner_type;
}

FunctionMemberTypeNode* TypeSystem::extract_function_member_type(const TypeNode_ptr type) const
{
	ASSERT(holds_alternative<FunctionMemberTypeNode>(*type), "Must be a FunctionMemberTypeNode");
	auto inner_type = get_if<FunctionMemberTypeNode>(&*type);
	return inner_type;
}

GeneratorMemberTypeNode* TypeSystem::extract_generator_member_type(const TypeNode_ptr type) const
{
	ASSERT(holds_alternative<GeneratorMemberTypeNode>(*type), "Must be a GeneratorMemberTypeNode");
	auto inner_type = get_if<GeneratorMemberTypeNode>(&*type);
	return inner_type;
}