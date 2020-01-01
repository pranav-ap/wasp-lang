#pragma once
#include <iostream>
#include <iomanip>
#include <variant>
#include <string>
#include <type_traits>
#include "Types.h"
#include "ExpressionNodes.h"

using std::cout;
using std::endl;
using std::setw;
using std::visit;
using std::string;

void print_type_node(TypeNode_ptr node, int level)
{
	visit([level](auto&& n) {
		using T = std::decay_t<decltype(n)>;

		if constexpr (std::is_same_v<T, Optional>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Variant>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Number>)
			n.print(level);
		else if constexpr (std::is_same_v<T, String>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Bool>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Vector>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Tuple>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Map>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Record>)
			n.print(level);
		}, *node.get());
}

void print_key_type_node(KeyTypeNode_ptr node, int level)
{
	visit([level](auto&& n) {
		using T = std::decay_t<decltype(n)>;

		if constexpr (std::is_same_v<T, StringLiteral>)
			n.print(level);
		else if constexpr (std::is_same_v<T, NumberLiteral>)
			n.print(level);
		}, *node.get());
}

void Optional::print(int level)
{
	cout << string(level, ' ') << "Type : Optional : " << endl;
	print_type_node(this->optional_type, level + 4);
}

void Variant::print(int level)
{
	cout << string(level, ' ') << "Type : Variant : " << endl;

	for (auto const& type : this->types)
	{
		print_type_node(type, level + 4);
	}
}

void Number::print(int level)
{
	cout << string(level, ' ') << "Type : Number" << endl;
}

void String::print(int level)
{
	cout << string(level, ' ') << "Type : String" << endl;
}

void Bool::print(int level)
{
	cout << string(level, ' ') << "Type : Bool" << endl;
}

void Tuple::print(int level)
{
	cout << string(level, ' ') << "Type : Tuple" << endl;

	for (auto const& type : this->types)
	{
		print_type_node(type, level + 4);
	}
}

void Vector::print(int level)
{
	cout << string(level, ' ') << "Type : Vector" << endl;
	print_type_node(this->type, level + 4);
}

void Map::print(int level)
{
	cout << string(level, ' ') << "Type : Map" << endl;

	cout << string(level + 4, ' ') << "Key Type" << endl;
	print_key_type_node(this->key_type, level + 8);

	cout << string(level + 4, ' ') << "Value Type" << endl;
	print_type_node(this->value_type, level + 8);
}

void Record::print(int level)
{
	cout << string(level, ' ') << "Type : Record : " << this->type << endl;
}