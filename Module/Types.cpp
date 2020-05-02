#include "pch.h"
#include "Types.h"
#include <variant>

#define MAKE_TYPE(x) std::make_shared<Type>(x)

using std::string;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;
//
//string get_type_string(Type_ptr type)
//{
//	return std::visit(overloaded{
//			[](NumberType exp) { return "num"; },
//			[](StringType exp) { return "str"; },
//			[](BooleanType exp) { return "bool"; },
//			[&](ListType exp)
//			{
//				return "list[" + get_type_string(exp.element_type) + "]";
//			},
//			[&](TupleType exp)
//			{
//				string mangle = "tuple(";
//
//				for (auto element_type : exp.element_types)
//				{
//					mangle += get_type_string(element_type);
//				}
//
//				mangle += ")";
//
//				return mangle;
//			},
//			[](UDTType exp) { return exp.name; },
//			[&](MapType exp)
//			{
//				return
//					"map{" +
//					get_type_string(exp.key_type) +
//					"->" +
//					get_type_string(exp.value_type) +
//					"}";
//			},
//			[&](EnumType exp) { return "enum_" + exp.enum_name; },
//			[&](OptionalType exp) { return "opt_" + get_type_string(exp.type); },
//			[&](VariantType exp)
//			{
//				string mangle;
//
//				for (auto element_type : exp.types)
//				{
//					mangle += get_type_string(element_type) + "|";
//				}
//
//				return mangle;
//			},
//			[](AnyType exp) { return "any"; },
//
//			[](auto) { return "unknown"; }
//		}, *type);
//}