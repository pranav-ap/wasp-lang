#pragma once
#include "pch.h"
#include "Type.h"

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_SYMBOL(x) std::make_shared<Symbol>(x)
#define MAKE_TYPE(x) std::make_shared<Type>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::holds_alternative;
using std::wstring;
using std::get_if;
using std::make_shared;
using std::move;

std::wstring stringify_type(Type_ptr type)
{
	return std::visit(overloaded{
		[&](AnyType const& type) -> wstring { return L"_any"; },
		[&](IntType const& type)  -> wstring { return L"_int"; },
		[&](FloatType const& type)  -> wstring { return L"_float"; },
		[&](BooleanType const& type) -> wstring { return L"_bool"; },
		[&](StringType const& type) -> wstring { return L"_string"; },

		[&](ListType const& type) -> wstring
		{
			wstring str = stringify_type(type.element_type);
			return str;
		},
		[&](TupleType const& type) -> wstring
		{
			wstring str = stringify_type(type.element_types);
			return str;
		},
		[&](SetType const& type) -> wstring
		{
			wstring str = stringify_type(type.element_types);
			return str;
		},
		[&](VariantType const& type) -> wstring
		{
			wstring str = stringify_type(type.types);
			return str;
		},
		[&](MapType const& type) -> wstring
		{
			wstring str = stringify_type(type.key_type);
			str += stringify_type(type.value_type);
			return str;
		},

		[&](FunctionType const& type) -> wstring
		{
			wstring str = stringify_type(type.input_types);
			return str;
		},
		[&](GeneratorType const& type) -> wstring
		{
			wstring str = stringify_type(type.input_types);
			return str;
		},
		[&](OperatorType const& type) -> wstring
		{
			wstring str = L"_Operator";
			str += stringify_type(type.input_types);
			str += stringify_type(type.return_type);

			return str;
		},

		[&](ClassType const& type) -> wstring { return L"_" + type.name; },
		[&](EnumType const& type) -> wstring { return L"_" + type.enum_name; },
		[&](NoneType const& type) -> wstring { return L"_none"; },

		[&](IntLiteralType const& type) -> wstring { return L"_int_literal"; },
		[&](FloatLiteralType const& type) -> wstring { return L"_float_literal"; },
		[&](StringLiteralType const& type) -> wstring { return L"_string_literal"; },
		[&](BooleanLiteralType const& type) -> wstring { return L"_bool_literal"; },

		[](auto) -> wstring { return L"_"; }
		}, *type);
}

std::wstring stringify_type(std::vector<Type_ptr> types)
{
	wstring str = L"";

	for (auto const ty : types)
	{
		str += stringify_type(ty);
	}

	return str;
}