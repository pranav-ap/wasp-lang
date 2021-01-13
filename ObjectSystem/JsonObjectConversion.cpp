#pragma once
#include "pch.h"
#include "JsonObjectConversion.h"

using nlohmann::json;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

void to_json(nlohmann::json& j, const Object_ptr obj)
{
	to_json(j, *obj);
}

void from_json(const nlohmann::json& j, Object_ptr obj)
{
	from_json(j, *obj);
}

void to_json(nlohmann::json& j, const Object& o)
{
	std::visit(overloaded{
		[&](IntObject const& obj) { to_json(j, obj); },
		[&](FloatObject const& obj) { to_json(j, obj); },
		[&](StringObject const& obj) { to_json(j, obj); },
		[&](BooleanObject const& obj) { to_json(j, obj); },
		[&](ListObject const& obj) { to_json(j, obj); },
		[&](TupleObject const& obj) { to_json(j, obj); },
		[&](SetObject const& obj) { to_json(j, obj); },
		[&](MapObject const& obj) { to_json(j, obj); },
		[&](VariantObject const& obj) { to_json(j, obj); },
		[&](ReturnObject const& obj) { to_json(j, obj); },
		[&](YieldObject const& obj) { to_json(j, obj); },
		[&](BuiltInObject const& obj) { to_json(j, obj); },
		[&](ErrorObject const& obj) { to_json(j, obj); },
		[&](NoneObject const& obj) { to_json(j, obj); },
		[&](BreakObject const& obj) { to_json(j, obj); },
		[&](RedoObject const& obj) { to_json(j, obj); },
		[&](ContinueObject const& obj) { to_json(j, obj); },

		// Types

		[&](AnyType const& obj) { to_json(j, obj); },
		[&](IntLiteralType const& obj) { to_json(j, obj); },
		[&](FloatLiteralType const& obj) { to_json(j, obj); },
		[&](StringLiteralType const& obj) { to_json(j, obj); },
		[&](BooleanLiteralType const& obj) { to_json(j, obj); },
		[&](IntType const& obj) { to_json(j, obj); },
		[&](FloatType const& obj) { to_json(j, obj); },
		[&](StringType const& obj) { to_json(j, obj); },
		[&](BooleanType const& obj) { to_json(j, obj); },
		[&](ListType const& obj) { to_json(j, obj); },
		[&](TupleType const& obj) { to_json(j, obj); },
		[&](SetType const& obj) { to_json(j, obj); },
		[&](MapType const& obj) { to_json(j, obj); },
		[&](VariantType const& obj) { to_json(j, obj); },
		[&](NoneType const& obj) { to_json(j, obj); },

		[&](auto) {}
		}, o);
}

void from_json(const nlohmann::json& j, Object& o)
{
	std::visit(overloaded{
		[&](IntObject& obj) { from_json(j, obj); },
		[&](FloatObject& obj) { from_json(j, obj); },
		[&](StringObject& obj) { from_json(j, obj); },
		[&](BooleanObject& obj) { from_json(j, obj); },
		[&](ListObject& obj) { from_json(j, obj); },
		[&](TupleObject& obj) { from_json(j, obj); },
		[&](SetObject& obj) { from_json(j, obj); },
		[&](MapObject& obj) { from_json(j, obj); },
		[&](VariantObject& obj) { from_json(j, obj); },
		[&](ReturnObject& obj) { from_json(j, obj); },
		[&](YieldObject& obj) { from_json(j, obj); },
		[&](BuiltInObject& obj) { from_json(j, obj); },
		[&](ErrorObject& obj) { from_json(j, obj); },
		[&](NoneObject& obj) { from_json(j, obj); },
		[&](BreakObject& obj) { from_json(j, obj); },
		[&](RedoObject& obj) { from_json(j, obj); },
		[&](ContinueObject& obj) { from_json(j, obj); },

		// Types

		[&](AnyType& obj) { from_json(j, obj); },
		[&](IntLiteralType& obj) { from_json(j, obj); },
		[&](FloatLiteralType& obj) { from_json(j, obj); },
		[&](StringLiteralType& obj) { from_json(j, obj); },
		[&](BooleanLiteralType& obj) { from_json(j, obj); },
		[&](IntType& obj) { from_json(j, obj); },
		[&](FloatType& obj) { from_json(j, obj); },
		[&](StringType& obj) { from_json(j, obj); },
		[&](BooleanType& obj) { from_json(j, obj); },
		[&](ListType& obj) { from_json(j, obj); },
		[&](TupleType& obj) { from_json(j, obj); },
		[&](SetType& obj) { from_json(j, obj); },
		[&](MapType& obj) { from_json(j, obj); },
		[&](VariantType& obj) { from_json(j, obj); },
		[&](NoneType& obj) { from_json(j, obj); },

		[&](auto) {}
		}, o);
}

