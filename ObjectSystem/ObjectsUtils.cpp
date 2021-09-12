#pragma once
#include "pch.h"
#include "Objects.h"
#include "Assertion.h"
#include <string>
#include <vector>
#include <exception>
#include <variant>
#include <algorithm>

#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define VOID std::make_shared<Object>(ReturnObject())
#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define THROW(message) return std::make_shared<Object>(ErrorObject(message));
#define THROW_IF(condition, message) if (!condition) { return std::make_shared<Object>(ErrorObject(message)); }
#define STR(x) to_wstring(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::string;
using std::wstring;
using std::vector;
using std::get;
using std::get_if;
using std::to_wstring;

ObjectVector to_vector(std::deque<Object_ptr> values)
{
	ObjectVector vec;

	for (auto value : values)
	{
		vec.push_back(value);
	}

	return vec;
}

ObjectVector to_vector(std::wstring text)
{
	ObjectVector vec;

	for (auto ch : text)
	{
		vec.push_back(MAKE_OBJECT_VARIANT(StringObject(std::to_wstring(ch))));
	}

	return vec;
}

bool are_equal_types(Object_ptr left, Object_ptr right)
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
			return left == right;
		},

		[&](TupleType const& type_1, TupleType const& type_2)
		{
			return left == right;
		},

		[&](SetType const& type_1, SetType const& type_2)
		{
			return left == right;
		},

		[&](VariantType const& type_1, VariantType const& type_2)
		{
			return are_equal_types_unordered(type_1.types, type_2.types);
		},

		[&](MapType const& type_1, MapType const& type_2)
		{
			bool key_compare = are_equal_types(type_1.key_type, type_2.key_type);
			bool value_compare = are_equal_types(type_1.value_type, type_2.value_type);
			return key_compare && value_compare;
		},

		[&](NoneType const& type_1, NoneType const& type_2) { return true; },

		[&](EnumType const& type_1, EnumType const& type_2)
		{
			return type_1.name == type_2.name;
		},

		[&](FunctionType const& type_1, FunctionType const& type_2)
		{
			return are_equal_types(type_1.input_types, type_2.input_types);
		},

		[](auto, auto) { return false; }
		}, *left, *right);
}

bool are_equal_types(ObjectVector left_vector, ObjectVector right_vector)
{
	int type_vector_1_length = left_vector.size();
	int type_vector_2_length = right_vector.size();

	if (type_vector_1_length != type_vector_2_length)
	{
		return false;
	}

	for (int index = 0; index < type_vector_1_length; index++)
	{
		auto type_1 = left_vector.at(index);
		auto type_2 = right_vector.at(index);

		if (!are_equal_types(type_1, type_2))
		{
			return false;
		}
	}

	return true;
}

bool are_equal_types_unordered(ObjectVector left_vector, ObjectVector right_vector)
{
	int type_vector_1_length = left_vector.size();
	int type_vector_2_length = right_vector.size();

	if (type_vector_1_length != type_vector_2_length)
	{
		return false;
	}

	for (auto left : left_vector)
	{
		bool any_equal = std::any_of(begin(right_vector), end(right_vector),
			[&](auto right)
			{
				return are_equal_types(left, right);
			}
		);

		if (!any_equal)
		{
			return false;
		}
	}

	return true;
}

Object_ptr convert_type(Object_ptr type, Object_ptr operand)
{
	return nullptr;
}

std::wstring stringify_object(Object_ptr value)
{
	return std::visit(overloaded{
		[&](IntObject const& obj) { return STR(obj.value); },
		[&](FloatObject const& obj) { return STR(obj.value); },
		[&](StringObject const& obj) { return obj.value; },
		[&](BooleanObject const& obj) { return STR(obj.value); },
		[&](ListObject const& obj) { return wstring(L"list object"); },
		[&](TupleObject const& obj) { return wstring(L"tuple object"); },
		[&](SetObject const& obj) { return wstring(L"set object"); },
		[&](MapObject const& obj) { return wstring(L"map object"); },
		[&](VariantObject const& obj) { return wstring(L"variant object"); },
		[&](ReturnObject const& obj) { return wstring(L"return"); },
		[&](ErrorObject const& obj) { return wstring(L"error : ") + obj.message; },
		[&](NoneObject const& obj) { return wstring(L"none"); },
		[&](BreakObject const& obj) { return wstring(L"break"); },
		[&](RedoObject const& obj) { return wstring(L"redo"); },
		[&](ContinueObject const& obj) { return wstring(L"continue"); },
		[&](IteratorObject const& obj) { return wstring(L"break"); },
		[&](EnumDefinitionObject const& obj) { return wstring(L"enum " + obj.name); },
		[&](EnumMemberObject const& obj) { return wstring(L"enum member" + obj.value); },
		[&](FunctionDefinitionObject const& obj) { return wstring(L"function " + obj.name); },
		[&](BuiltInFunctionObject const& obj) { return wstring(L"builtin " + obj.name); },

		// Types

		[&](AnyType const& obj) { return wstring(L"any type"); },
		[&](IntLiteralType const& obj) { return wstring(L"int literal type"); },
		[&](FloatLiteralType const& obj) { return wstring(L"float literal type"); },
		[&](StringLiteralType const& obj) { return wstring(L"string literal type"); },
		[&](BooleanLiteralType const& obj) { return wstring(L"boolean literal type"); },
		[&](IntType const& obj) { return wstring(L"int type"); },
		[&](FloatType const& obj) { return wstring(L"float type"); },
		[&](StringType const& obj) { return wstring(L"string type"); },
		[&](BooleanType const& obj) { return wstring(L"bool type"); },
		[&](ListType const& obj) { return wstring(L"list type"); },
		[&](TupleType const& obj) { return wstring(L"tuple type"); },
		[&](SetType const& obj) { return wstring(L"set type"); },
		[&](MapType const& obj) { return wstring(L"map type"); },
		[&](VariantType const& obj) { return wstring(L"variant type"); },
		[&](NoneType const& obj) { return wstring(L"none type"); },
		[&](FunctionType const& obj) { return wstring(L"function type"); },

		[&](auto) { return wstring(L" "); }
		}, *value);
}