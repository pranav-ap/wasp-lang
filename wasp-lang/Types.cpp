#include <iostream>
#include <iomanip>
#include <variant>
#include <string>
#include "ExpressionNodes.h"
#include "Types.h"

using std::cout;
using std::endl;
using std::setw;
using std::visit;
using std::string;

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;

void print_type_node(TypeNode_ptr node, int level)
{
	visit(overload{
		[=](Optional node) { node.print(level); },
		[=](Variant node) { node.print(level); },
		[=](Number node) { node.print(level); },
		[=](String node) { node.print(level); },
		[=](Bool node) { node.print(level); },
		[=](Vector node) { node.print(level); },
		[=](Tuple node) { node.print(level); },
		[=](Map node) { node.print(level); },
		[=](Record node) { node.print(level); },
		[](std::monostate x) {}
		}, *node.get());
}

void print_key_type_node(KeyTypeNode_ptr node, int level)
{
	visit(overload{
		[=](StringLiteral node) { node.print(level); },
		[=](NumberLiteral node) { node.print(level); },
		[](std::monostate x) {}
		}, *node.get());
}

void Optional::print(int level)
{
	cout << string(level, ' ') << "Optional Type : " << setw(25) << std::left << endl;
	print_type_node(this->optional_type, level + 4);
}

void Variant::print(int level)
{
	cout << string(level, ' ') << "Variant Type : " << setw(25) << std::left << endl;;

	for (auto const& type : this->types)
	{
		print_type_node(type, level + 4);
	}
}

void Number::print(int level)
{
	cout << string(level, ' ') << "Number Type : " << endl;
}

void String::print(int level)
{
	cout << string(level, ' ') << "String Type : " << endl;
}

void Bool::print(int level)
{
	cout << string(level, ' ') << "Bool Type : " << endl;
}

void Tuple::print(int level)
{
	cout << string(level, ' ') << "Tuple Type : " << setw(25) << std::left << endl;;

	for (auto const& type : this->types)
	{
		print_type_node(type, level + 4);
	}
}

void Vector::print(int level)
{
	cout << string(level, ' ') << "Vector Type : " << setw(25) << std::left << endl;;
	print_type_node(this->type, level + 4);
}

void Map::print(int level)
{
	cout << string(level, ' ') << "Vector Type : " << setw(25) << std::left << endl;;
	print_key_type_node(this->key_type, level + 4);
	print_type_node(this->value_type, level + 4);
}

void Record::print(int level)
{
	cout << string(level, ' ') << "Record Type : " << setw(25) << std::left << this->type << endl;
}