void to_json(nlohmann::json& j, const IntObject& o)
{
	j = json{ {"object_type", "IntObject"}, {"value", o.value } };
}

void from_json(const nlohmann::json& j, IntObject& o)
{
	j.at("value").get_to(o.value);
}

void to_json(nlohmann::json& j, const FloatObject& o)
{
	j = json{ {"object_type", "FloatObject"}, {"value", o.value } };
}

void from_json(const nlohmann::json& j, FloatObject& o)
{
	j.at("value").get_to(o.value);
}

void to_json(nlohmann::json& j, const StringObject& o)
{
	j = json{ {"object_type", "StringObject"}, {"value", o.value } };
}

void from_json(const nlohmann::json& j, StringObject& o)
{
	j.at("value").get_to(o.value);
}

void to_json(nlohmann::json& j, const BooleanObject& o)
{
	j = json{ {"object_type", "BooleanObject"}, {"value", o.value } };
}

void from_json(const nlohmann::json& j, BooleanObject& o)
{
	j.at("value").get_to(o.value);
}

void to_json(nlohmann::json& j, const NoneObject& o)
{
	j = json{ {"object_type", "NoneObject"} };
}

void from_json(const nlohmann::json& j, NoneObject& o)
{
}

void to_json(nlohmann::json& j, const ListObject& o)
{
	nlohmann::json j_values;

	for (const auto value : o.values)
	{
		nlohmann::json j_value = value;
		j_values.push_back(j_value);
	}

	j = json{ {"object_type", "ListObject"}, {"values", j_values} };
}

void from_json(const nlohmann::json& j, ListObject& o)
{
	j.at("values").get_to(o.values);
}

void to_json(nlohmann::json& j, const TupleObject& o)
{
	nlohmann::json j_values;

	for (const auto value : o.values)
	{
		nlohmann::json j_value = value;
		j_values.push_back(j_value);
	}

	j = json{ {"object_type", "TupleObject"}, {"values", j_values} };
}

void from_json(const nlohmann::json& j, TupleObject& o)
{
	j.at("values").get_to(o.values);
}

void to_json(nlohmann::json& j, const SetObject& o)
{
	nlohmann::json j_values;

	for (const auto value : o.values)
	{
		nlohmann::json j_value = value;
		j_values.push_back(j_value);
	}

	j = json{ {"object_type", "SetObject"}, {"values", j_values} };
}

void from_json(const nlohmann::json& j, SetObject& o)
{
	j.at("values").get_to(o.values);
}

void to_json(nlohmann::json& j, const MapObject& o)
{
	nlohmann::json j_values;

	for (const auto p : o.pairs)
	{
		auto key = p.first;
		auto value = p.second;

		nlohmann::json j_k_value = key;
		nlohmann::json j_v_value = value;
		nlohmann::json j_pair_value = json{ j_k_value, j_v_value };

		j_values.push_back(j_pair_value);
	}

	j = json{ {"object_type", "MapObject"}, {"values", j_values} };
}

void from_json(const nlohmann::json& j, MapObject& o)
{
}

void to_json(nlohmann::json& j, const VariantObject& o)
{
	j = json{ {"object_type", "VariantObject"} };
}

void from_json(const nlohmann::json& j, VariantObject& o)
{
}

void to_json(nlohmann::json& j, const ReturnObject& o)
{
	j = json{ {"object_type", "ReturnObject"} };
}

void from_json(const nlohmann::json& j, ReturnObject& o)
{
}

