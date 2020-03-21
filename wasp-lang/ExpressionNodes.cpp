#pragma once
#include <iostream>
#include <iomanip>
#include <string>

#include "ExpressionNodes.h"
#include "Types.h"

using std::cout;
using std::endl;
using std::setw;
using std::string;

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
		expression->print(level + 4);
}

void VectorLiteral::print(int level)
{
	cout << string(level, ' ') << "Vector Literal : " << endl;

	for (auto const& expression : this->expressions)
		expression->print(level + 4);
}

void MapLiteral::print(int level)
{
	cout << string(level, ' ') << "Map Literal : " << endl;

	for (auto const& pair : this->pairs)
	{
		cout << string(level + 4, ' ') << "Key : " << endl;
		pair.first->print(level + 8);

		cout << string(level + 4, ' ') << "Value : " << endl;
		pair.second->print(level + 8);

		cout << endl;
	}
}

void RecordLiteral::print(int level)
{
	cout << string(level, ' ') << "Record Literal : " << endl;

	for (auto const& pair : this->pairs)
	{
		cout << string(level + 4, ' ') << "Key : " << pair.first << endl;

		cout << string(level + 4, ' ') << "Value : " << endl;
		pair.second->print(level + 8);

		cout << endl;
	}
}

void MemberAccess::print(int level)
{
	cout << string(level, ' ') << "Member Access : " << endl;

	cout << string(level, ' ') << "Key : " << this->name << endl;

	cout << string(level, ' ') << "Value : " << endl;
	this->expression->print(level + 4);
}

void Identifier::print(int level)
{
	cout << string(level, ' ') << "Identifier : " << this->name << endl;
}

void FunctionCall::print(int level)
{
	cout << string(level, ' ') << "Function Call : " << endl;

	cout << string(level + 4, ' ') << "Function Name : " << this->name << endl;

	if (this->arguments.size() > 0)
	{
		cout << string(level + 4, ' ') << "Arguments : " << endl;

		for (auto const& argument : this->arguments)
			argument->print(level + 8);
	}
	else
		cout << string(level + 4, ' ') << "Arguments : None" << endl;
}

void InclusiveRange::print(int level)
{
	cout << string(level, ' ') << "Inclusive Range : " << endl;

	cout << string(level + 4, ' ') << "Left : " << endl;
	left->print(level + 8);

	cout << string(level + 4, ' ') << "Right : " << endl;
	right->print(level + 8);
}

void ExclusiveRange::print(int level)
{
	cout << string(level, ' ') << "Exclusive Range : " << endl;

	cout << string(level + 4, ' ') << "Left : " << endl;
	left->print(level + 8);

	cout << string(level + 4, ' ') << "Right : " << endl;
	right->print(level + 8);
}

void Unary::print(int level)
{
	cout << string(level, ' ') << "Unary Operation : " << this->op->get_value() << endl;
	this->operand->print(level + 4);
}

void Binary::print(int level)
{
	cout << string(level, ' ') << "Binary Operation : " << this->op->get_value() << endl;
	this->left->print(level + 4);
	this->right->print(level + 4);
}