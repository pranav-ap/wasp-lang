#pragma once
#include <iostream>
#include <iomanip>
#include <variant>
#include <string>
#include <type_traits>
#include "ExpressionNodes.h"
#include "Types.h"

using std::cout;
using std::endl;
using std::setw;
using std::visit;
using std::string;

void print_expression_node(ExpressionNode_ptr node, int level)
{
	visit([level](auto&& n) {
		using T = std::decay_t<decltype(n)>;

		if constexpr (std::is_same_v<T, StringLiteral>)
			n.print(level);
		else if constexpr (std::is_same_v<T, NumberLiteral>)
			n.print(level);
		else if constexpr (std::is_same_v<T, BooleanLiteral>)
			n.print(level);
		else if constexpr (std::is_same_v<T, TupleLiteral>)
			n.print(level);
		else if constexpr (std::is_same_v<T, VectorLiteral>)
			n.print(level);
		else if constexpr (std::is_same_v<T, MapLiteral>)
			n.print(level);
		else if constexpr (std::is_same_v<T, RecordLiteral>)
			n.print(level);
		else if constexpr (std::is_same_v<T, MemberAccess>)
			n.print(level);
		else if constexpr (std::is_same_v<T, RecordMemberAccess>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Identifier>)
			n.print(level);
		else if constexpr (std::is_same_v<T, FunctionCall>)
			n.print(level);
		else if constexpr (std::is_same_v<T, InclusiveRange>)
			n.print(level);
		else if constexpr (std::is_same_v<T, ExclusiveRange>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Unary>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Binary>)
			n.print(level);
		}, *node.get());
}

void StringLiteral::print(int level)
{
	cout << string(level, ' ') << "String Literal : " << this->value << endl;
}

void NumberLiteral::print(int level)
{
	cout << string(level, ' ') << "Number Literal : " << this->value << endl;
}

void BooleanLiteral::print(int level)
{
	cout << string(level, ' ') << "Boolean Literal : " << this->value << endl;
}

void TupleLiteral::print(int level)
{
	cout << string(level, ' ') << "Tuple Literal : " << endl;

	for (auto const& expression : this->expressions)
	{
		print_expression_node(expression, level + 4);
	}
}

void VectorLiteral::print(int level)
{
	cout << string(level, ' ') << "Vector Literal : " << endl;

	for (auto const& expression : this->expressions)
	{
		print_expression_node(expression, level + 4);
	}
}

void MapLiteral::print(int level)
{
	cout << string(level, ' ') << "Map Literal : " << endl;

	for (auto const& pair : this->pairs)
	{
		print_expression_node(pair.first, level + 4);
		print_expression_node(pair.second, level + 4);
	}
}

void RecordLiteral::print(int level)
{
	cout << string(level, ' ') << "Record Literal : " << endl;

	for (auto const& pair : this->pairs)
	{
		cout << string(level + 4, ' ') << "Key : " << pair.first << endl;
		print_expression_node(pair.second, level + 4);
	}
}

void MemberAccess::print(int level)
{
	cout << string(level, ' ') << "Member Access : " << endl;

	cout << string(level, ' ') << "Key : " << this->name << endl;

	cout << string(level, ' ') << "Value : " << endl;
	print_expression_node(this->index_expression, level + 4);
}

void RecordMemberAccess::print(int level)
{
	cout << string(level, ' ') << "Record Member Access : " << endl;
	cout << string(level + 4, ' ') << "Key : " << this->record_name << endl;
	cout << string(level + 4, ' ') << "Value : " << this->member_name << endl;
}

void Identifier::print(int level)
{
	cout << string(level, ' ') << "Identifier : " << this->name << endl;
}

void FunctionCall::print(int level)
{
	cout << string(level, ' ') << "Function Call : " << endl;

	cout << string(level + 4, ' ') << "Function Name : " << this->name << endl;
	cout << string(level + 4, ' ') << "Arguments : " << endl;

	for (auto const& argument : this->arguments)
	{
		print_expression_node(argument, level + 8);
	}
}

void InclusiveRange::print(int level)
{
	cout << string(level, ' ') << "Inclusive Range : " << endl;

	cout << string(level + 4, ' ') << "Left : " << endl;
	print_expression_node(left, level + 8);

	cout << string(level + 4, ' ') << "Right : " << endl;
	print_expression_node(right, level + 8);
}

void ExclusiveRange::print(int level)
{
	cout << string(level, ' ') << "Exclusive Range : " << endl;

	cout << string(level + 4, ' ') << "Left : " << endl;
	print_expression_node(left, level + 8);

	cout << string(level + 4, ' ') << "Right : " << endl;
	print_expression_node(right, level + 8);
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