void to_json(nlohmann::json& j, const ErrorObject& o)
{
	j = json{ {"object_type", "ErrorObject"} };
}

void from_json(const nlohmann::json& j, ErrorObject& o)
{
}

void to_json(nlohmann::json& j, const YieldObject& o)
{
	j = json{ {"object_type", "YieldObject"} };
}

void from_json(const nlohmann::json& j, YieldObject& o)
{
}

void to_json(nlohmann::json& j, const RedoObject& o)
{
	j = json{ {"object_type", "RedoObject"} };
}

void from_json(const nlohmann::json& j, RedoObject& o)
{
}

void to_json(nlohmann::json& j, const BreakObject& o)
{
	j = json{ {"object_type", "BreakObject"} };
}

void from_json(const nlohmann::json& j, BreakObject& o)
{
}

void to_json(nlohmann::json& j, const ContinueObject& o)
{
	j = json{ {"object_type", "ContinueObject"} };
}

void from_json(const nlohmann::json& j, ContinueObject& o)
{
}

void to_json(nlohmann::json& j, const BuiltInObject& o)
{
	j = json{ {"object_type", "BuiltInObject"} };
}

void from_json(const nlohmann::json& j, BuiltInObject& o)
{
}

void to_json(nlohmann::json& j, const AnyType& o)
{
	j = json{ {"object_type", "AnyType"} };
}

void from_json(const nlohmann::json& j, AnyType& o)
{
}

void to_json(nlohmann::json& j, const IntLiteralType& o)
{
	j = json{ {"object_type", "IntLiteralType"} };
}

void from_json(const nlohmann::json& j, IntLiteralType& o)
{
}

void to_json(nlohmann::json& j, const FloatLiteralType& o)
{
	j = json{ {"object_type", "FloatLiteralType"} };
}

void from_json(const nlohmann::json& j, FloatLiteralType& o)
{
}

void to_json(nlohmann::json& j, const StringLiteralType& o)
{
	j = json{ {"object_type", "StringLiteralType"} };
}

void from_json(const nlohmann::json& j, StringLiteralType& o)
{
}

void to_json(nlohmann::json& j, const BooleanLiteralType& o)
{
	j = json{ {"object_type", "BooleanLiteralType"} };
}

void from_json(const nlohmann::json& j, BooleanLiteralType& o)
{
}

void to_json(nlohmann::json& j, const IntType& o)
{
	j = json{ {"object_type", "IntType"} };
}

void from_json(const nlohmann::json& j, IntType& o)
{
}

void to_json(nlohmann::json& j, const FloatType& o)
{
	j = json{ {"object_type", "FloatType"} };
}

void from_json(const nlohmann::json& j, FloatType& o)
{
}

void to_json(nlohmann::json& j, const StringType& o)
{
	j = json{ {"object_type", "StringType"} };
}

void from_json(const nlohmann::json& j, StringType& o)
{
}

void to_json(nlohmann::json& j, const BooleanType& o)
{
	j = json{ {"object_type", "BooleanType"} };
}

void from_json(const nlohmann::json& j, BooleanType& o)
{
}

void to_json(nlohmann::json& j, const ListType& o)
{
	j = json{ {"object_type", "ListType"} };
}

void from_json(const nlohmann::json& j, ListType& o)
{
}

void to_json(nlohmann::json& j, const TupleType& o)
{
	j = json{ {"object_type", "TupleType"} };
}

void from_json(const nlohmann::json& j, TupleType& o)
{
}

void to_json(nlohmann::json& j, const SetType& o)
{
	j = json{ {"object_type", "SetType"} };
}

void from_json(const nlohmann::json& j, SetType& o)
{
}

void to_json(nlohmann::json& j, const MapType& o)
{
	j = json{ {"object_type", "MapType"} };
}

void from_json(const nlohmann::json& j, MapType& o)
{
}

void to_json(nlohmann::json& j, const VariantType& o)
{
	j = json{ {"object_type", "VariantType"} };
}

void from_json(const nlohmann::json& j, VariantType& o)
{
}

void to_json(nlohmann::json& j, const NoneType& o)
{
	j = json{ {"object_type", "NoneType"} };
}

void from_json(const nlohmann::json& j, NoneType& o)
{
}