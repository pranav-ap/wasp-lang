#pragma once
#include <iostream>
#include <iomanip>
#include <variant>
#include <string>
#include "ExpressionNodes.h"

using std::cout;
using std::endl;
using std::setw;
using std::visit;
using std::string;

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;

void print_expression_node(ExpressionNode_ptr node, int level)
{
	visit(overload{
		[=](StringLiteral node) { node.print(level); },
		[=](NumberLiteral node) { node.print(level); },
		[=](BooleanLiteral node) { node.print(level); },
		[=](TupleLiteral node) { node.print(level); },
		[=](VectorLiteral node) { node.print(level); },
		[=](MapLiteral node) { node.print(level); },
		[=](RecordLiteral node) { node.print(level); },
		[=](MemberAccess node) { node.print(level); },
		[=](RecordMemberAccess node) { node.print(level); },
		[=](Identifier node) { node.print(level); },
		[=](FunctionCall node) { node.print(level); },
		[=](Unary node) { node.print(level); },
		[=](Binary node) { node.print(level); },
		[](std::monostate x) {}
		}, *node.get());
}

void StringLiteral::print(int level)
{
	cout << string(level, ' ') << "String Literal : " << setw(25) << std::left << this->value << endl;
}

void NumberLiteral::print(int level)
{
	cout << string(level, ' ') << "Number Literal : " << setw(25) << std::left << this->value << endl;
}

void BooleanLiteral::print(int level)
{
	cout << string(level, ' ') << "Boolean Literal : " << setw(25) << std::left << this->value << endl;
}

void TupleLiteral::print(int level)
{
	cout << string(level, ' ') << "Tuple Literal : " << setw(25) << std::left << endl;

	for (auto const& expression : this->expressions)
	{
		print_expression_node(expression, level + 4);
	}
}

void VectorLiteral::print(int level)
{
	cout << string(level, ' ') << "Vector Literal : " << setw(25) << std::left << endl;

	for (auto const& expression : this->expressions)
	{
		print_expression_node(expression, level + 4);
	}
}

void MapLiteral::print(int level)
{
	cout << string(level, ' ') << "Map Literal : " << setw(25) << std::left << endl;

	for (auto const& pair : this->pairs)
	{
		print_key_type_node(pair.first, level + 4);
		print_expression_node(pair.second, level + 4);
	}
}

void RecordLiteral::print(int level)
{
	cout << string(level, ' ') << "Record Literal : " << setw(25) << std::left << endl;

	for (auto const& pair : this->pairs)
	{
		cout << string(level + 4, ' ') << "Key : " << setw(25) << std::left << pair.first << endl;
		print_expression_node(pair.second, level + 4);
	}
}

void MemberAccess::print(int level)
{
	cout << string(level, ' ') << "Member Access : " << setw(25) << std::left;
	cout << string(level, ' ') << "Key : " << this->name << endl;
}

void RecordMemberAccess::print(int level)
{
	cout << string(level, ' ') << "Record Member Access : " << setw(25) << std::left;
	cout << string(level, ' ') << "Key : " << this->record_name << endl;
	cout << string(level, ' ') << "Value : " << this->member_name << endl;
}

void Identifier::print(int level)
{
	cout << string(level, ' ') << "Identifier : " << setw(25) << std::left << this->name << endl;
}

void FunctionCall::print(int level)
{
	cout << string(level, ' ') << "Function Call : " << setw(25) << std::left;
	cout << string(level, ' ') << "Function Name : " << this->name << endl;

	for (auto const& argument : this->arguments)
	{
		print_expression_node(argument, level + 4);
	}
}

void Unary::print(int level)
{
	cout << string(level, ' ') << "Unary Operation : " << this->op->get_value() << endl;
	print_expression_node(this->operand, level + 4);
}

void Binary::print(int level)
{
	cout << string(level, ' ') << "Binary Operation : " << this->op->get_value() << endl;
	print_expression_node(this->left, level + 4);
	print_expression_node(this->right, level + 4);
}