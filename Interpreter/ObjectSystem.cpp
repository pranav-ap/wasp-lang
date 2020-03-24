#pragma once
#include "pch.h"
#include <iostream>
#include <iomanip>
#include "ObjectSystem.h"

using std::cout;
using std::endl;
using std::setw;
using std::string;

void NumberObject::print(int level)
{
	cout << string(level, ' ') << "Number Object : " << this->value << endl;
}

void StringObject::print(int level)
{
	cout << string(level, ' ') << "String Object : " << this->value << endl;
}

void BooleanObject::print(int level)
{
	cout << string(level, ' ') << "Boolean Object : " << this->value << endl;
}

void OptionalObject::print(int level)
{
	if (this->value)
	{
		cout << string(level + 4, ' ') << "Optional Object : " << this->value.value() << endl;
	}
	else
		cout << string(level + 4, ' ') << "Optional Object : None" << endl;
}

void VectorObject::print(int level)
{
	cout << string(level, ' ') << "Vector Object : " << endl;

	for (auto const& value : this->values)
		value->print(level + 4);
}

void TupleObject::print(int level)
{
	cout << string(level, ' ') << "Tuple Object : " << endl;

	for (auto const& value : this->values)
		value->print(level + 4);
}

void MapObject::print(int level)
{
	cout << string(level, ' ') << "Map Object : " << endl;

	for (auto const& value : this->values)
	{
		cout << string(level + 4, ' ') << "Key : " << endl;
		value.first->print(level + 8);

		cout << string(level + 4, ' ') << "Value : " << endl;
		value.second->print(level + 8);

		cout << endl;
	}
}

void RecordObject::print(int level)
{
	cout << string(level, ' ') << "Record Object : " << endl;

	for (auto const& value : this->values)
	{
		cout << string(level + 4, ' ') << "Key : " << value.first << endl;

		cout << string(level + 4, ' ') << "Value : " << endl;
		value.second->print(level + 8);

		cout << endl;
	}
}