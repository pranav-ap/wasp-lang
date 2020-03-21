#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include "Types.h"
#include "ExpressionNodes.h"

using std::cout;
using std::endl;
using std::setw;
using std::string;

void Optional::print(int level)
{
	cout << string(level, ' ') << "Type : Optional : " << endl;
	this->optional_type->print(level + 4);
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
		type->print(level + 4);
	}
}

void Vector::print(int level)
{
	cout << string(level, ' ') << "Type : Vector" << endl;
	this->type->print(level + 4);
}

void Map::print(int level)
{
	cout << string(level, ' ') << "Type : Map" << endl;

	cout << string(level + 4, ' ') << "Key Type" << endl;
	this->key_type->print(level + 8);

	cout << string(level + 4, ' ') << "Value Type" << endl;
	this->value_type->print(level + 8);
}

void Record::print(int level)
{
	cout << string(level, ' ') << "Type : " << this->type << endl;
}